#pragma once

// #include "quickfix/Application.h"
// #include "quickfix/Values.h"
// #include "quickfix/fix42/NewOrderSingle.h"
// #include "quickfix/fix42/OrderCancelRequest.h"
// #include "rust/cxx.h"

#include "ccapi_wrapper.h"

enum class FixMessageType : uint8_t;

struct QuickFixMessage;
struct TradingClientContext;

class ApplicationCCApi {
public:
  ApplicationCCApi(eventHandlerFunc *eventHandler, void *myEventHandlerObj);
  ~ApplicationCCApi();

  auto new_order_single(const std::string &symbol, char side, 
                        const double quantity,const double price, const double stop_price,
                        const char order_type,const char time_in_force) const
      -> std::unique_ptr<std::string>;
  auto cancel_order(const std::string &order_id) const -> void;

  auto start() const -> void;
  auto stop() const -> void;

  static bool eventHandler(void *obj, const ccapi::Event &event,
                           ccapi::Session *session);
private:
  auto convertSide(const char side) const -> std::string;

  auto convertOrderType(const char side) const -> std::string;

  auto convertTimeInForce(const char side) const -> std::string;                          

private:
  // auto inbound(const FIX::Message &message, const FIX::SessionID &session_id,
  //              const FixMessageType from) -> void;

  // rust::Box<TradingClientContext> ctx;
  // rust::Fn<void(const QuickFixMessage, const rust::Box<TradingClientContext>
  // &)>
  //     inbound_callback;

  CCApiWrapper ccapiwrapper;

  eventHandlerFunc *eventHandle;
  void *myEventHandlerObj;
};
