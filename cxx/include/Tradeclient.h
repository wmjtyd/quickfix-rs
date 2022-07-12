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

  auto start() const -> void;
  auto stop() const -> void;

  auto put_order(const std::string &order_id, const std::string &quoteid,
                 const std::string &symbol, const std::string &currency,
                 const int side, const int quantity, const int price,
                 const int time_in_force) const -> void;

private:
  Application application;

  FIX::SessionSettings settings;
  FIX::FileStoreFactory store_factory;
  FIX::ScreenLogFactory log_factory;
  FIX::Initiator *initiator;
};

auto create_client(const std::string &filepath) -> std::unique_ptr<TradeClient>;

#endif
