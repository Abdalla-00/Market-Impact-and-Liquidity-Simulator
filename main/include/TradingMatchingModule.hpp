#ifndef TRADING_MATCHING_MODULE_HPP
#define TRADING_MATCHING_MODULE_HPP

#include <cadmium/modeling/devs/coupled.hpp>
#include "TraderAgent.hpp"
#include "MatchingEngine.hpp"
#include "ShockEvent.hpp"

using namespace cadmium;
using namespace std;
using namespace custom_model;

struct TradingMatchingModule: public Coupled {
    Port<std::string> reg_control; // External input for regulatory signals.
    Port<double> market_feed;      // External output for market updates.
    
    TradingMatchingModule(const std::string &id): Coupled(id) {
         reg_control = addInPort<std::string>("reg_control");
         market_feed = addOutPort<double>("market_feed");
         
         auto trader = addComponent<TraderAgent>("TraderAgent");
         auto engine = addComponent<MatchingEngine>("MatchingEngine");
         auto shockGen = addComponent<ShockEvent>("ShockEvent");
         
         addCoupling(trader->order_out, engine->order_in);
         addCoupling(engine->market_update_out, trader->market_info);
         addCoupling(shockGen->shock_out, engine->shock_in);
         
         addCoupling(reg_control, trader->reg_signal);
         addCoupling(reg_control, engine->reg_signal_in);
         addCoupling(engine->market_update_out, market_feed);
    }
};

#endif // TRADING_MATCHING_MODULE_HPP
