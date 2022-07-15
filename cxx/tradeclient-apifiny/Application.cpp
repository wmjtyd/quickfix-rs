/* -*- C++ -*- */

/****************************************************************************
** Copyright (c) 2001-2014
**
** This file is part of the QuickFIX FIX Engine
**
** This file may be distributed under the terms of the quickfixengine.org
** license as defined by quickfixengine.org and appearing in the file
** LICENSE included in the packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.quickfixengine.org/LICENSE for licensing information.
**
** Contact ask@quickfixengine.org if any conditions of this licensing are
** not clear to you.
**
****************************************************************************/

#ifdef _MSC_VER
#pragma warning( disable : 4503 4355 4786 )
#endif

#undef max
#undef min
#define NOMINMAX 1
#include "quickfix/config.h"
#include "Application.h"
#include "quickfix/Session.h"
//#include <jwt-cpp/jwt.h>
#include "jwt/jwt.hpp"
#include <picojson/picojson.h>
#include <iostream>
#include <openssl/sha.h>
#include <random>

//config
std::string ACCOUNT_ID = "";
std::string SECRET_KEY_ID = "";
std::string SECRET_KEY = "";
std::string SYMBOL = "BTCUSDT";
std::string VENUE = "FTX";  // choose exchange you want to trade
std::string SERVER = "fixapiftx.apifiny.com:1443";  //#use the right endpoint for each exchange
std::string SenderCompID = ACCOUNT_ID;
std::string TargetCompID = "APIFINY";

//const std::string __SOH__2 = "";
const std::string __SOH__ = std::string("\x01");
void replace_str(std::string& str, const std::string& before, const std::string& after)
{
    for (std::string::size_type pos(0); pos != std::string::npos; pos += after.length())
    {
        pos = str.find(before, pos);
        if (pos != std::string::npos)

            str.replace(pos, before.length(), after);
        else
            break;
    }
}

std::string sha256(const std::string str)
{
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, str.c_str(), str.size());
    SHA256_Final(hash, &sha256);
    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    return ss.str();
}

std::string getSignature(std::string method, 
                         std::string account_id, 
                         std::string secret_key_id,
                         std::string params, 
                         std::string secret_key) {
    /*std::cout << "c++:" << std::endl;
    std::cout << "method:" << method << std::endl;
    std::cout << "account_id:" << account_id << std::endl;
    std::cout << "secret_key_id:" << secret_key_id << std::endl;
    std::cout << "params:" << params << std::endl;
    std::cout << "secret_key:" << secret_key << std::endl;*/
    
    auto digest = sha256(params);
    //std::cout << "digest:" << digest << std::endl;
    std::time_t now = std::time(NULL);
    
    std::tm* ptm = std::localtime(&now);
    std::chrono::system_clock::time_point exp = std::chrono::system_clock::from_time_t(1656559384);
    /*auto token = jwt::create()
        //.set_issuer("auth0")
        //.set_type("JWS")

        .set_payload_claim("accountId", jwt::claim(std::string(account_id)))
        .set_payload_claim("secretKeyId", jwt::claim(std::string(secret_key_id)))
        .set_payload_claim("digest", jwt::claim(std::string(digest)))
        .set_payload_claim("method", jwt::claim(std::string(method)))
        //.set_payload_claim("exp", jwt::claim(std::chrono::system_clock::now() + std::chrono::seconds{ 36000 }))

        //.set_issued_at(std::chrono::system_clock::now())
        //.set_expires_at(std::chrono::system_clock::now() + std::chrono::seconds{ 300 })
        .set_expires_at(exp)
        .sign(jwt::algorithm::hs256{ secret_key });
        std::cout << "token:" << token << std::endl;
    */
    using namespace jwt::params;
    //auto key = "secret"; //Secret to use for the algorithm
    //Create JWT object
    jwt::jwt_object obj{ 
        algorithm("HS256"), 
        payload({
            {"accountId", account_id},
            {"secretKeyId", secret_key_id},
            {"digest", digest},
            {"method", method}         
        }),
        secret(secret_key) };
    obj.add_claim("exp", std::chrono::system_clock::now() + std::chrono::seconds{ 300 });

    //Get the encoded string/assertion
    auto enc_str = obj.signature();
    //std::cout << "enc_str: " << enc_str << std::endl;

    //
    //auto dec_obj = jwt::decode(enc_str, algorithms({ "HS256" }), secret(secret_key));
    //std::cout << "dec_obj.header():" << dec_obj.header() << std::endl;
    //std::cout << "dec_obj.payload():" << dec_obj.payload() << std::endl;
    //jwt::jwt_object obj{ algorithm("HS256"), payload({{"some", "payload"}}), secret("secret")};
    
    auto token = enc_str;
    return token;
}

static void split(const std::string& s, std::vector<std::string>& tokens, const std::string& delimiters = " ")
{
    std::string::size_type lastPos = s.find_first_not_of(delimiters, 0);
    std::string::size_type pos = s.find_first_of(delimiters, lastPos);
    while (std::string::npos != pos || std::string::npos != lastPos) {
        tokens.push_back(s.substr(lastPos, pos - lastPos));
        lastPos = s.find_first_not_of(delimiters, pos);
        pos = s.find_first_of(delimiters, lastPos);
    }
}

std::string generate_order_id(std::string accountId)
{
    std::vector<std::string> result;
    split(accountId, result, "-");

    //orderId if requested, should be composed of: account number + random number + 13 digit timestamp+3 digit random number. 
    //Up to 64 digits.
    
    //std::cout << "split:" << result[1] << std::endl
    std::cout << "now:" << std::chrono::system_clock::now().time_since_epoch().count()/1000 << std::endl;
    //std::cout << "now:" << std::chrono::system_clock::now().time_since_epoch().count() / std::chrono::micro << std::endl;
    auto currentTime = std::chrono::system_clock::now().time_since_epoch().count() / 1000;
    std::random_device rd;
    auto r = rd();
    auto randomDigit = (r % 900) + 100;
    std::cout << "countid:" << result[1] << std::endl;
    std::cout << "currentTime:" << currentTime << std::endl;
    std::cout << "randomDigit:" << randomDigit << std::endl;
    std::string orderId = result[1] + std::to_string(currentTime) + std::to_string(randomDigit);
    std::cout << "orderId:" << orderId << std::endl;
    return orderId;
    
}

void Application::onCreate( const FIX::SessionID& sessionID)
{
    std::cout << std::endl << "onCreate - " << sessionID << std::endl;
}

void Application::onLogon( const FIX::SessionID& sessionID )
{
  std::cout << std::endl << "Logon - " << sessionID << std::endl;
}

void Application::onLogout( const FIX::SessionID& sessionID )
{
  std::cout << std::endl << "Logout - " << sessionID << std::endl;
}

void Application::toAdmin( FIX::Message& message, const FIX::SessionID& sessionID)
{
	//std::cout << std::endl << "toAdmin - " << sessionID << std::endl;
	//std::cout << std::endl << "toAdmin: " << message << std::endl;
    
    if (message.getHeader().getField(FIX::FIELD::MsgType) == "A")
    {
        std::string method = "Fix";
        std::string account_id = ACCOUNT_ID;
        std::string secret_key_id = SECRET_KEY_ID;
        std::string params = ACCOUNT_ID;
        std::string secret_key = SECRET_KEY;
        auto signature = getSignature(method,
            account_id,
            secret_key_id,
            params,
            secret_key);
        message.setField(FIX::RawDataLength(signature.size()));
        message.setField(FIX::RawData(signature));
        //std::cout << "signature:" << signature << std::endl;
    }

    std::string m = message.toString();
    replace_str(m, __SOH__, "|");
    //std::cout << std::endl
    //<< "toAdmin: " << m << std::endl;
}

void Application::fromAdmin( const FIX::Message& message, const FIX::SessionID& sessionID)
EXCEPT( FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::RejectLogon ) 
{
	std::cout << std::endl << "fromAdmin - " << sessionID << std::endl;
	//std::cout << std::endl << "fromAdmin: " << message << std::endl;
    std::string m = message.toString();
    replace_str(m, __SOH__, "|");
    std::cout << std::endl << "fromAdmin: " << m << std::endl;
}

void Application::fromApp( const FIX::Message& message, const FIX::SessionID& sessionID )
EXCEPT( FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::UnsupportedMessageType )
{
  mFromAppCallback(message.toString(), sessionID.toString());
  crack( message, sessionID );
  //std::cout << std::endl << "IN: " << message << std::endl;
  std::string m = message.toString();
  replace_str(m, __SOH__, "|");
  std::cout << std::endl << "IN: " << m << std::endl;


}

void Application::toApp( FIX::Message& message, const FIX::SessionID& sessionID )
EXCEPT( FIX::DoNotSend )
{
    std::cout << std::endl << "toApp: " << sessionID << std::endl;
  try
  {
    FIX::PossDupFlag possDupFlag;
    message.getHeader().getField( possDupFlag );
    if ( possDupFlag ) throw FIX::DoNotSend();
  }
  catch ( FIX::FieldNotFound& ) {}

  std::string m = message.toString();
  //replace_str(&m, __SOH__, "|");
  replace_str(m, __SOH__, "|");
  std::cout << std::endl
  << "OUT: " << m << std::endl;
}

void Application::onMessage
( const FIX42::ExecutionReport& executionReport, const FIX::SessionID& sessionId) 
{
    // auto nowUtc = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    auto millisec_since_epoch = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    std::cout << std::endl
        << "ExecutionReport utc:" << millisec_since_epoch << std::endl
        << "onMessage: FIX::SessionID:" << sessionId << std::endl
        << "onMessage: FIX42::ExecutionReport:" << executionReport << std::endl;
}

void Application::onMessage
( const FIX42::OrderCancelReject&, const FIX::SessionID& ) {}

void Application::run()
{
  while ( true )
  {
    try
    {
        char action = queryAction();
        if ( action == '1' )
            NewOrderSingle();
//        else if ( action == '2' )
//            queryCancelOrder();
//        else if ( action == '3' )
//            queryReplaceOrder();
//        else if ( action == '4' )
//            queryMarketDataRequest();
//        else if ( action == '5' )
//            break;
    }
    catch ( std::exception & e )
    {
      std::cout << "Message Not Sent: " << e.what();
    }
  }
}

void Application::NewOrderSingle()
{
  // auto nowUtc = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  auto millisec_since_epoch = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
  std::cout << "order utc:" << millisec_since_epoch << std::endl;
  //std::chrono::system_clock::now();
  //FIX::DateTime();
  auto orderId = generate_order_id(ACCOUNT_ID);
  FIX::OrdType ordType;
  std::cout << "orderId:" << orderId << std::endl;
  FIX42::NewOrderSingle newOrderSingle(
          FIX::ClOrdID(orderId), 
          FIX::HandlInst( '1' ),
          FIX::Symbol( "BTCUSDT" ), 
          FIX::Side( FIX::Side_BUY ),
          FIX::TransactTime(FIX::UTCTIMESTAMP()),
          //FIX::TransactTime(),
          ordType = FIX::OrdType( FIX::OrdType_LIMIT ) );

  newOrderSingle.set( FIX::OrderQty( 1 ) );
  newOrderSingle.set( FIX::TimeInForce(FIX::TimeInForce_IMMEDIATE_OR_CANCEL) );

  ordType = FIX::OrdType_LIMIT;
  if (ordType == FIX::OrdType_LIMIT || ordType == FIX::OrdType_STOP_LIMIT)
  {
      // newOrderSingle.set(queryPrice());
      newOrderSingle.set(FIX::Price(1));
  }
    
  if (ordType == FIX::OrdType_STOP || ordType == FIX::OrdType_STOP_LIMIT)
  {
      newOrderSingle.set(queryStopPx());
  }

  newOrderSingle.set(FIX::Account(ACCOUNT_ID));
  newOrderSingle.set(FIX::Text("new Order"));

  //queryHeader( newOrderSingle.getHeader() );
  FIX::Header& header = newOrderSingle.getHeader();
  //std::string('APIFINY')
  header.setField( FIX::SenderCompID( ACCOUNT_ID ) );
  header.setField( FIX::TargetCompID(TargetCompID) );
  header.setField(FIX::SecurityExchange(VENUE));
  
  //header.setField( FIX::TargetSubID( value ) );
  //return newOrderSingle;
  //if ( queryConfirm( "Send order" ) )
  FIX::Session::sendToTarget( newOrderSingle );
}

void Application::CancelOrder(FIX::ClOrdID& aClOrdID)
{
    auto nowUtc = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::cout << "order utc:" << nowUtc << std::endl;

    //    11	ClOrdID	Only present on order acknowledgements, ExecType=New (150=0)
    //    1	    Account	account id
    //    207	SecurityExchange	VENUE
    //    58	Text	Free format text string

    FIX42::OrderCancelRequest orderCancelRequest;
    orderCancelRequest.set(aClOrdID);
    orderCancelRequest.set(FIX::Account(ACCOUNT_ID));
    orderCancelRequest.set(FIX::SecurityExchange(VENUE));
    FIX::Session::sendToTarget(orderCancelRequest );
}

void Application::queryHeader( FIX::Header& header )
{
  header.setField( querySenderCompID() );
  header.setField( queryTargetCompID() );

  if ( queryConfirm( "Use a TargetSubID" ) )
    header.setField( queryTargetSubID() );
}

char Application::queryAction()
{
  char value;
  std::cout << std::endl
  << "1) Enter Order" << std::endl
  << "2) Cancel Order" << std::endl
  << "3) Replace Order" << std::endl
  << "4) Market data test" << std::endl
  << "5) test" << std::endl
  << "6) Quit" << std::endl
  << "Action: ";
  std::cin >> value;
  switch ( value )
  {
    case '1': case '2': case '3': case '4': case '5': case '6': break;
    default: throw std::exception();
  }
  return value;
}

int Application::queryVersion()
{
  char value;
  std::cout << std::endl
  << "1) FIX.4.0" << std::endl
  << "2) FIX.4.1" << std::endl
  << "3) FIX.4.2" << std::endl
  << "4) FIX.4.3" << std::endl
  << "5) FIX.4.4" << std::endl
  << "6) FIXT.1.1 (FIX.5.0)" << std::endl
  << "BeginString: ";
  std::cin >> value;
  switch ( value )
  {
    case '1': return 40;
    case '2': return 41;
    case '3': return 42;
    case '4': return 43;
    case '5': return 44;
    case '6': return 50;
    default: throw std::exception();
  }
}

bool Application::queryConfirm( const std::string& query )
{
  std::string value;
  std::cout << std::endl << query << "?: ";
  std::cin >> value;
  return toupper( *value.c_str() ) == 'Y';
}

FIX::SenderCompID Application::querySenderCompID()
{
  std::string value;
  std::cout << std::endl << "SenderCompID: ";
  std::cin >> value;
  return FIX::SenderCompID( value );
}

FIX::TargetCompID Application::queryTargetCompID()
{
  std::string value;
  std::cout << std::endl << "TargetCompID: ";
  std::cin >> value;
  return FIX::TargetCompID( value );
}

FIX::TargetSubID Application::queryTargetSubID()
{
  std::string value;
  std::cout << std::endl << "TargetSubID: ";
  std::cin >> value;
  return FIX::TargetSubID( value );
}

FIX::ClOrdID Application::queryClOrdID()
{
  std::string value;
  std::cout << std::endl << "ClOrdID: ";
  std::cin >> value;
  return FIX::ClOrdID( value );
}

FIX::OrigClOrdID Application::queryOrigClOrdID()
{
  std::string value;
  std::cout << std::endl << "OrigClOrdID: ";
  std::cin >> value;
  return FIX::OrigClOrdID( value );
}

FIX::Symbol Application::querySymbol()
{
  std::string value;
  std::cout << std::endl << "Symbol: ";
  std::cin >> value;
  return FIX::Symbol( value );
}

FIX::Side Application::querySide()
{
  char value;
  std::cout << std::endl
  << "1) Buy" << std::endl
  << "2) Sell" << std::endl
  << "3) Sell Short" << std::endl
  << "4) Sell Short Exempt" << std::endl
  << "5) Cross" << std::endl
  << "6) Cross Short" << std::endl
  << "7) Cross Short Exempt" << std::endl
  << "Side: ";

  std::cin >> value;
  switch ( value )
  {
    case '1': return FIX::Side( FIX::Side_BUY );
    case '2': return FIX::Side( FIX::Side_SELL );
    case '3': return FIX::Side( FIX::Side_SELL_SHORT );
    case '4': return FIX::Side( FIX::Side_SELL_SHORT_EXEMPT );
    case '5': return FIX::Side( FIX::Side_CROSS );
    case '6': return FIX::Side( FIX::Side_CROSS_SHORT );
    case '7': return FIX::Side( 'A' );
    default: throw std::exception();
  }
}

FIX::OrderQty Application::queryOrderQty()
{
  long value;
  std::cout << std::endl << "OrderQty: ";
  std::cin >> value;
  return FIX::OrderQty( value );
}

FIX::OrdType Application::queryOrdType()
{
  char value;
  std::cout << std::endl
  << "1) Market" << std::endl
  << "2) Limit" << std::endl
  << "3) Stop" << std::endl
  << "4) Stop Limit" << std::endl
  << "OrdType: ";

  std::cin >> value;
  switch ( value )
  {
    case '1': return FIX::OrdType( FIX::OrdType_MARKET );
    case '2': return FIX::OrdType( FIX::OrdType_LIMIT );
    case '3': return FIX::OrdType( FIX::OrdType_STOP );
    case '4': return FIX::OrdType( FIX::OrdType_STOP_LIMIT );
    default: throw std::exception();
  }
}

FIX::Price Application::queryPrice()
{
  double value;
  std::cout << std::endl << "Price: ";
  std::cin >> value;
  return FIX::Price( value );
}

FIX::StopPx Application::queryStopPx()
{
  double value;
  std::cout << std::endl << "StopPx: ";
  std::cin >> value;
  return FIX::StopPx( value );
}

FIX::TimeInForce Application::queryTimeInForce()
{
  char value;
  std::cout << std::endl
  << "1) Day" << std::endl
  << "2) IOC" << std::endl
  << "3) OPG" << std::endl
  << "4) GTC" << std::endl
  << "5) GTX" << std::endl
  << "TimeInForce: ";

  std::cin >> value;
  switch ( value )
  {
    case '1': return FIX::TimeInForce( FIX::TimeInForce_DAY );
    case '2': return FIX::TimeInForce( FIX::TimeInForce_IMMEDIATE_OR_CANCEL );
    case '3': return FIX::TimeInForce( FIX::TimeInForce_AT_THE_OPENING );
    case '4': return FIX::TimeInForce( FIX::TimeInForce_GOOD_TILL_CANCEL );
    case '5': return FIX::TimeInForce( FIX::TimeInForce_GOOD_TILL_CROSSING );
    default: throw std::exception();
  }
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