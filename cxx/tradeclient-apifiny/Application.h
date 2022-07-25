#pragma once

#include "quickfix/Application.h"
#include "quickfix/Values.h"
#include "quickfix/fix42/NewOrderSingle.h"
#include "quickfix/fix42/OrderCancelRequest.h"
#include "rust/cxx.h"

enum class FixMessageType : uint8_t;

struct QuickFixMessage;
struct TradingClientContext;

class Application : public FIX::Application {
public:
  Application(rust::Box<TradingClientContext> ctx,
              rust::Fn<void(const QuickFixMessage,
                            const rust::Box<TradingClientContext> &)>);
  auto new_order_single(const std::string &symbol, const char side,
                        const double quantity, const double price,
                        const char time_in_force) const
      -> std::unique_ptr<std::string>;
  auto cancel_order(const std::string &order_id) const -> void;

private:
  void onCreate(const FIX::SessionID &sesison_id);
  void onLogon(const FIX::SessionID &session_id);
  void onLogout(const FIX::SessionID &session_id);
  void toAdmin(FIX::Message &message, const FIX::SessionID &session_id);
  void toApp(FIX::Message &message, const FIX::SessionID &session_id)
      EXCEPT(FIX::DoNotSend);
  void fromAdmin(const FIX::Message &message, const FIX::SessionID &session_id)
      EXCEPT(FIX::FieldNotFound, FIX::IncorrectDataFormat,
             FIX::IncorrectTagValue, FIX::RejectLogon);
  void fromApp(const FIX::Message &message, const FIX::SessionID &session_id)
      EXCEPT(FIX::FieldNotFound, FIX::IncorrectDataFormat,
             FIX::IncorrectTagValue, FIX::UnsupportedMessageType);

private:
  auto inbound(const FIX::Message &message, const FIX::SessionID &session_id,
               const FixMessageType from) -> void;

  rust::Box<TradingClientContext> ctx;
  rust::Fn<void(const QuickFixMessage, const rust::Box<TradingClientContext> &)>
      inbound_callback;
};
