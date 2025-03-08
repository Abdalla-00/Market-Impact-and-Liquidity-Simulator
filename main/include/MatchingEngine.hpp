#ifndef MATCHING_ENGINE_HPP
#define MATCHING_ENGINE_HPP

#include <iostream>
#include <limits>
#include <string>
#include <random>
#include "cadmium/modeling/devs/atomic.hpp"
#include "TraderAgent.hpp"  // For Order

namespace custom_model {

struct MatchingEngineState {
    int liquidity;              // Available liquidity.
    double last_trade_price;    // Last traded price.
    bool halted;                // Whether the market is halted.
    double sigma;               // Time until next update.
    std::string phase;          // "Running" or "Halted"
    
    explicit MatchingEngineState():
         liquidity(1000),
         last_trade_price(50.0),
         halted(false),
         sigma(1.0), // Periodic update every second.
         phase("Running"){}
};

inline std::ostream& operator<<(std::ostream &out, const MatchingEngineState &state) {
    out << "{liquidity: " << state.liquidity 
        << ", last_trade_price: " << state.last_trade_price 
        << ", halted: " << state.halted 
        << ", phase: " << state.phase << "}";
    return out;
}

class MatchingEngine : public cadmium::Atomic<MatchingEngineState> {
public:
    cadmium::Port<Order> order_in;           // Input orders.
    cadmium::Port<std::string> reg_signal_in;  // Regulatory signals.
    cadmium::Port<double> shock_in;          // Shock events.
    cadmium::Port<Order> execution_out;      // Execution report.
    cadmium::Port<double> market_update_out; // Market update.
    
    MatchingEngine(const std::string &id)
        : Atomic<MatchingEngineState>(id, MatchingEngineState()) {
         order_in = addInPort<Order>("order_in");
         reg_signal_in = addInPort<std::string>("reg_signal_in");
         shock_in = addInPort<double>("shock_in");
         execution_out = addOutPort<Order>("execution_out");
         market_update_out = addOutPort<double>("market_update_out");
    }
    
    void externalTransition(MatchingEngineState& state, double e) const override {
         state.sigma -= e;
         if (!reg_signal_in->empty()) {
             std::string signal = reg_signal_in->getBag().back();
             if (signal == "halt") {
                 state.halted = true;
                 state.phase = "Halted";
             } else if (signal == "resume") {
                 state.halted = false;
                 state.phase = "Running";
             }
         }
         if (!shock_in->empty()) {
              double shock = shock_in->getBag().back();
              state.last_trade_price += shock;
              state.liquidity -= 200;
              state.sigma = 0.0;
              return;
         }
         if (state.phase == "Running" && !order_in->empty()) {
             Order order = order_in->getBag().back();
             static std::random_device rd;
             static std::mt19937 gen(rd());
             if (order.order_type == "sell") {
                 std::uniform_real_distribution<> dis(-6.0, -1.0);
                 double random_fluctuation = dis(gen);
                 state.last_trade_price += random_fluctuation;
             } else if (order.order_type == "buy") {
                 std::uniform_real_distribution<> dis(0.0, 3.0);
                 double random_fluctuation = dis(gen);
                 state.last_trade_price += (random_fluctuation + 0.5);
             }
             state.liquidity -= order.quantity;
             state.sigma = 0.0;
         }
    }
    
    void output(const MatchingEngineState& state) const override {
         if (!order_in->empty() && state.phase == "Running") {
             Order order = order_in->getBag().back();
             Order exec_order(order.order_type, order.quantity, state.last_trade_price);
             execution_out->addMessage(exec_order);
         }
         // Always send out the market update.
         market_update_out->addMessage(state.last_trade_price);
    }
    
    void internalTransition(MatchingEngineState& state) const override {
         // Periodically update every second.
         state.sigma = 1.0;
    }
    
    [[nodiscard]] double timeAdvance(const MatchingEngineState& state) const override {
         return state.sigma;
    }
    
    void confluentTransition(MatchingEngineState& state, double e) const override {
         internalTransition(state);
         externalTransition(state, 0);
    }
};

} // namespace custom_model

#endif // MATCHING_ENGINE_HPP
