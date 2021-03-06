#pragma once

#include "Application.h"
#include "ITradeclient.h"

#include "quickfix/FileStore.h"
#include "quickfix/Log.h"
#include "quickfix/SocketInitiator.h"
#include "quickfix/config.h"
#include "rust/cxx.h"

#include <iostream>
#include <memory>

struct TradingClientContext;

class TradeClientApifiny : public ITradeClient {
public:
  TradeClientApifiny(const std::string &filepath,
                     rust::Box<TradingClientContext> ctx,
                     rust::Fn<void(const QuickFixMessage,
                                   const rust::Box<TradingClientContext> &)>
                         inbound_callback);
  ~TradeClientApifiny();

  auto start() const -> void override;
  auto stop() const -> void override;

  auto put_order(const std::string &symbol, char side, const double quantity,
                 const double price, const char time_in_force) const
      -> std::unique_ptr<std::string> override;
  auto cancel_order(const std::string &order_id) const -> void override;

private:
  Application application;

  FIX::SessionSettings settings;
  FIX::FileStoreFactory store_factory;
  FIX::ScreenLogFactory log_factory;
  FIX::Initiator *initiator;
};
