#ifndef ITRADECLIENT_H
#define ITRADECLIENT_H
#include "quickfix/Values.h"

const int TradeClient_Apifiny = 1;
const int TradeClient_Wintmut = 2;
#include "common.h"
class ITradeClient {
public:
//    TradeClient(const std::string &filepath) {};
//    ~TradeClient() ;

    virtual auto start() const -> void = 0;
    virtual auto stop() const -> void = 0;

    virtual auto put_order(const std::string &order_id, const std::string &quoteid,
                   const std::string &symbol, const std::string &currency,
                   const int side, const int quantity, const int price,
                   const int time_in_force) const -> void = 0;

};

ITradeClient* create_client(int type, const std::string &filepath, const FromAppCallback fromAppCallback);
#endif