#ifndef SHOCK_EVENT_HPP
#define SHOCK_EVENT_HPP

#include <iostream>
#include <limits>
#include "cadmium/modeling/devs/atomic.hpp"

using namespace cadmium;

struct ShockEventState {
    double sigma; // Time until shock event.
    bool event_occurred;
    
    explicit ShockEventState():
       sigma(30.0), // Shock event scheduled at 30 seconds.
       event_occurred(false){}
};

std::ostream& operator<<(std::ostream &out, const ShockEventState &state) {
    out << "{sigma: " << state.sigma << ", event_occurred: " << state.event_occurred << "}";
    return out;
}

class ShockEvent: public Atomic<ShockEventState> {
public:
    Port<double> shock_out;  // Output shock value.
    
    ShockEvent(const std::string &id): Atomic<ShockEventState>(id, ShockEventState()){
         shock_out = addOutPort<double>("shock_out");
    }
    
    void externalTransition(ShockEventState& state, double e) const override {
         state.sigma -= e;
    }
    
    void output(const ShockEventState& state) const override {
         if(!state.event_occurred){
              shock_out->addMessage(-10.0); // Force a shock of -10.
         }
    }
    
    void internalTransition(ShockEventState& state) const override {
         state.event_occurred = true;
         state.sigma = std::numeric_limits<double>::infinity(); // No further events.
    }
    
    [[nodiscard]] double timeAdvance(const ShockEventState& state) const override {
         return state.sigma;
    }
    
    void confluentTransition(ShockEventState& state, double e) const override {
         internalTransition(state);
         externalTransition(state, 0);
    }
};

#endif // SHOCK_EVENT_HPP