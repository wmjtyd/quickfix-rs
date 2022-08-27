#pragma once

#include "Application.h"
#include "ITradeclient.h"

#include "rust/cxx.h"

#include <iostream>
#include <memory>

struct TradingClientContext;

class TradeClientCCApi : public ITradeClient {
public:
  TradeClientCCApi(const std::string &filepath,
                     rust::Box<TradingClientContext> ctx,
                     rust::Fn<void(const QuickFixMessage,
                                   const rust::Box<TradingClientContext> &)>
                         inbound_callback);
  ~TradeClientCCApi();

  auto start() -> void override;
  auto stop() const -> void override;

  auto put_order(const std::string &symbol, char side, const double quantity,
                 const double price, const char time_in_force) const
      -> std::unique_ptr<std::string> override;
  auto cancel_order(const std::string &order_id) const -> void override;

private:
  ApplicationCCApi application;
};
