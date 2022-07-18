#include "Tradeclient.h"
#include "Application.h"

#include "../../src/getopt-repl.h"
#include "quickfix/FileStore.h"
#include "quickfix/Log.h"
#include "quickfix/SessionSettings.h"
#include "quickfix/SocketInitiator.h"
#include "quickfix/config.h"

#include <fstream>
#include <iostream>
#include <string>

TradeClientApifiny::TradeClientApifiny(
    const std::string &filepath, rust::Box<TradeClientContext> ctx,
    rust::Fn<void(const QuickFixMessage, const FIX::SessionID &,
                  const rust::Box<TradeClientContext> &)>
        inbound_callback)
    : application(std::move(ctx), inbound_callback), settings(filepath),
      store_factory(settings), log_factory(settings),
      initiator(new FIX::SocketInitiator(application, store_factory, settings,
                                         log_factory)) {}

TradeClientApifiny::~TradeClientApifiny() {
  delete this->initiator;
  this->initiator = nullptr;
}

auto TradeClientApifiny::start() const -> void {
  try {
    this->initiator->start();
  } catch (std::exception &e) {
    std::cout << e.what() << std::endl;
  }
}

auto TradeClientApifiny::stop() const -> void { this->initiator->stop(); }

auto TradeClientApifiny::put_order(const std::string &quoteid,
                                   const std::string &symbol,
                                   const std::string &currency,
                                   const uint32_t side, const uint32_t quantity,
                                   const uint32_t price,
                                   const uint32_t time_in_force) const
    -> std::unique_ptr<std::string> {
  return this->application.new_order_single(symbol, side, quantity, price,
                                            time_in_force);
}

auto create_client(uint32_t type, const std::string &filepath,
                   rust::Box<TradeClientContext> ctx,
                   rust::Fn<void(const QuickFixMessage, const FIX::SessionID &,
                                 const rust::Box<TradeClientContext> &)>
                       inbound_callback) -> std::unique_ptr<ITradeClient> {
  ITradeClient *pitradeclient = nullptr;
  if (TradeClient_Apifiny == type) {
    pitradeclient =
        new TradeClientApifiny(filepath, std::move(ctx), inbound_callback);
  }

  return std::unique_ptr<ITradeClient>(pitradeclient);
}
