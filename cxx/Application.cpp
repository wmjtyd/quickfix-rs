#include "include/Application.h"

#include "quickfix/Session.h"
#include "quickfix/config.h"

#include "jwt/jwt.hpp"
#include <picojson/picojson.h>

#include <iostream>
#include <memory>
#include <openssl/sha.h>
#include <random>

#include "quickfix-rs/src/lib.rs.h"

// config
std::string SYMBOL = "BTCUSDT";
std::string VENUE = "FTX"; // choose exchange you want to trade
std::string SERVER =
    "fixapiftx.apifiny.com:1443"; //#use the right endpoint for each exchange
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

Application::Application(
    rust::Box<ApplicationContext> app_ctx,
    rust::Fn<void(const QuickFixMessage, const FIX::SessionID &,
                  const rust::Box<ApplicationContext> &)>
        inbound_callback)
    : app_ctx(std::move(app_ctx)), inbound_callback(inbound_callback) {}

void Application::onCreate(const FIX::SessionID &sessionID) {
  std::cout << std::endl << "onCreate - " << sessionID << std::endl;
}

void Application::onLogon(const FIX::SessionID &sessionID) {
  std::cout << std::endl << "Logon - " << sessionID << std::endl;
}

void Application::onLogout(const FIX::SessionID &sessionID) {
  std::cout << std::endl << "Logout - " << sessionID << std::endl;
}

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

  std::string m = message.toString();
  replace_str(m, __SOH__, "|");
  // std::cout << std::endl
  //<< "toAdmin: " << m << std::endl;
}

void Application::fromAdmin(const FIX::Message &message,
                            const FIX::SessionID &sessionID)
    EXCEPT(FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue,
           FIX::RejectLogon) {
  this->inbound(message, FixMessageType::Admin, sessionID);
}

void Application::fromApp(const FIX::Message &message,
                          const FIX::SessionID &sessionID)
    EXCEPT(FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue,
           FIX::UnsupportedMessageType) {
  this->inbound(message, FixMessageType::App, sessionID);
}

void Application::toApp(FIX::Message &message, const FIX::SessionID &sessionID)
    EXCEPT(FIX::DoNotSend) {
  std::cout << std::endl << "toApp: " << sessionID << std::endl;
  try {
    FIX::PossDupFlag possDupFlag;
    message.getHeader().getField(possDupFlag);
    if (possDupFlag)
      throw FIX::DoNotSend();
  } catch (FIX::FieldNotFound &) {
  }

  std::string m = message.toString();
  // replace_str(&m, __SOH__, "|");
  replace_str(m, __SOH__, "|");
  std::cout << std::endl << "OUT: " << m << std::endl;
}

auto Application::new_order_single(const std::string &order_id,
                                   const std::string &symbol, const int side,
                                   const int quantity, const int price,
                                   const int time_in_force) const -> void {
  FIX42::NewOrderSingle order(FIX::ClOrdID(order_id), FIX::HandlInst('1'),
                              FIX::Symbol(symbol), FIX::Side(side),
                              FIX::TransactTime(FIX::UTCTIMESTAMP()),
                              FIX::OrdType(FIX::OrdType_LIMIT));

  order.set(FIX::OrderQty(quantity));
  order.set(FIX::TimeInForce(time_in_force));
  order.set(FIX::Price(price));
  order.set(FIX::Account(ACCOUNT_ID));
  order.set(FIX::Text("new Order"));

  auto &header = order.getHeader();
  header.setField(FIX::SenderCompID(ACCOUNT_ID));
  header.setField(FIX::TargetCompID(TargetCompID));
  header.setField(FIX::SecurityExchange(VENUE));

  FIX::Session::sendToTarget(order);
}

auto Application::inbound(const FIX::Message &message,
                          const FixMessageType from,
                          const FIX::SessionID &session_id) -> void {
  const auto content = message.toString();
  auto *content_ptr = new std::string(std::move(content));
  QuickFixMessage quick_fix_message{
    content : std::unique_ptr<std::string>(content_ptr),
    from : from,
  };

  this->inbound_callback(std::move(quick_fix_message), session_id,
                         this->app_ctx);
}

void Application::CancelOrder(FIX::ClOrdID &aClOrdID) {
  auto nowUtc =
      std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  std::cout << "order utc:" << nowUtc << std::endl;

  //    11	ClOrdID	Only present on order acknowledgements, ExecType=New
  //    (150=0) 1	    Account	account id 207	SecurityExchange
  //    VENUE 58	Text	Free format text string

  FIX42::OrderCancelRequest orderCancelRequest;
  orderCancelRequest.set(aClOrdID);
  orderCancelRequest.set(FIX::Account(ACCOUNT_ID));
  orderCancelRequest.set(FIX::SecurityExchange(VENUE));
  FIX::Session::sendToTarget(orderCancelRequest);
}
