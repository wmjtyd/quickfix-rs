#pragma once

#include "ApplicationCCApi.h"
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
  TradeClientCCApi(const std::string exchangeName, const std::string &filepath, FromAppCallback cb);
  TradeClientCCApi(const std::string exchangeName, const std::string &filepath, FromAppCallbackExecutionReport cb);
#endif
  ~TradeClientCCApi();

  static bool eventHandler(void *obj, const ccapi::Event &event,
                           ccapi::Session *session);

  auto start() const -> void override;
  auto stop() const -> void override;

  auto put_order(const std::string &symbol, char side, 
                 const double quantity,const double price, const double stop_price,
                 const char order_type,const char time_in_force) const
      -> std::unique_ptr<std::string> override;
  auto cancel_order(const std::string &symbol, const std::string &order_id) const -> void override;

  auto put_order(const NewOrderSingle &aNewOrderSingle) const -> std::unique_ptr<std::string> override;
  auto cancel_order(const OrderCancelRequest &aOrderCancelRequest) const -> void override;
  auto cancel_open_orders(const std::string &symbol) const -> void override;

  auto get_order(const std::string &symbol, const std::string &order_id) const -> void override;
  auto get_open_orders(const std::string &symbol) const -> void override;

  auto get_recent_trades(const std::string &symbol) const -> void override;

  auto get_account_balances() const -> void override;

  auto get_accounts() const -> void override;
  auto get_account_postions() const -> void override;

private:
  static std::string convert_to_fix(const std::string &value, ccapi::Element elem);
  static char convert_exec_type_to_fix(ccapi::Message::Type type);

private:
  ApplicationCCApi application;
  FromAppCallback eventCallback;
  FromAppCallbackExecutionReport executionReportCallback;
#ifdef USE_TRADECLIENT_RUST_INTERFACE
  rust::Box<TradingClientContext> ctx;
  rust::Fn<void(const QuickFixMessage, const rust::Box<TradingClientContext> &)>
      inbound_callback;
#endif
};
