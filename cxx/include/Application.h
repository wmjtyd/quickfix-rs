#pragma once

#include "../../vendor/quickfix-cpp/include/quickfix/Application.h"
#include "../../vendor/quickfix-cpp/include/quickfix/MessageCracker.h"
#include "../../vendor/quickfix-cpp/include/quickfix/Values.h"

#include "rust/cxx.h"

#include <memory>

struct ApplicationImpl;

class Application: FIX::Application, FIX::MessageCracker {
 public:
  Application();
  ~Application() override;

  auto onCreate(const FIX::SessionID&) -> void override;
  auto onLogon(const FIX::SessionID&) -> void override;
  auto onLogout(const FIX::SessionID&) -> void override;
  auto toAdmin(FIX::Message&, const FIX::SessionID&) -> void override;
  auto fromAdmin(const FIX::Message&, const FIX::SessionID&) EXCEPT(FieldNotFound, IncorrectDataFormat, IncorrectTagValue, RejectLogon) -> void override;
  auto toApp(FIX::Message&, const FIX::SessionID&) EXCEPT(DoNotSend) -> void override;
  auto fromApp(const FIX::Message&, const FIX::SessionID&) EXCEPT(FieldNotFound, IncorrectDataFormat, IncorrectTagValue, UnsupportedMessageType) -> void override;

 private:
  rust::Box<ApplicationImpl> inner;
};

auto new_application() -> std::unique_ptr<Application>;
