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
      for (const auto& message : messagelist) {
          auto messageType = message.getType();
          std::cout << "message.messageType:" << int(messageType) << std::endl;
          const auto elemList = message.getElementList();
          for (const auto& elem : elemList) {
              // std::cout << "elem:" << elem.toStringPretty() << std::endl;
              ExecutionReport aExecutionReport;
              
              auto errorMessage = elem.getValue("ERROR_MESSAGE");
              auto httpSatusCode = elem.getValue("HTTP_STATUS_CODE");
              std::cout << "elem.errorMessage:" << errorMessage << std::endl;
              std::cout << "elem.httpSatusCode:" << httpSatusCode << std::endl;
              if (messageType == ccapi::Message::Type::RESPONSE_ERROR
               || messageType == ccapi::Message::Type::REQUEST_FAILURE) {
                auto errorMessage = elem.getValue("ERROR_MESSAGE");
                auto httpSatusCode = elem.getValue("HTTP_STATUS_CODE");

               } else if (messageType == ccapi::Message::Type::CREATE_ORDER) {
                aExecutionReport.ClOrdId = elem.getValue("CLIENT_ORDER_ID");
                aExecutionReport.Symbol = elem.getValue("INSTRUMENT");
                aExecutionReport.ClOrdId = elem.getValue("ORDER_ID");
                aExecutionReport.OrdStatus = FIX::OrdStatus_NEW;
               } else if (messageType == ccapi::Message::Type::CANCEL_OPEN_ORDERS) {
                aExecutionReport.ClOrdId = elem.getValue("CLIENT_ORDER_ID");
                aExecutionReport.Symbol = elem.getValue("INSTRUMENT");
                aExecutionReport.ClOrdId = elem.getValue("ORDER_ID");
                aExecutionReport.OrdStatus = FIX::OrdStatus_CANCELED;
               } else if (messageType == ccapi::Message::Type::CANCEL_ORDER) {
                aExecutionReport.ClOrdId = elem.getValue("CLIENT_ORDER_ID");
                aExecutionReport.Symbol = elem.getValue("INSTRUMENT");
                aExecutionReport.ClOrdId = elem.getValue("ORDER_ID");
                aExecutionReport.OrdStatus = FIX::OrdStatus_CANCELED;
               } else if (messageType == ccapi::Message::Type::EXECUTION_MANAGEMENT_EVENTS_ORDER_UPDATE) {
                aExecutionReport.ClOrdId = elem.getValue("CLIENT_ORDER_ID");
                aExecutionReport.Symbol = elem.getValue("INSTRUMENT");
                aExecutionReport.ClOrdId = elem.getValue("ORDER_ID");
               } 
   
              excutionReportList.push_back(aExecutionReport);
          }
      }
      
    // pObj->executionReportCallback(content, "0");
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
  return this->application.new_order_single(symbol, side, quantity, price, stop_price, 
                                            order_type, time_in_force);
}

auto TradeClientCCApi::put_order(const NewOrderSingle &aNewOrderSingle) const
-> std::unique_ptr<std::string> {
    return this->application.new_order_single(aNewOrderSingle.Symbol, aNewOrderSingle.Side,
                                              aNewOrderSingle.Quantity, aNewOrderSingle.Price, aNewOrderSingle.StopPrice,
                                              aNewOrderSingle.OrdType, aNewOrderSingle.TimeInForce);
}

auto TradeClientCCApi::cancel_order(const std::string &order_id) const -> void {
  this->application.cancel_order(order_id);
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
