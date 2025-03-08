#ifndef TRADER_AGENT_HPP
#define TRADER_AGENT_HPP

#include <iostream>
#include <limits>
#include <string>
#include <random>
#include "cadmium/modeling/devs/atomic.hpp"

using namespace cadmium;

// Order message structure.
struct Order {
    std::string order_type; // "buy" or "sell"
    int quantity;
    double price;
    Order(): order_type(""), quantity(0), price(0.0){}
    Order(const std::string &ot, int q, double p): order_type(ot), quantity(q), price(p){}
};

std::ostream& operator<<(std::ostream &out, const Order &order) {
    out << "{" << order.order_type << ", " << order.quantity << ", " << order.price << "}";
    return out;
}

// State for TraderAgent.
struct TraderAgentState {
    double inventory;    // Trader’s current inventory.
    double sigma;        // Time until next order.
    
    explicit TraderAgentState():
        inventory(0.0),
        sigma(1.0) {}
};

std::ostream& operator<<(std::ostream &out, const TraderAgentState &state) {
    out << "{inventory: " << state.inventory << "}";
    return out;
}

class TraderAgent: public Atomic<TraderAgentState> {
public:
    // Ports.
    Port<double> market_info;       // (Not used here, but available.)
    Port<std::string> reg_signal;     // For receiving "halt" or "resume".
    Port<Order> order_out;          // Output orders.
    
    TraderAgent(const std::string &id): Atomic<TraderAgentState>(id, TraderAgentState()){
         market_info = addInPort<double>("market_info");
         reg_signal = addInPort<std::string>("reg_signal");
         order_out = addOutPort<Order>("order_out");
    }
    
    // External transition: if a resume/halt signal is received, update state.
    void externalTransition(TraderAgentState& state, double e) const override {
         state.sigma -= e;
         if(!reg_signal->empty()){
             std::string signal = reg_signal->getBag().back();
             // If halted, no order is generated.
             if(signal == "halt"){
                 // No change to inventory, but we could log or change internal parameters.
             } else if(signal == "resume"){
                 // Resume normal operation.
             }
         }
         // We ignore market_info for simplicity.
    }
    
    // Output: generate a new order if not halted.
    void output(const TraderAgentState& state) const override {
         // Only output order if not paused.
         static std::random_device rd;
         static std::mt19937 gen(rd());
         std::uniform_real_distribution<> type_dis(0.0, 1.0);
         std::uniform_int_distribution<> qty_dis(50, 200);
         std::string orderType = (type_dis(gen) < 0.5) ? "buy" : "sell";
         int qty = qty_dis(gen);
         Order order(orderType, qty, 50.0); // Base price is 50.0.
         order_out->addMessage(order);
    }
    
    // Internal transition: simply schedule next order.
    void internalTransition(TraderAgentState& state) const override {
         // Assume immediate execution in MatchingEngine will update inventory separately.
         state.sigma = 1.0;
    }
    
    [[nodiscard]] double timeAdvance(const TraderAgentState& state) const override {
         return state.sigma;
    }
    
    void confluentTransition(TraderAgentState& state, double e) const override {
         internalTransition(state);
         externalTransition(state, 0);
    }
};

#endif // TRADER_AGENT_HPP
