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

static void split_2(const std::string &s, std::vector<std::string> &tokens,
                  const std::string &delimiters = " ") {
  std::string::size_type lastPos = s.find_first_not_of(delimiters, 0);
  std::string::size_type pos = s.find_first_of(delimiters, lastPos);
  while (std::string::npos != pos || std::string::npos != lastPos) {
    tokens.push_back(s.substr(lastPos, pos - lastPos));
    lastPos = s.find_first_not_of(delimiters, pos);
    pos = s.find_first_of(delimiters, lastPos);
  }
}

std::string generate_order_id_2(std::string accountId) {
  std::vector<std::string> result;
  split_2(accountId, result, "-");

  auto currentTime =
      std::chrono::system_clock::now().time_since_epoch().count() / 1000000;
  std::random_device rd;
  auto r = rd();
  auto randomDigit = (r % 900) + 100;
  std::string orderId =
      result[1] + std::to_string(currentTime) + std::to_string(randomDigit);

  return orderId;
}

ApplicationCCApi::ApplicationCCApi(
    rust::Box<TradingClientContext> ctx,
    rust::Fn<void(const QuickFixMessage, const rust::Box<TradingClientContext> &)>
        inbound_callback)
    : ctx(std::move(ctx)), inbound_callback(inbound_callback) {}



auto ApplicationCCApi::new_order_single(const std::string &symbol, const char side,
                                   const double quantity, const double price,
                                   const char time_in_force) const
    -> std::unique_ptr<std::string> {
  const auto order_id = generate_order_id_2(ACCOUNT_ID_CCAPI);

  
  return std::make_unique<std::string>(std::move(order_id));
}

auto ApplicationCCApi::cancel_order(const std::string &order_id) const -> void {
  
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
