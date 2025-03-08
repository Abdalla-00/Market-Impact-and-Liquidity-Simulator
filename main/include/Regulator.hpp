#ifndef REGULATOR_HPP
#define REGULATOR_HPP

#include <iostream>
#include <limits>
#include <string>
#include "cadmium/modeling/devs/atomic.hpp"

namespace custom_model {

struct RegulatorState {
    double last_price;       // Last observed market price.
    bool halt_active;        // Whether a halt is active.
    double halt_duration;    // How long the halt lasts.
    bool halt_signal_sent;   // Flag for output.
    double sigma;            // Time advance.
    std::string phase;       // "Monitoring" or "HaltingPeriod"
    
    explicit RegulatorState():
         last_price(50.0),
         halt_active(false),
         halt_duration(5.0),
         halt_signal_sent(false),
         sigma(1.0),  // Check every second.
         phase("Monitoring"){}
};

inline std::ostream& operator<<(std::ostream &out, const RegulatorState &state) {
    out << "{last_price: " << state.last_price 
        << ", halt_active: " << state.halt_active 
        << ", phase: " << state.phase << "}";
    return out;
}

class Regulator : public cadmium::Atomic<RegulatorState> {
public:
    cadmium::Port<double> market_update_in;   // Input: market updates.
    cadmium::Port<std::string> reg_signal_out;  // Output: "halt" or "resume".
    
    Regulator(const std::string &id)
        : Atomic<RegulatorState>(id, RegulatorState()){
         market_update_in = addInPort<double>("market_update_in");
         reg_signal_out = addOutPort<std::string>("reg_signal_out");
    }
    
    void externalTransition(RegulatorState& state, double e) const override {
         state.sigma -= e;
         if(!market_update_in->empty()){
             double new_price = market_update_in->getBag().back();
             if((state.last_price - new_price) >= 1.0 && state.phase == "Monitoring"){
                 state.phase = "HaltingPeriod";
                 state.sigma = 0.0;
                 state.halt_active = true;
             }
             state.last_price = new_price;
         }
    }
    
    void output(const RegulatorState& state) const override {
         if(state.phase == "HaltingPeriod"){
             if(!state.halt_signal_sent)
                 reg_signal_out->addMessage("halt");
             else
                 reg_signal_out->addMessage("resume");
         }
    }
    
    void internalTransition(RegulatorState& state) const override {
         if(state.phase == "HaltingPeriod"){
             if(!state.halt_signal_sent){
                 state.halt_signal_sent = true;
                 state.sigma = state.halt_duration;
             } else {
                 state.phase = "Monitoring";
                 state.halt_active = false;
                 state.halt_signal_sent = false;
                 state.sigma = 1.0;
             }
         } else {
             state.sigma = 1.0;
         }
    }
    
    [[nodiscard]] double timeAdvance(const RegulatorState& state) const override {
         return state.sigma;
    }
    
    void confluentTransition(RegulatorState& state, double e) const override {
         internalTransition(state);
         externalTransition(state, 0);
    }
};

} // namespace custom_model

#endif // REGULATOR_HPP
