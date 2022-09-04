#include "ApplicationCCApi.h"
// #include "quickfix-rs/src/lib.rs.h"



#include <iostream>
#include <chrono>
#include <openssl/sha.h>
#include <random>

#include <ccapi_cpp/ccapi_session.h>
#include "ccapi_cpp/ccapi_event.h"
#include "ccapi_cpp/ccapi_queue.h"

// #include "quickfix/FixValues.h"
#include "quickfix/Values.h"

static std::string ACCOUNT_ID_CCAPI = "";
static std::string SECRET_KEY_ID_CCAPI = "";
static std::string SECRET_KEY_CCAPI = "";

bool ApplicationCCApi::eventHandler(void *obj, const ccapi::Event& event, ccapi::Session* session) {
  std::cout << "Received an event in eventHandler(Application):\n" + event.toStringPretty(2, 2) << std::endl;
  auto pApplicationCCApi = (ApplicationCCApi*)(obj);
  pApplicationCCApi->eventHandle(pApplicationCCApi->myEventHandlerObj, event, session);
  return true;
}


ApplicationCCApi::ApplicationCCApi(
  eventHandlerFunc *eventHandle, void *myEventHandlerObj
    )
    : eventHandle(eventHandle), myEventHandlerObj(myEventHandlerObj),
    ccapiwrapper(CCApiWrapper("binance", ApplicationCCApi::eventHandler, this)) {
      // this->ccapiwrapper = new CCApiWrapper("binance-us", eventHandler);
      this->ccapiwrapper.Start(); // symbol = "BTCUSDT"
    }

ApplicationCCApi::~ApplicationCCApi() {
      // this->ccapiwrapper = new CCApiWrapper("binance-us", eventHandler);
      this->ccapiwrapper.Stop(); // symbol = "BTCUSDT" 
    }

auto ApplicationCCApi::new_order_single(const std::string &symbol, char side, 
                        const double quantity,const double price, const double stop_price,
                        const char order_type,const char time_in_force) const
    -> std::unique_ptr<std::string> {
  // const auto order_id = generate_order_id_2(ACCOUNT_ID_CCAPI);

  std::string _side;
  std::string _order_type;
  std::string _time_in_force;

  _side = convertSide(side);
  _order_type = convertOrderType(order_type);
  _time_in_force = convertTimeInForce(time_in_force);

  this->ccapiwrapper.Request(CCAPI_EXECUTION_CREATE_ORDER, symbol, _side, 
                        quantity, price, stop_price,
                        _order_type, _time_in_force); // symbol = "BTCUSDT"
  // return std::make_unique<std::string>(std::move(order_id));
  std::string order_id = "0"; //TODO: 暂时这样先编译通过
  return std::make_unique<std::string>(std::move(order_id));
}

auto ApplicationCCApi::cancel_order(const std::string &order_id) const -> void {
  this->ccapiwrapper.Request(CCAPI_EXECUTION_CANCEL_ORDER, order_id); // symbol = "BTCUSDT"
}

auto ApplicationCCApi::start() const ->  void {
  // this->ccapiwrapper.Start(); // symbol = "BTCUSDT"
}


auto ApplicationCCApi::stop() const -> void {
  // this->ccapiwrapper.Stop(); // symbol = "BTCUSDT"
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

/*
 https://www.onixs.biz/fix-dictionary/4.4/tagNum_54.html
-s side
1 = Buy
2 = Sell
3 = Buy minus
4 = Sell plus
5 = Sell short
6 = Sell short exempt


binance: https://binance-docs.github.io/apidocs/spot/en/#public-api-definitions
Order types (orderTypes, type):

LIMIT
MARKET
STOP_LOSS
STOP_LOSS_LIMIT
TAKE_PROFIT
TAKE_PROFIT_LIMIT
LIMIT_MAKER


Order Response Type (newOrderRespType):

ACK
RESULT
FULL
Order side (side):

BUY
SELL
Time in force (timeInForce):

This sets how long an order will be active before expiration.

Status	Description
GTC	Good Til Canceled
An order will be on the book unless the order is canceled.
IOC	Immediate Or Cancel
An order will try to fill the order as much as it can before the order expires.
FOK	Fill or Kill
An order will expire if the full order cannot be filled upon e
*****************************

*/
auto ApplicationCCApi::convertSide(const char side) const -> std::string {
  std::string _side = "";
  switch(side) {
    case FIX::Side_BUY:
      _side = "BUY";
    break;
    
    case ::FIX::Side_SELL:
      _side = "SELL";
    break;
    
    default:
        // throw std::exception::exception("convertSide not support side value");
        throw "convertOrderType not support order_type value";
    break;

  }
  return _side;
}

/*
https://www.onixs.biz/fix-dictionary/4.4/tagNum_40.html
    
 -o order_type
1 = Market
2 = Limit
3 = Stop
4 = Stop limit
*/
auto ApplicationCCApi::convertOrderType(const char order_type) const -> std::string {
  std::string _order_type = "";
  switch(order_type) {
    case FIX::OrdType_MARKET:
      _order_type = "MARKET";
    break;
    
    case FIX::OrdType_LIMIT:
      _order_type = "LIMIT";
    break;

    case FIX::OrdType_STOP:
      _order_type = "STOP_LOSS";
    break;

    case FIX::OrdType_STOP_LIMIT:
      _order_type = "STOP_LOSS_LIMIT";
    break;        
    
    default:
        // throw std::exception::exception("convertOrderType not support order_type value");
        throw "convertOrderType not support order_type value";
    break;

  }
  return _order_type;
}

/*
https://www.onixs.biz/fix-dictionary/4.4/tagNum_59.html
-t time_in_force
0 = Day (or session)
1 = Good Till Cancel (GTC)
2 = At the Opening (OPG)
3 = Immediate or Cancel (IOC)
4 = Fill or Kill (FOK)
5 = Good Till Crossing (GTX)
6 = Good Till Date
7 = At the Close
*/
auto ApplicationCCApi::convertTimeInForce(const char time_in_force) const -> std::string {
  std::string _time_in_force = "";
  switch(time_in_force) {
    // case Fix::TimeInForce_DAY: //'0'
    //   _time_in_force = "_MARKET";
    // break;
    
    case FIX::TimeInForce_GOOD_TILL_CANCEL: //'1'
      _time_in_force = "GTC";
    break;

    // case FIX::TimeInForce_AT_THE_OPENING: //'2'
    //   _time_in_force = "LIMIT";
    // break;

    case FIX::TimeInForce_IMMEDIATE_OR_CANCEL: //'3'
      _time_in_force = "IOC";
    break;

    case FIX::TimeInForce_FILL_OR_KILL: //'4'
      _time_in_force = "FOK";
    break;

    // case FIX::TimeInForce_GOOD_TILL_CROSSING: //'5'
    //   _time_in_force = "LIMIT";
    // break;

    // case FIX::TimeInForce_GOOD_TILL_DATE: //'6'
    //   _time_in_force = "LIMIT";
    // break;    

    // case FIX::TimeInForce_AT_THE_CLOSE: //'7'
    //   _time_in_force = "LIMIT";
    // break;     

    // case FIX::TimeInForce_GOOD_THROUGH_CROSSING: //'8'
    //   _time_in_force = "LIMIT";
    // break;     

    // case FIX::TimeInForce_AT_CROSSING: //'9'
    //   _time_in_force = "LIMIT";
    // break;                                 
    
    default:
        // throw std::exception::exception("convertOrderType not support order_type value");
        throw "convertOrderType not support time_in_force value";
    break;

  }
  return _time_in_force;
}