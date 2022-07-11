#include "include/Application.h"

#include "quickfix-rs/src/lib.rs.h"

Application::Application(): inner(new_application_impl()) {
  const auto session_id = FIX::SessionID();
  this->onCreate(session_id);
}

Application::~Application() {}

auto Application::onCreate(const FIX::SessionID& session_id) -> void {
  this->inner->on_create(session_id);
}

auto Application::onLogon(const FIX::SessionID& session_id) -> void {}

auto Application::onLogout(const FIX::SessionID& session_id) -> void {}

auto Application::toAdmin(FIX::Message& message, const FIX::SessionID& session_id) -> void {}

auto Application::fromAdmin(const FIX::Message& message, const FIX::SessionID& session_id) -> void {}

auto Application::toApp(FIX::Message& message, const FIX::SessionID& session_id) -> void {}

auto Application::fromApp(const FIX::Message& message, const FIX::SessionID& session_id) -> void {}

auto new_application() -> std::unique_ptr<Application> {
  return std::unique_ptr<Application>(new Application());
}
