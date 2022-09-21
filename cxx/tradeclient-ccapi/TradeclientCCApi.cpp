#include "TradeclientCCApi.h"
#include "ApplicationCCApi.h"

#ifdef USE_TRADECLIENT_RUST_INTERFACE
#include "quickfix-rs/src/lib.rs.h"

#include "../../src/getopt-repl.h"
#endif


#include <fstream>
#include <iostream>
#include <string>

// #include "quickfix/FixValues.h"
#include "quickfix/Values.h"
#include "ccapi_cpp/ccapi_macro.h"



const std::map<ccapi::Message::Type, const char> execTypeMap = {
  {ccapi::Message::Type::CREATE_ORDER, FIX::ExecType_NEW},
  {ccapi::Message::Type::CANCEL_ORDER, FIX::ExecType_CANCELLED},
  {ccapi::Message::Type::GET_ORDER, FIX::ExecType_ORDER_STATUS},
  {ccapi::Message::Type::GET_OPEN_ORDERS, FIX::ExecType_ORDER_STATUS},
  {ccapi::Message::Type::CANCEL_OPEN_ORDERS, FIX::ExecType_CANCELLED},

  {ccapi::Message::Type::GET_ACCOUNTS, ExecType_GET_ACCOUNTS},
  {ccapi::Message::Type::GET_ACCOUNT_BALANCES, ExecType_GET_ACCOUNT_BALANCES},
  {ccapi::Message::Type::GET_ACCOUNT_POSITIONS, ExecType_GET_ACCOUNT_POSITIONS},
  // {_, ExecType_EXPIRED},

  {ccapi::Message::Type::RESPONSE_ERROR, ExecType_RESPONSE_ERROR},
  {ccapi::Message::Type::REQUEST_FAILURE, ExecType_REQUEST_FAILURE},

};

char TradeClientCCApi::convert_exec_type_to_fix(ccapi::Message::Type type) {
  char result = ExecType_UNKOWN;
  std::map<ccapi::Message::Type, const char>::const_iterator it = execTypeMap.find(type);
    if(it != execTypeMap.end()) {
      result = it->second;
    }

  return result;
}

std::string TradeClientCCApi::convert_to_fix(const std::string &name, ccapi::Element elem) {
    std::string result = "";
    const std::map<std::string, std::string> &fixFieldNameMap = {
      {"BUY", std::string(1, FIX::Side_BUY)},
      {"SELL", std::string(1, FIX::Side_SELL)},

      /*
      Execution types:
NEW - The order has been accepted into the engine.
CANCELED - The order has been canceled by the user.
REPLACED (currently unused)
REJECTED - The order has been rejected and was not processed. (This is never pushed into the User Data Stream)
TRADE - Part of the order or all of the order's quantity has filled.
EXPIRED - The order was canceled according to the order type's rules 
(e.g. LIMIT FOK orders with no fill, LIMIT IOC or MARKET orders that partially fill) or by the exchange, 
(e.g. orders canceled during liquidation, orders canceled during maintenance)

  const char OrdStatus_NEW = '0';
  const char OrdStatus_PARTIALLY_FILLED = '1';
  const char OrdStatus_FILLED = '2';
  const char OrdStatus_DONE_FOR_DAY = '3';
  const char OrdStatus_CANCELED = '4';
  const char OrdStatus_REPLACED = '5';
  const char OrdStatus_PENDING_CANCEL_REPLACE = '6';
  const char OrdStatus_STOPPED = '7';
  const char OrdStatus_REJECTED = '8';
  const char OrdStatus_SUSPENDED = '9';
  const char OrdStatus_PENDING_NEW = 'A';
  const char OrdStatus_CALCULATED = 'B';
  const char OrdStatus_EXPIRED = 'C';
  const char OrdStatus_PENDING_CANCEL = '6';
  const char OrdStatus_ACCEPTED_FOR_BIDDING = 'D';
  const char OrdStatus_PENDING_REPLACE = 'E';
  */
        {"NEW", std::string(1, FIX::OrdStatus_NEW)},
        {"CANCELED", std::string(1, FIX::OrdStatus_CANCELED)},
        {"REPLACED", std::string(1, FIX::OrdStatus_REPLACED)},
        {"REJECTED", std::string(1, FIX::OrdStatus_REJECTED)},
        {"TRADE", std::string(1, FIX::OrdStatus_PARTIALLY_FILLED)}, //这里要有一个特殊处理
        {"EXPIRED", std::string(1, FIX::OrdStatus_EXPIRED)},

    };
    std::map<std::string, std::string>::const_iterator it = fixFieldNameMap.find(name);
    if(it != fixFieldNameMap.end()) {
      result = it->second;
      if (name == "TRADE") {
        //币安的TRADE状态根据成交情况，对应fix的两个状态
        std::string filledQuantity = elem.getValue(CCAPI_EM_ORDER_CUMULATIVE_FILLED_QUANTITY);
        std::string orderQuantity = elem.getValue(CCAPI_EM_ORDER_CUMULATIVE_FILLED_QUANTITY);
        result = FIX::OrdStatus_FILLED;
      }
    }
  
    return result;
}

#ifdef USE_TRADECLIENT_RUST_INTERFACE
TradeClientCCApi::TradeClientCCApi(
    const std::string &filepath, rust::Box<TradingClientContext> ctx,
    rust::Fn<void(const QuickFixMessage,
                  const rust::Box<TradingClientContext> &)>
        inbound_callback)
    : ctx(std::move(ctx)), inbound_callback(inbound_callback),
      application(TradeClientCCApi::eventHandler, this) {}
#else
TradeClientCCApi::TradeClientCCApi(const std::string &filepath, FromAppCallback cb)
    : eventCallback(cb), application(TradeClientCCApi::eventHandler, this) {}
TradeClientCCApi::TradeClientCCApi(const std::string &filepath, FromAppCallbackExecutionReport cb)
    : executionReportCallback(cb), application(TradeClientCCApi::eventHandler, this) {}    
#endif
TradeClientCCApi::~TradeClientCCApi() {
  // delete this->initiator;
  // this->initiator = nullptr;
}


bool TradeClientCCApi::eventHandler(void *obj, const ccapi::Event &event,
                                    ccapi::Session *session) {
  std::cout << "Received an event in eventHandler(TradeClientCCApi):\n" +
                   event.toStringPretty(2, 2)
            << std::endl;
  auto pObj = (TradeClientCCApi *)(obj);
  auto content = event.toStringPretty(2, 2);
  if (pObj->eventCallback != nullptr) {
    pObj->eventCallback(content, "0");
  }

  if (pObj->executionReportCallback != nullptr) {
      std::vector<ExecutionReport> excutionReportList;
      std::vector<ccapi::Message> messagelist = event.getMessageList();
      ccapi::Event::Type eventType = event.getType();

      for (const auto& message : messagelist) {
          ccapi::Message::Type messageType = message.getType();
          std::cout << "message.messageType:" << int(messageType) << std::endl;
          const auto elemList = message.getElementList();
          for (const auto& elem : elemList) {
              // std::cout << "elem:" << elem.toStringPretty() << std::endl;
              ExecutionReport aExecutionReport;

              aExecutionReport.ExecType = convert_exec_type_to_fix(messageType);

              if (messageType == ccapi::Message::Type::RESPONSE_ERROR
               || messageType == ccapi::Message::Type::REQUEST_FAILURE) {

                aExecutionReport.ErrorMessage = elem.getValue("ERROR_MESSAGE");
                aExecutionReport.httpStatusCode = atoi(elem.getValue("HTTP_STATUS_CODE").c_str());

               } else if (messageType == ccapi::Message::Type::CREATE_ORDER) {
                
                aExecutionReport.ClOrdId = elem.getValue(CCAPI_EM_ORDER_ID);
                aExecutionReport.Symbol = elem.getValue(CCAPI_EM_ORDER_INSTRUMENT);
                aExecutionReport.OrderID = elem.getValue(CCAPI_EM_CLIENT_ORDER_ID);

                aExecutionReport.OrdStatus = FIX::OrdStatus_NEW;
                
               } else if (messageType == ccapi::Message::Type::CANCEL_OPEN_ORDERS) {

                aExecutionReport.ClOrdId = elem.getValue(CCAPI_EM_ORDER_ID);
                aExecutionReport.Symbol = elem.getValue(CCAPI_EM_ORDER_INSTRUMENT);
                aExecutionReport.OrderID = elem.getValue(CCAPI_EM_CLIENT_ORDER_ID);

                aExecutionReport.OrderID = atof(elem.getValue(CCAPI_EM_ORDER_CUMULATIVE_FILLED_PRICE_TIMES_QUANTITY).c_str());
                aExecutionReport.OrderID = atof(elem.getValue(CCAPI_EM_ORDER_CUMULATIVE_FILLED_QUANTITY).c_str());
                aExecutionReport.OrderID = atof(elem.getValue(CCAPI_EM_ORDER_LIMIT_PRICE).c_str());
                aExecutionReport.OrderID = atof(elem.getValue(CCAPI_EM_ORDER_QUANTITY).c_str());
                aExecutionReport.Side = convert_to_fix(elem.getValue(CCAPI_EM_ORDER_SIDE), elem).c_str()[0]; //这里是std::string转char，但这种方法不是很好
                aExecutionReport.OrdStatus = convert_to_fix(elem.getValue(CCAPI_EM_ORDER_STATUS), elem).c_str()[0]; 
                // assert(elem.getValue("STATUS")  == APP_EVENT_HANDLER_BASE_ORDER_STATUS_CANCELED);
                

               } else if (messageType == ccapi::Message::Type::GET_OPEN_ORDERS) {
                aExecutionReport.ClOrdId = elem.getValue(CCAPI_EM_ORDER_ID);
                aExecutionReport.Symbol = elem.getValue(CCAPI_EM_ORDER_INSTRUMENT);
                aExecutionReport.OrderID = elem.getValue(CCAPI_EM_CLIENT_ORDER_ID);

                // _ = atof(elem.getValue(CUMULATIVE_FILLED_PRICE_TIMES_QUANTITY).c_str()); TODO 暂未找到对应字段
                aExecutionReport.CumQty = atof(elem.getValue(CCAPI_EM_ORDER_CUMULATIVE_FILLED_QUANTITY).c_str());
                aExecutionReport.OrderQty = atof(elem.getValue(CCAPI_EM_ORDER_QUANTITY).c_str());

                aExecutionReport.LimitPrice = atof(elem.getValue(CCAPI_EM_ORDER_LIMIT_PRICE).c_str());

                aExecutionReport.Side = convert_to_fix(elem.getValue(CCAPI_EM_ORDER_SIDE), elem).c_str()[0]; 
                aExecutionReport.OrdStatus = convert_to_fix(elem.getValue(CCAPI_EM_ORDER_STATUS), elem).c_str()[0]; 
            
               } else if (messageType == ccapi::Message::Type::CANCEL_ORDER) {
                aExecutionReport.ClOrdId = elem.getValue(CCAPI_EM_ORDER_ID);
                aExecutionReport.Symbol = elem.getValue(CCAPI_EM_ORDER_INSTRUMENT);
                aExecutionReport.OrderID = elem.getValue(CCAPI_EM_CLIENT_ORDER_ID);

                // _ = atof(elem.getValue(CUMULATIVE_FILLED_PRICE_TIMES_QUANTITY).c_str()); TODO 暂未找到对应字段
                aExecutionReport.CumQty = atof(elem.getValue(CCAPI_EM_ORDER_CUMULATIVE_FILLED_QUANTITY).c_str());
                aExecutionReport.OrderQty = atof(elem.getValue(CCAPI_EM_ORDER_QUANTITY).c_str());

                aExecutionReport.LimitPrice = atof(elem.getValue(CCAPI_EM_ORDER_LIMIT_PRICE).c_str());

                aExecutionReport.Side = convert_to_fix(elem.getValue(CCAPI_EM_ORDER_SIDE), elem).c_str()[0]; 
                aExecutionReport.OrdStatus = convert_to_fix(elem.getValue(CCAPI_EM_ORDER_STATUS), elem).c_str()[0]; 
               } else if (messageType == ccapi::Message::Type::EXECUTION_MANAGEMENT_EVENTS_ORDER_UPDATE) {
                aExecutionReport.ClOrdId = elem.getValue(CCAPI_EM_ORDER_ID);
                aExecutionReport.Symbol = elem.getValue(CCAPI_EM_ORDER_INSTRUMENT);
                aExecutionReport.OrderID = elem.getValue(CCAPI_EM_CLIENT_ORDER_ID);

                // _ = atof(elem.getValue(CUMULATIVE_FILLED_PRICE_TIMES_QUANTITY).c_str()); TODO 暂未找到对应字段
                aExecutionReport.CumQty = atof(elem.getValue(CCAPI_EM_ORDER_CUMULATIVE_FILLED_QUANTITY).c_str());
                aExecutionReport.OrderQty = atof(elem.getValue(CCAPI_EM_ORDER_QUANTITY).c_str());

                aExecutionReport.LimitPrice = atof(elem.getValue(CCAPI_EM_ORDER_LIMIT_PRICE).c_str());

                aExecutionReport.Side = convert_to_fix(elem.getValue(CCAPI_EM_ORDER_SIDE), elem).c_str()[0]; 
                aExecutionReport.OrdStatus = convert_to_fix(elem.getValue(CCAPI_EM_ORDER_STATUS), elem).c_str()[0]; 
                
               } else {
                // TODO这里后续要加逻辑，elem有值才会更新对应字段
                aExecutionReport.ClOrdId = elem.getValue(CCAPI_EM_ORDER_ID);
                aExecutionReport.Symbol = elem.getValue(CCAPI_EM_ORDER_INSTRUMENT);
                aExecutionReport.OrderID = elem.getValue(CCAPI_EM_CLIENT_ORDER_ID);

                // _ = atof(elem.getValue(CUMULATIVE_FILLED_PRICE_TIMES_QUANTITY).c_str()); TODO 暂未找到对应字段
                aExecutionReport.CumQty = atof(elem.getValue(CCAPI_EM_ORDER_CUMULATIVE_FILLED_QUANTITY).c_str());
                aExecutionReport.OrderQty = atof(elem.getValue(CCAPI_EM_ORDER_QUANTITY).c_str());

                aExecutionReport.LimitPrice = atof(elem.getValue(CCAPI_EM_ORDER_LIMIT_PRICE).c_str());

                aExecutionReport.Side = convert_to_fix(elem.getValue(CCAPI_EM_ORDER_SIDE), elem).c_str()[0]; 
                aExecutionReport.OrdStatus = convert_to_fix(elem.getValue(CCAPI_EM_ORDER_STATUS), elem).c_str()[0]; 
               }
   
              excutionReportList.push_back(aExecutionReport);
          }
      }
      
    pObj->executionReportCallback(excutionReportList, "0");
  }
  
#ifdef USE_TRADECLIENT_RUST_INTERFACE

  QuickFixMessage quick_fix_message{
    content : std::make_unique<std::string>(std::move(content)),
    from : FixMessageType::App, // 1是借用quickfix里面的定义，代表from app
  };
  pObj->inbound_callback(std::move(quick_fix_message), pObj->ctx);

#endif

  return true;
}

auto TradeClientCCApi::start() const -> void {
  try {
    // this->initiator->start();
    this->application.start();
  } catch (std::exception &e) {
    std::cout << e.what() << std::endl;
  }
}

auto TradeClientCCApi::stop() const -> void {
  // this->initiator->stop();
  this->application.stop();
}

auto TradeClientCCApi::put_order(const std::string &symbol, char side, 
                 const double quantity,const double price, const double stop_price,
                 const char order_type,const char time_in_force) const
    -> std::unique_ptr<std::string> {
  return this->application.new_order_single(symbol, "", side, quantity, price, stop_price, 
                                            order_type, time_in_force);
}

auto TradeClientCCApi::cancel_order(const std::string &symbol, const std::string &order_id) const -> void {
  this->application.cancel_order(symbol, order_id);
}


auto TradeClientCCApi::put_order(const NewOrderSingle &aNewOrderSingle) const
-> std::unique_ptr<std::string> {
    return this->application.new_order_single(aNewOrderSingle.Symbol, aNewOrderSingle.ClOrdID, aNewOrderSingle.Side,
                                              aNewOrderSingle.Quantity, aNewOrderSingle.Price, aNewOrderSingle.StopPrice,
                                              aNewOrderSingle.OrdType, aNewOrderSingle.TimeInForce);
}

auto TradeClientCCApi::cancel_order(const OrderCancelRequest &aOrderCancelRequest) const -> void {
  this->application.cancel_order(aOrderCancelRequest.Symbol, aOrderCancelRequest.OrdID);
}

auto TradeClientCCApi::cancel_open_orders(const std::string &symbol) const -> void {
  this->application.cancel_open_orders(symbol);
}

auto TradeClientCCApi::get_order(const std::string &symbol, const std::string &order_id) const -> void {
  this->application.get_order(symbol, order_id);
}

auto TradeClientCCApi::get_open_orders(const std::string &symbol) const -> void {
  this->application.get_open_orders(symbol);
}

auto TradeClientCCApi::get_recent_trades(const std::string &symbol) const -> void {
  this->application.get_recent_trades(symbol);
}

auto TradeClientCCApi::get_account_balances() const -> void {
  this->application.get_account_balances();
}

auto TradeClientCCApi::get_accounts() const -> void {
  this->application.get_accounts();
}

auto TradeClientCCApi::get_account_postions() const -> void {
  this->application.get_account_postions();
}

/*auto create_client_ccapi(
    const TradingClientType type, const std::string &filepath,
    rust::Box<TradingClientContext> ctx,
    rust::Fn<void(const QuickFixMessage, const rust::Box<TradingClientContext>
&)> inbound_callback) -> std::unique_ptr<ITradeClient> { ITradeClient
*pitradeclient = nullptr; switch (type) { case TradingClientType::Apifiny:
    pitradeclient =
        new TradeClientApifiny(filepath, std::move(ctx), inbound_callback);
    break;
  case TradingClientType::Wintmute:
    break;
  }

  return std::unique_ptr<ITradeClient>(pitradeclient);
}*/
