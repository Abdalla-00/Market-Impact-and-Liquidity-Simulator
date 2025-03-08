#ifndef TOP_MODEL_HPP
#define TOP_MODEL_HPP

#include <cadmium/modeling/devs/coupled.hpp>
#include "TradingMatchingModule.hpp"
#include "Regulator.hpp"

using namespace cadmium;
using namespace std;
using namespace custom_model;

struct TopModel: public Coupled {
    TopModel(const std::string &id): Coupled(id) {
         auto tradingModule = addComponent<TradingMatchingModule>("TradingMatchingModule");
         auto regulator = addComponent<Regulator>("Regulator");
         
         addCoupling(tradingModule->market_feed, regulator->market_update_in);
         addCoupling(regulator->reg_signal_out, tradingModule->reg_control);
    }
};

#endif // TOP_MODEL_HPP
