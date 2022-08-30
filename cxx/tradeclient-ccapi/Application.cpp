#include "Application.h"
#include "quickfix-rs/src/lib.rs.h"



#include <iostream>
#include <chrono>
#include <openssl/sha.h>
#include <random>

#include <ccapi_cpp/ccapi_session.h>
#include "ccapi_cpp/ccapi_event.h"
#include "ccapi_cpp/ccapi_queue.h"



static std::string ACCOUNT_ID_CCAPI = "";
static std::string SECRET_KEY_ID_CCAPI = "";
static std::string SECRET_KEY_CCAPI = "";

bool ApplicationCCApi::eventHandler(void *obj, const ccapi::Event& event, ccapi::Session* session) {
  std::cout << "Received an event in eventHandler(Application):\n" + event.toStringPretty(2, 2) << std::endl;
  auto pApplicationCCApi = (ApplicationCCApi*)(obj);
  auto content = event.toStringPretty(2, 2);
  QuickFixMessage quick_fix_message{
    content : std::make_unique<std::string>(std::move(content)),
    from : FixMessageType::App, // 1是借用quickfix里面的定义，代表from app
  };
  pApplicationCCApi->inbound_callback(std::move(quick_fix_message), pApplicationCCApi->ctx);
  return true;
}


ApplicationCCApi::ApplicationCCApi(
    rust::Box<TradingClientContext> ctx,
    rust::Fn<void(const QuickFixMessage, const rust::Box<TradingClientContext> &)>
        inbound_callback)
    : ctx(std::move(ctx)), inbound_callback(inbound_callback),
    ccapiwrapper(CCApiWrapper("binance", eventHandler, this)) {
      // this->ccapiwrapper = new CCApiWrapper("binance-us", eventHandler);
      this->ccapiwrapper.Start(); // symbol = "BTCUSDT"
    }

ApplicationCCApi::~ApplicationCCApi() {
      // this->ccapiwrapper = new CCApiWrapper("binance-us", eventHandler);
      this->ccapiwrapper.Stop(); // symbol = "BTCUSDT" 
    }

auto ApplicationCCApi::new_order_single(const std::string &symbol, const char side,
                                   const double quantity, const double price,
                                   const char time_in_force) const
    -> std::unique_ptr<std::string> {
  // const auto order_id = generate_order_id_2(ACCOUNT_ID_CCAPI);

  this->ccapiwrapper.Request(CCAPI_EXECUTION_CREATE_ORDER, symbol); // symbol = "BTCUSDT"
  // return std::make_unique<std::string>(std::move(order_id));
  std::string order_id = "0"; //TODO: 暂时这样先编译通过
  return std::make_unique<std::string>(std::move(order_id));
}

auto ApplicationCCApi::cancel_order(const std::string &order_id) const -> void {
  this->ccapiwrapper.Request(CCAPI_EXECUTION_CANCEL_ORDER, order_id); // symbol = "BTCUSDT"
}

auto ApplicationCCApi::start() const ->  void {
  // this->ccapiwrapper.Start(); // symbol = "BTCUSDT"
}


auto ApplicationCCApi::stop() const -> void {
  // this->ccapiwrapper.Stop(); // symbol = "BTCUSDT"
}

/*auto ApplicationCCApi::inbound(const FIX::Message &message,
                          const FIX::SessionID &session_id,
                          const FixMessageType from) -> void {
  const auto content = message.toString();
  QuickFixMessage quick_fix_message{
    content : std::make_unique<std::string>(std::move(content)),
    from : from,
  };

  this->inbound_callback(std::move(quick_fix_message), this->ctx);
}*/
