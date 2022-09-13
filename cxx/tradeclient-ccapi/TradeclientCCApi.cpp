#include "TradeclientCCApi.h"
#include "ApplicationCCApi.h"

#ifdef USE_TRADECLIENT_RUST_INTERFACE
#include "quickfix-rs/src/lib.rs.h"

#include "../../src/getopt-repl.h"
#endif


#include <fstream>
#include <iostream>
#include <string>

// #include "quickfix/FixValues.h"

#ifdef USE_TRADECLIENT_RUST_INTERFACE
TradeClientCCApi::TradeClientCCApi(
    const std::string &filepath, rust::Box<TradingClientContext> ctx,
    rust::Fn<void(const QuickFixMessage,
                  const rust::Box<TradingClientContext> &)>
        inbound_callback)
    : ctx(std::move(ctx)), inbound_callback(inbound_callback),
      application(TradeClientCCApi::eventHandler, this) {}
#else
TradeClientCCApi::TradeClientCCApi(const std::string &filepath, FromAppCallback cb)
    : eventCallback(cb), application(TradeClientCCApi::eventHandler, this) {}
TradeClientCCApi::TradeClientCCApi(const std::string &filepath, FromAppCallbackExecutionReport cb)
    : executionReportCallback(cb), application(TradeClientCCApi::eventHandler, this) {}    
#endif
TradeClientCCApi::~TradeClientCCApi() {
  // delete this->initiator;
  // this->initiator = nullptr;
}

bool TradeClientCCApi::eventHandler(void *obj, const ccapi::Event &event,
                                    ccapi::Session *session) {
  std::cout << "Received an event in eventHandler(TradeClientCCApi):\n" +
                   event.toStringPretty(2, 2)
            << std::endl;
  auto pObj = (TradeClientCCApi *)(obj);
  auto content = event.toStringPretty(2, 2);
  if (pObj->eventCallback != nullptr) {
    pObj->eventCallback(content, "0");
  }

  if (pObj->executionReportCallback != nullptr) {
    // pObj->executionReportCallback(content, "0");
  }
  
#ifdef USE_TRADECLIENT_RUST_INTERFACE

  QuickFixMessage quick_fix_message{
    content : std::make_unique<std::string>(std::move(content)),
    from : FixMessageType::App, // 1是借用quickfix里面的定义，代表from app
  };
  pObj->inbound_callback(std::move(quick_fix_message), pObj->ctx);

#endif

  return true;
}

auto TradeClientCCApi::start() const -> void {
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
                 const double quantity,const double price, const double stop_price,
                 const char order_type,const char time_in_force) const
    -> std::unique_ptr<std::string> {
  return this->application.new_order_single(symbol, side, quantity, price, stop_price, 
                                            order_type, time_in_force);
}

auto TradeClientCCApi::put_order(const NewOrderSingle &aNewOrderSingle) const
-> std::unique_ptr<std::string> {
    return this->application.new_order_single(aNewOrderSingle.Symbol, aNewOrderSingle.Side,
                                              aNewOrderSingle.Quantity, aNewOrderSingle.Price, aNewOrderSingle.StopPrice,
                                              aNewOrderSingle.OrdType, aNewOrderSingle.TimeInForce);
}

auto TradeClientCCApi::cancel_order(const std::string &order_id) const -> void {
  this->application.cancel_order(order_id);
}

/*auto create_client_ccapi(
    const TradingClientType type, const std::string &filepath,
    rust::Box<TradingClientContext> ctx,
    rust::Fn<void(const QuickFixMessage, const rust::Box<TradingClientContext>
&)> inbound_callback) -> std::unique_ptr<ITradeClient> { ITradeClient
*pitradeclient = nullptr; switch (type) { case TradingClientType::Apifiny:
    pitradeclient =
        new TradeClientApifiny(filepath, std::move(ctx), inbound_callback);
    break;
  case TradingClientType::Wintmute:
    break;
  }

  return std::unique_ptr<ITradeClient>(pitradeclient);
}*/
