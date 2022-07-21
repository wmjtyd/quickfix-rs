#include "Application.h"
#include "quickfix-rs/src/lib.rs.h"

#include "jwt/jwt.hpp"
#include "picojson/picojson.h"
#include "quickfix/Session.h"
#include "quickfix/config.h"

#include <iostream>
#include <openssl/sha.h>
#include <random>

// config
std::string SYMBOL = "BTCUSDT";
std::string VENUE = "BINANCE";
std::string SenderCompID = ACCOUNT_ID;
std::string TargetCompID = "APIFINY";

// const std::string __SOH__2 = "";
const std::string __SOH__ = std::string("\x01");
void replace_str(std::string &str, const std::string &before,
                 const std::string &after) {
  for (std::string::size_type pos(0); pos != std::string::npos;
       pos += after.length()) {
    pos = str.find(before, pos);
    if (pos != std::string::npos)

      str.replace(pos, before.length(), after);
    else
      break;
  }
}

std::string sha256(const std::string str) {
  unsigned char hash[SHA256_DIGEST_LENGTH];
  SHA256_CTX sha256;
  SHA256_Init(&sha256);
  SHA256_Update(&sha256, str.c_str(), str.size());
  SHA256_Final(hash, &sha256);
  std::stringstream ss;
  for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
    ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
  }
  return ss.str();
}

std::string getSignature(std::string method, std::string account_id,
                         std::string secret_key_id, std::string params,
                         std::string secret_key) {
  /*std::cout << "c++:" << std::endl;
  std::cout << "method:" << method << std::endl;
  std::cout << "account_id:" << account_id << std::endl;
  std::cout << "secret_key_id:" << secret_key_id << std::endl;
  std::cout << "params:" << params << std::endl;
  std::cout << "secret_key:" << secret_key << std::endl;*/

  auto digest = sha256(params);
  // std::cout << "digest:" << digest << std::endl;
  std::time_t now = std::time(NULL);

  std::tm *ptm = std::localtime(&now);
  std::chrono::system_clock::time_point exp =
      std::chrono::system_clock::from_time_t(1656559384);
  /*auto token = jwt::create()
      //.set_issuer("auth0")
      //.set_type("JWS")

      .set_payload_claim("accountId", jwt::claim(std::string(account_id)))
      .set_payload_claim("secretKeyId", jwt::claim(std::string(secret_key_id)))
      .set_payload_claim("digest", jwt::claim(std::string(digest)))
      .set_payload_claim("method", jwt::claim(std::string(method)))
      //.set_payload_claim("exp", jwt::claim(std::chrono::system_clock::now() +
     std::chrono::seconds{ 36000 }))

      //.set_issued_at(std::chrono::system_clock::now())
      //.set_expires_at(std::chrono::system_clock::now() + std::chrono::seconds{
     300 }) .set_expires_at(exp) .sign(jwt::algorithm::hs256{ secret_key });
      std::cout << "token:" << token << std::endl;
  */
  using namespace jwt::params;
  // auto key = "secret"; //Secret to use for the algorithm
  // Create JWT object
  jwt::jwt_object obj{algorithm("HS256"),
                      payload({{"accountId", account_id},
                               {"secretKeyId", secret_key_id},
                               {"digest", digest},
                               {"method", method}}),
                      secret(secret_key)};
  obj.add_claim("exp",
                std::chrono::system_clock::now() + std::chrono::seconds{300});

  // Get the encoded string/assertion
  auto enc_str = obj.signature();
  // std::cout << "enc_str: " << enc_str << std::endl;

  //
  // auto dec_obj = jwt::decode(enc_str, algorithms({ "HS256" }),
  // secret(secret_key)); std::cout << "dec_obj.header():" << dec_obj.header()
  // << std::endl; std::cout << "dec_obj.payload():" << dec_obj.payload() <<
  // std::endl; jwt::jwt_object obj{ algorithm("HS256"), payload({{"some",
  // "payload"}}), secret("secret")};

  auto token = enc_str;
  return token;
}

static void split(const std::string &s, std::vector<std::string> &tokens,
                  const std::string &delimiters = " ") {
  std::string::size_type lastPos = s.find_first_not_of(delimiters, 0);
  std::string::size_type pos = s.find_first_of(delimiters, lastPos);
  while (std::string::npos != pos || std::string::npos != lastPos) {
    tokens.push_back(s.substr(lastPos, pos - lastPos));
    lastPos = s.find_first_not_of(delimiters, pos);
    pos = s.find_first_of(delimiters, lastPos);
  }
}

std::string generate_order_id(std::string accountId) {
  std::vector<std::string> result;
  split(accountId, result, "-");

  auto currentTime =
      std::chrono::system_clock::now().time_since_epoch().count() / 1000000;
  std::random_device rd;
  auto r = rd();
  auto randomDigit = (r % 900) + 100;
  std::string orderId =
      result[1] + std::to_string(currentTime) + std::to_string(randomDigit);

  return orderId;
}

Application::Application(
    rust::Box<TradeClientContext> ctx,
    rust::Fn<void(const QuickFixMessage, const rust::Box<TradeClientContext> &)>
        inbound_callback)
    : ctx(std::move(ctx)), inbound_callback(inbound_callback) {}

void Application::onCreate(const FIX::SessionID &sessionID) {}

void Application::onLogon(const FIX::SessionID &sessionID) {}

void Application::onLogout(const FIX::SessionID &sessionID) {}

void Application::toAdmin(FIX::Message &message,
                          const FIX::SessionID &sessionID) {
  // std::cout << std::endl << "toAdmin - " << sessionID << std::endl;
  // std::cout << std::endl << "toAdmin: " << message << std::endl;

  if (message.getHeader().getField(FIX::FIELD::MsgType) == "A") {
    std::string method = "Fix";
    std::string account_id = ACCOUNT_ID;
    std::string secret_key_id = SECRET_KEY_ID;
    std::string params = ACCOUNT_ID;
    std::string secret_key = SECRET_KEY;
    auto signature =
        getSignature(method, account_id, secret_key_id, params, secret_key);
    message.setField(FIX::RawDataLength(signature.size()));
    message.setField(FIX::RawData(signature));
    // std::cout << "signature:" << signature << std::endl;
  }
}

void Application::toApp(FIX::Message &message, const FIX::SessionID &sessionID)
    EXCEPT(FIX::DoNotSend) {}

void Application::fromAdmin(const FIX::Message &message,
                            const FIX::SessionID &sessionID)
    EXCEPT(FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue,
           FIX::RejectLogon) {
  this->inbound(message, sessionID, FixMessageType::Admin);
}

void Application::fromApp(const FIX::Message &message,
                          const FIX::SessionID &sessionID)
    EXCEPT(FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue,
           FIX::UnsupportedMessageType) {
  this->inbound(message, sessionID, FixMessageType::App);
}

auto Application::new_order_single(const std::string &symbol, const char side,
                                   const double quantity, const double price,
                                   const char time_in_force) const
    -> std::unique_ptr<std::string> {
  const auto order_id = generate_order_id(ACCOUNT_ID);

  FIX42::NewOrderSingle order(FIX::ClOrdID(order_id), FIX::HandlInst('1'),
                              FIX::Symbol(symbol), FIX::Side(side),
                              FIX::TransactTime(FIX::UTCTIMESTAMP()),
                              FIX::OrdType(FIX::OrdType_LIMIT));
  order.set(FIX::OrderQty(quantity));
  order.set(FIX::TimeInForce(time_in_force));
  order.set(FIX::Price(price));
  order.set(FIX::Account(ACCOUNT_ID));
  order.set(FIX::Text("New Order"));

  auto &header = order.getHeader();
  header.setField(FIX::SenderCompID(ACCOUNT_ID));
  header.setField(FIX::TargetCompID(TargetCompID));
  header.setField(FIX::SecurityExchange(VENUE));

  FIX::Session::sendToTarget(order);

  return std::make_unique<std::string>(std::move(order_id));
}

auto Application::cancel_order(const std::string &order_id,
                               const FIX::SessionID &session_id) const -> void {
  FIX42::OrderCancelRequest orderCancelRequest;
  orderCancelRequest.set(FIX::ClOrdID(order_id));
  orderCancelRequest.set(FIX::Account(ACCOUNT_ID));
  orderCancelRequest.set(FIX::Text("Cancel Order"));
  orderCancelRequest.set(FIX::SecurityExchange(VENUE));

  FIX::Session::sendToTarget(orderCancelRequest, session_id);
}

auto Application::inbound(const FIX::Message &message,
                          const FIX::SessionID &session_id,
                          const FixMessageType from) -> void {
  const auto content = message.toString();
  QuickFixMessage quick_fix_message{
    content : std::make_unique<std::string>(std::move(content)),
    session_id : std::make_unique<FIX::SessionID>(std::move(session_id)),
    from : from,
  };

  this->inbound_callback(std::move(quick_fix_message), this->ctx);
}
