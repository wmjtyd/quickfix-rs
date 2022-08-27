#include "Tradeclient.h"
#include "Application.h"
#include "quickfix-rs/src/lib.rs.h"

#include "../../src/getopt-repl.h"


#include <fstream>
#include <iostream>
#include <string>

TradeClientCCApi::TradeClientCCApi(
    const std::string &filepath, rust::Box<TradingClientContext> ctx,
    rust::Fn<void(const QuickFixMessage, const rust::Box<TradingClientContext> &)>
        inbound_callback)
    : application(std::move(ctx), inbound_callback) {}

TradeClientCCApi::~TradeClientCCApi() {
  // delete this->initiator;
  // this->initiator = nullptr;
}

auto TradeClientCCApi::start() -> void {
  try {
    // this->initiator->start();
    this->application.start();
  } catch (std::exception &e) {
    std::cout << e.what() << std::endl;
  }
}

auto TradeClientCCApi::stop() const -> void { 
  // this->initiator->stop(); 
  this->application.stop();
}

auto TradeClientCCApi::put_order(const std::string &symbol, char side,
                                   const double quantity, const double price,
                                   const char time_in_force) const
    -> std::unique_ptr<std::string> {
  return this->application.new_order_single(symbol, side, quantity, price,
                                            time_in_force);
}

auto TradeClientCCApi::cancel_order(const std::string &order_id) const
    -> void {
  this->application.cancel_order(order_id);
}

/*auto create_client_ccapi(
    const TradingClientType type, const std::string &filepath,
    rust::Box<TradingClientContext> ctx,
    rust::Fn<void(const QuickFixMessage, const rust::Box<TradingClientContext> &)>
        inbound_callback) -> std::unique_ptr<ITradeClient> {
  ITradeClient *pitradeclient = nullptr;
  switch (type) {
  case TradingClientType::Apifiny:
    pitradeclient =
        new TradeClientApifiny(filepath, std::move(ctx), inbound_callback);
    break;
  case TradingClientType::Wintmute:
    break;
  }

  return std::unique_ptr<ITradeClient>(pitradeclient);
}*/
