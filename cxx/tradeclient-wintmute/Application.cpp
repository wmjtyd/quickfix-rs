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
//#include <picojson/picojson.h>
#include <iostream>
#include <random>
#include <chrono>


std::string SYMBOL = "BTC/USDT";
std::string Currency = "USD";
std::string SenderCompID = "";
std::string TargetCompID = "";


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
Application::Application()
{
    mUUID.init(1, 1);
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
        message.getHeader().setField(FIX::Username(""));
        message.getHeader().setField(FIX::Password(""));

        if (true == message.isSetField(FIX::FIELD::ResetSeqNumFlag))
        {
            std::cout << "Sending admin: Logging in user" << message.getHeader().getField(FIX::FIELD::Username)
//                << "reset seq" << message.getHeader().getField(FIX::FIELD::ResetSeqNumFlag)
//                << "at" << message.getHeader().getField(FIX::FIELD::SendingTime)
//                << "seq" << message.getHeader().getField(FIX::FIELD::MsgSeqNum)
                << "for session" << sessionID << std::endl;
        }
        else
        {
            std::cout << "Sending admin: Logging in user" << message.getHeader().getField(FIX::FIELD::Username)
                << "at" << message.getHeader().getField(FIX::FIELD::SendingTime)
                << "seq" << message.getHeader().getField(FIX::FIELD::MsgSeqNum)
                << "for session" << sessionID << std::endl;
        }
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
  crack( message, sessionID );
  //std::cout << std::endl << "IN: " << message << std::endl;
  std::string m = message.toString();
  replace_str(m, __SOH__, "|");
  std::cout << std::endl << "IN: " << m << std::endl;
}

void Application::toApp( FIX::Message& message, const FIX::SessionID& sessionID )
EXCEPT( FIX::DoNotSend )
{
    std::cout << std::endl << "toApp: " << sessionID << "message" << message << std::endl;
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


// Quote Response
void Application::onMessage
( const FIX44::Quote& quote, const FIX::SessionID& sessionId) {
    std::cout << std::endl
              << "onMessage: FIX::SessionID:" << sessionId << std::endl
              << "onMessage: FIX42::quote:" << quote << std::endl;
}
// MarketDataSnapshotFullRefresh
void Application::onMessage
( const FIX44::MarketDataSnapshotFullRefresh& marketDataSnapshotFullRefresh, const FIX::SessionID& sessionId) {
    std::cout << std::endl
              << "onMessage: FIX::SessionID:" << sessionId << std::endl
              << "onMessage: FIX42::marketDataSnapshotFullRefresh:" << marketDataSnapshotFullRefresh << std::endl;
}

// ExecutionReport
void Application::onMessage
( const FIX44::ExecutionReport& executionReport, const FIX::SessionID& sessionId) {
    std::cout << std::endl
    << "onMessage: FIX::SessionID:" << sessionId << std::endl
    << "onMessage: FIX42::ExecutionReport:" << executionReport << std::endl;
}
// OrderCancelReject
void Application::onMessage
( const FIX44::OrderCancelReject& orderCancelReject, const FIX::SessionID& sessionId) {
    std::cout << std::endl
              << "onMessage: FIX::SessionID:" << sessionId << std::endl
              << "onMessage: FIX42::orderCancelReject:" << orderCancelReject << std::endl;
}
// PositionReport
void Application::onMessage
( const FIX44::PositionReport& positionReport, const FIX::SessionID& sessionId) {
    std::cout << std::endl
              << "onMessage: FIX::SessionID:" << sessionId << std::endl
              << "onMessage: FIX42::positionReport:" << positionReport << std::endl;
}
// SecurityList
void Application::onMessage
( const FIX44::SecurityList& securityList, const FIX::SessionID& sessionId) {
    std::cout << std::endl
              << "onMessage: FIX::SessionID:" << sessionId << std::endl
              << "onMessage: FIX42::securityList:" << securityList << std::endl;
}
// change_password
void Application::onMessage
( const FIX44::UserResponse& userResponse, const FIX::SessionID& sessionId) {
    std::cout << std::endl
              << "onMessage: FIX::SessionID:" << sessionId << std::endl
              << "onMessage: FIX42::userResponse:" << userResponse << std::endl;
}

void Application::run()
{
  while ( true )
  {
    try
    {
        char action = queryAction();
        if ( action == '1' )
            put_quote(FIX::Symbol( "USDT/USD" ), FIX::Currency( "USDT/USD" ), FIX::Side_BUY, FIX::OrderQty( 1 ));
        else if ( action == '2' )
            put_order(FIX::QuoteID(generateID()), FIX::Symbol( "USDT/USD" ), FIX::Currency( "USDT/USD" ),
                      FIX::Side_BUY, FIX::OrderQty( 1 ), FIX::Price( 1 ), FIX::TimeInForce_IMMEDIATE_OR_CANCEL);
        else if ( action == '3' )
            put_subscribe(FIX::Symbol( "USDT/USD" ), true);
        else if ( action == '4' )
            put_position(FIX::Currency( "USDT/USD" ), false, true);
        else if ( action == '5' )
            put_security(FIX::Symbol( "USDT/USD" ));
        else if ( action == '6' )
            put_change_password(FIX::Username(""), FIX::Password(""), FIX::Password(""));
        else if ( action == '7' )
            break;
    }
    catch ( std::exception & e )
    {
      std::cout << "Message Not Sent: " << e.what();
    }
  }
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
  << "1) put_quote" << std::endl
  << "2) put_order" << std::endl
  << "3) put_subscribe" << std::endl
  << "4) put_position" << std::endl
  << "5) put_security" << std::endl
  << "6) put_change_password" << std::endl
  << "7) Quit" << std::endl
  << "Action: ";
  std::cin >> value;
  switch ( value )
  {
    case '1': case '2': case '3': case '4': case '5': case '6': case '7':break;
    default: throw std::exception();
  }
  return value;
}

char Application::queryTestAction()
{
    char value;
    std::cout << std::endl
              << "1) Enter Order" << std::endl
              << "Action: ";
    std::cin >> value;
    switch ( value )
    {
        case '1': break;
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

void Application::put_quote(FIX::Symbol symbol, FIX::Currency currency, FIX::Side side, FIX::OrderQty quantity)
{
    FIX44::QuoteRequest quoteRequest;
    quoteRequest.set( FIX::QuoteReqID(generateID()) ); // String (max length 31 chars) Unique ID provided by the client [a-zA-Z0-9._-]
    /*quoteRequest.set( FIX::Symbol( symbol ) );
      quoteRequest.set( FIX::Side( FIX::Side_BUY ) );
      quoteRequest.set( FIX::OrderQty( 1 ) );*/
    FIX44::QuoteRequest::NoRelatedSym noRelatedSym;
    noRelatedSym.set( FIX::Symbol( symbol ) );
    noRelatedSym.set( side );
    noRelatedSym.set( quantity );
    quoteRequest.addGroup(noRelatedSym);

    FIX::Header& header = quoteRequest.getHeader();
    header.setField( FIX::SenderCompID(SenderCompID) );
    header.setField( FIX::TargetCompID(TargetCompID) );

    FIX::Session::sendToTarget( quoteRequest );
}

void Application::put_order(FIX::QuoteID quoteid, FIX::Symbol symbol, FIX::Currency currency,
    FIX::Side side, FIX::OrderQty quantity, FIX::Price price, FIX::TimeInForce time_in_force)
{

//    msg.setField(fix.ClOrdID(str(self.__genOrderID()))) #11=Unique order
//    if quoteid:
//        msg.setField(fix.QuoteID(quoteid))
//    msg.setField(fix.Symbol(symbol)) #55
//    if currency:
//        msg.setField(fix.Currency(currency))
//    msg.setField(fix.Side(side)) #54=1 Buy
//    msg.setField(fix.Price(price))
//    msg.setField(fix.TimeInForce(time_in_force))
//    msg.setField(fix.OrderQty(quantity)) #38=100
//    fix.Session.sendToTarget(msg, self.__sessionID)

    auto nowUtc = std::chrono::system_clock::now().time_since_epoch().count() / 1000;
    std::cout << "order utc:" << nowUtc << std::endl;
    //std::chrono::system_clock::now();
    //FIX::DateTime();
    FIX::OrdType ordType;
    FIX44::NewOrderSingle newOrderSingle;
    newOrderSingle.set( quoteid );
    newOrderSingle.set( symbol );
    newOrderSingle.set( side );
    newOrderSingle.set( price );
    newOrderSingle.set( time_in_force );
    newOrderSingle.set( FIX::OrderQty( 1 ) );

    FIX::Header& header = newOrderSingle.getHeader();
    header.setField( FIX::SenderCompID(SenderCompID) );
    header.setField( FIX::TargetCompID(TargetCompID) );

    FIX::Session::sendToTarget( newOrderSingle );
}
//def put_subscribe(self, symbol:str, subscribe : bool) 
void Application::put_subscribe(FIX::Symbol symbol, bool subscribe)
{
//    msg = fix.Message()
//    msg.getHeader().setField(fix.BeginString(fix.BeginString_FIX44)) #
//    msg.getHeader().setField(fix.MsgType(fix.MsgType_MarketDataRequest)) #39=V
//    msg.setField(fix.MDReqID(str(self.__genMDReqID())))
//    msg.setField(fix.Symbol(symbol)) #55
//    msg.setField(fix.SubscriptionRequestType('1' if subscribe else '0'))
//    fix.Session.sendToTarget(msg, self.__sessionID)

    FIX::Message message;
    message.getHeader().setField(FIX::BeginString(FIX::BeginString_FIX44));
    message.getHeader().setField(FIX::MsgType(FIX::MsgType_MarketDataRequest)); // 39=AN
    message.setField(FIX::MDReqID(generateID()));
    message.setField(symbol);
    if (subscribe == true){
        message.setField(FIX::SubscriptionRequestType(FIX::SubscriptionRequestType_SNAPSHOT_PLUS_UPDATES)); //'1'
    } else {
        message.setField(FIX::SubscriptionRequestType(FIX::SubscriptionRequestType_SNAPSHOT)); //'0'
    }

    FIX::Header& header = message.getHeader();
    header.setField( FIX::SenderCompID(SenderCompID) );
    header.setField( FIX::TargetCompID(TargetCompID) );

    FIX::Session::sendToTarget( message );

//    FIX44::MarketDataRequest marketDataRequest;
//    marketDataRequest.set( FIX::MDReqID( "MDReqID" ) ); // String (max 15 chars) Unique ID provided by the client [a-zA-Z0-9._-]
//
//    FIX44::MarketDataRequest::NoRelatedSym symbolGroup;
//    symbolGroup.set( symbol );
//    marketDataRequest.addGroup( symbolGroup );
//
//    if (subscribe == true){
//        marketDataRequest.set(FIX::SubscriptionRequestType('1'));
//    } else {
//        marketDataRequest.set(FIX::SubscriptionRequestType('0'));
//    }
//    FIX::Session::sendToTarget( marketDataRequest );
}

// def put_position(self, currency:str, zeroPositions:bool, subscribe:bool):
void Application::put_position(FIX::Currency currency, bool zeroPositions, bool subscribe)
{
//    msg = fix.Message()
//    msg.setField(fix.PosReqID(str(self.__genPosReqID())))
//    msg.setField(fix.SubscriptionRequestType('1' if subscribe else '0'))
//    msg.setField(fix.Currency(currency))
//    msg.setField(fix.BoolField(100551, zeroPositions)) # ZeroPositions
//    fix.Session.sendToTarget(msg, self.__sessionID)

    FIX::Message message;
    message.getHeader().setField(FIX::BeginString(FIX::BeginString_FIX44));
    message.getHeader().setField(FIX::MsgType(FIX::MsgType_RequestForPositions)); // 39=AN
    message.setField(FIX::PosReqID(generateID()));
    message.setField(currency);
    if (subscribe == true){
        message.setField( FIX::SubscriptionRequestType(FIX::SubscriptionRequestType_SNAPSHOT_PLUS_UPDATES));
    } else {
        message.setField( FIX::SubscriptionRequestType(FIX::SubscriptionRequestType_SNAPSHOT)); //'0'
    }
    message.setField( FIX::BoolField(100551, zeroPositions));

    FIX::Header& header = message.getHeader();
    header.setField( FIX::SenderCompID(SenderCompID) );
    header.setField( FIX::TargetCompID(TargetCompID) );

    FIX::Session::sendToTarget( message );
}

// def put_security(self, symbol:str):
void Application::put_security(FIX::Symbol symbol)
{
//    msg.getHeader().setField(fix.MsgType(fix.MsgType_SecurityListRequest)) #39=x
//    msg.setField(fix.SecurityReqID(str(self.__genSecurityReqID())))
//    if symbol:
//        msg.setField(fix.Symbol(symbol)) #55
//    fix.Session.sendToTarget(msg, self.__sessionID)

    FIX44::SecurityListRequest securityListRequest;
    securityListRequest.set( FIX::SecurityReqID(generateID()) ); // String (max 15 chars) Unique ID of this request provided by the client [a-zA-Z0-9._-]
    securityListRequest.set( symbol );

    FIX::Header& header = securityListRequest.getHeader();
    header.setField( FIX::SenderCompID(SenderCompID) );
    header.setField( FIX::TargetCompID(TargetCompID) );

    FIX::Session::sendToTarget( securityListRequest );
}

//def put_change_password(self, change_username, old_password, new_password):
void Application::put_change_password(FIX::Username change_username, FIX::Password old_password, FIX::Password new_password)
{
//    msg.setField(fix.UserRequestID(str(self.__genUserReqID())))
//    msg.setField(fix.UserRequestType(3)) # change password
//    msg.setField(fix.Username(change_username))
//    msg.setField(fix.Password(old_password))
//    msg.setField(fix.NewPassword(new_password))
//    fix.Session.sendToTarget(msg, self.__sessionID)

    FIX44::UserRequest userRequest;
    userRequest.set( FIX::UserRequestID(generateID()) ); // String (max 15 chars) Unique ID provided by the client [a-zA-Z0-9._-
    userRequest.set( FIX::UserRequestType( 3 ) );
    userRequest.set( change_username );
    userRequest.set( old_password );
    userRequest.set( new_password );

    FIX::Header& header = userRequest.getHeader();
    header.setField( FIX::SenderCompID(SenderCompID) );
    header.setField( FIX::TargetCompID(TargetCompID) );

    FIX::Session::sendToTarget( userRequest );
}

//def triger_logon_out(self):
void Application::triger_logon_out()
{
//    msg.getHeader().setField(fix.MsgType(fix.MsgType_Logout)) #35=5
//    fix.Session.sendToTarget(msg, self.__sessionID)
    FIX44::Logout logout;

    FIX::Header& header = logout.getHeader();
    header.setField( FIX::SenderCompID(SenderCompID) );
    header.setField( FIX::TargetCompID(TargetCompID) );

    FIX::Session::sendToTarget( logout );
}


//
std::string Application::generateID()
{
    return std::to_string(mUUID.nextid());
}

