#pragma once

#include "Application.h"
#include "ITradeclient.h"
#ifdef USE_TRADECLIENT_RUST_INTERFACE
#include "rust/cxx.h"
#endif

#include <iostream>
#include <memory>

struct TradingClientContext;

class TradeClientCCApi : public ITradeClient {
public:
#ifdef USE_TRADECLIENT_RUST_INTERFACE
  TradeClientCCApi(const std::string &filepath,
                   rust::Box<TradingClientContext> ctx,
                   rust::Fn<void(const QuickFixMessage,
                                 const rust::Box<TradingClientContext> &)>
                       inbound_callback);
#else
  TradeClientCCApi(const std::string &filepath);
#endif
  ~TradeClientCCApi();

  static bool eventHandler(void *obj, const ccapi::Event &event,
                           ccapi::Session *session);

  auto start() const -> void override;
  auto stop() const -> void override;

  auto put_order(const std::string &symbol, char side, const double quantity,
                 const double price, const char time_in_force) const
      -> std::unique_ptr<std::string> override;
  auto cancel_order(const std::string &order_id) const -> void override;

private:
  ApplicationCCApi application;
#ifdef USE_TRADECLIENT_RUST_INTERFACE
  rust::Box<TradingClientContext> ctx;
  rust::Fn<void(const QuickFixMessage, const rust::Box<TradingClientContext> &)>
      inbound_callback;
#endif
};
