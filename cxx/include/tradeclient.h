#ifndef TRADECLIENT_H
#define TRADECLIENT_H

#include "application.h"

#include "../../vendor/quickfix-cpp/include/quickfix/SocketInitiator.h"

#include <iostream>
#include <memory>

class TradeClient {
public:
  TradeClient *create_client(const std::string &filepath);
  int run();
  void put_order(const std::string &quoteid, const std::string &symbol,
                 const std::string &currency, int side, int quantity, int price,
                 int time_in_force);

private:
  Application *mApplication;
  FIX::Initiator *mInitiator;
};

auto create_client(const std::string &filepath) -> std::unique_ptr<TradeClient>;

#endif
