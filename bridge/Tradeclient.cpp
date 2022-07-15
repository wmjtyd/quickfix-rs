#include "include/Tradeclient.h"

#include "../vendor/quickfix-cpp/src/getopt-repl.h"
#include "quickfix/SessionSettings.h"

#include <fstream>
#include <iostream>
#include <string>

#include "quickfix-rs/src/lib.rs.h"

TradeClient::TradeClient(
    const std::string &filepath, rust::Box<ApplicationContext> app_ctx,
    rust::Fn<void(const QuickFixMessage, const FIX::SessionID &,
                  const rust::Box<ApplicationContext> &)>
        inbound_callback)
    : application(std::move(app_ctx), inbound_callback), settings(filepath),
      store_factory(settings), log_factory(settings),
      initiator(new FIX::SocketInitiator(application, store_factory, settings,
                                         log_factory)) {}

TradeClient::~TradeClient() {
  delete this->initiator;
  this->initiator = nullptr;
}

auto TradeClient::start() const -> void {
  try {
    this->initiator->start();
  } catch (std::exception &e) {
    std::cout << e.what() << std::endl;
  }
}

auto TradeClient::stop() const -> void { this->initiator->stop(); }

auto TradeClient::put_order(const std::string &order_id,
                            const std::string &quoteid,
                            const std::string &symbol,
                            const std::string &currency, const uint32_t side,
                            const uint32_t quantity, const uint32_t price,
                            const uint32_t time_in_force) const -> void {
  this->application.new_order_single(order_id, symbol, side, quantity, price,
                                     time_in_force);
}

auto create_client(const std::string &filepath,
                   rust::Box<ApplicationContext> app_ctx,
                   rust::Fn<void(const QuickFixMessage, const FIX::SessionID &,
                                 const rust::Box<ApplicationContext> &)>
                       inbound_callback) -> std::unique_ptr<TradeClient> {
  TradeClient *trade_client = nullptr;
  try {
    trade_client =
        new TradeClient(filepath, std::move(app_ctx), inbound_callback);
  } catch (std::exception &e) {
    std::cout << e.what() << std::endl;
  }

  return std::unique_ptr<TradeClient>(trade_client);
}
