#pragma once

#include "quickfix/Application.h"
#include "quickfix/Mutex.h"
#include "quickfix/Values.h"
#include "rust/cxx.h"

#include "quickfix/fix42/NewOrderSingle.h"
#include "quickfix/fix42/OrderCancelReplaceRequest.h"
#include "quickfix/fix42/OrderCancelRequest.h"

#include <queue>

enum class FixMessageType : uint8_t;

struct QuickFixMessage;
struct ApplicationContext;

class Application : public FIX::Application {
public:
  Application(rust::Box<ApplicationContext> app_ctx,
              rust::Fn<void(const QuickFixMessage, const FIX::SessionID &,
                            const rust::Box<ApplicationContext> &)>);

  auto new_order_single(const std::string &order_id, const std::string &symbol,
                        const int side, const int quantity, const int price,
                        const int time_in_force) const -> void;

private:
  void onCreate(const FIX::SessionID &);
  void onLogon(const FIX::SessionID &);
  void onLogout(const FIX::SessionID &);
  void toAdmin(FIX::Message &, const FIX::SessionID &);
  void toApp(FIX::Message &, const FIX::SessionID &) EXCEPT(FIX::DoNotSend);

  void fromAdmin(const FIX::Message &, const FIX::SessionID &)
      EXCEPT(FIX::FieldNotFound, FIX::IncorrectDataFormat,
             FIX::IncorrectTagValue, FIX::RejectLogon);

  void fromApp(const FIX::Message &, const FIX::SessionID &)
      EXCEPT(FIX::FieldNotFound, FIX::IncorrectDataFormat,
             FIX::IncorrectTagValue, FIX::UnsupportedMessageType);

  void CancelOrder(FIX::ClOrdID &aClOrdID);

  auto inbound(const FIX::Message &, const FixMessageType,
               const FIX::SessionID &) -> void;

  rust::Box<ApplicationContext> app_ctx;
  rust::Fn<void(const QuickFixMessage, const FIX::SessionID &,
                const rust::Box<ApplicationContext> &)>
      inbound_callback;
};
