#pragma once

#include "quickfix/Application.h"
#include "quickfix/MessageCracker.h"
#include "quickfix/Mutex.h"
#include "quickfix/Values.h"
#include "quickfix/fix42/NewOrderSingle.h"
#include "quickfix/fix42/OrderCancelRequest.h"
#include "rust/cxx.h"

enum class FixMessageType : uint8_t;

struct QuickFixMessage;
struct TradeClientContext;

class Application : public FIX::Application, public FIX::MessageCracker {
public:
  Application(rust::Box<TradeClientContext> ctx,
              rust::Fn<void(const QuickFixMessage, const FIX::SessionID &,
                            const rust::Box<TradeClientContext> &)>);
  auto new_order_single(const std::string &symbol, const char side,
                        const uint32_t quantity, const uint32_t price,
                        const char time_in_force) const
      -> std::unique_ptr<std::string>;

private:
  void onCreate(const FIX::SessionID &);
  void onLogon(const FIX::SessionID &sessionID);
  void onLogout(const FIX::SessionID &sessionID);
  void toAdmin(FIX::Message &, const FIX::SessionID &);
  void toApp(FIX::Message &, const FIX::SessionID &) EXCEPT(FIX::DoNotSend);
  void fromAdmin(const FIX::Message &, const FIX::SessionID &)
      EXCEPT(FIX::FieldNotFound, FIX::IncorrectDataFormat,
             FIX::IncorrectTagValue, FIX::RejectLogon);
  void fromApp(const FIX::Message &message, const FIX::SessionID &sessionID)
      EXCEPT(FIX::FieldNotFound, FIX::IncorrectDataFormat,
             FIX::IncorrectTagValue, FIX::UnsupportedMessageType);

  void CancelOrder(FIX::ClOrdID &aClOrdID);

private:
  auto inbound(const FIX::Message &, const FixMessageType,
               const FIX::SessionID &) -> void;

  rust::Box<TradeClientContext> ctx;
  rust::Fn<void(const QuickFixMessage, const FIX::SessionID &,
                const rust::Box<TradeClientContext> &)>
      inbound_callback;
};
