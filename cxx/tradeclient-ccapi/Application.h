#pragma once

#include "quickfix/Application.h"
#include "quickfix/Values.h"
#include "quickfix/fix42/NewOrderSingle.h"
#include "quickfix/fix42/OrderCancelRequest.h"
#include "rust/cxx.h"

#include "ccapi_wrapper.h"

enum class FixMessageType : uint8_t;

struct QuickFixMessage;
struct TradingClientContext;

class ApplicationCCApi {
public:
  ApplicationCCApi(rust::Box<TradingClientContext> ctx,
              rust::Fn<void(const QuickFixMessage,
                            const rust::Box<TradingClientContext> &)>);
  auto new_order_single(const std::string &symbol, const char side,
                        const double quantity, const double price,
                        const char time_in_force) const
      -> std::unique_ptr<std::string>;
  auto cancel_order(const std::string &order_id) const -> void;

  auto start() -> void;
  auto stop() const -> void;

private:
  // auto inbound(const FIX::Message &message, const FIX::SessionID &session_id,
  //              const FixMessageType from) -> void;

  rust::Box<TradingClientContext> ctx;
  rust::Fn<void(const QuickFixMessage, const rust::Box<TradingClientContext> &)>
      inbound_callback;

  CCApiWrapper ccapiwrapper;  
};
