#ifndef TRADECLIENT_H
#define TRADECLIENT_H

#include "include/Application.h"

#include "quickfix/FileStore.h"
#include "quickfix/Log.h"
#include "quickfix/SocketInitiator.h"
#include "quickfix/config.h"

#include <iostream>
#include <memory>

class TradeClient {
public:
  TradeClient(const std::string &filepath);
  ~TradeClient();

  bool run();
  void put_order(const std::string &quoteid, const std::string &symbol,
                 const std::string &currency, int side, int quantity, int price,
                 int time_in_force);

private:
  Application application;

  FIX::SessionSettings settings;
  FIX::FileStoreFactory store_factory;
  FIX::ScreenLogFactory log_factory;
  FIX::Initiator *initiator;
};

auto create_client(const std::string &filepath) -> std::unique_ptr<TradeClient>;

#endif
