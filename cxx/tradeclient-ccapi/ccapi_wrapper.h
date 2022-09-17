#pragma once


#include <string>
#include <map>

#include "ccapi_cpp/ccapi_session.h"


const int CCAPI_EXECUTION_GET_RECENT_TRADES = int(::ccapi::Request::Operation::GET_RECENT_TRADES);
const int CCAPI_EXECUTION_CREATE_ORDER = int(::ccapi::Request::Operation::CREATE_ORDER);
const int CCAPI_EXECUTION_CANCEL_ORDER = int(::ccapi::Request::Operation::CANCEL_ORDER);

const int CCAPI_EXECUTION_GET_ORDER = int(::ccapi::Request::Operation::GET_ORDER);
const int CCAPI_EXECUTION_GET_OPEN_ORDERS = int(::ccapi::Request::Operation::GET_OPEN_ORDERS);
const int CCAPI_EXECUTION_CANCEL_OPEN_ORDERS = int(::ccapi::Request::Operation::CANCEL_OPEN_ORDERS);
const int CCAPI_EXECUTION_GET_ACCOUNTS = int(::ccapi::Request::Operation::GET_ACCOUNTS);
const int CCAPI_EXECUTION_GET_ACCOUNT_BALANCES = int(::ccapi::Request::Operation::GET_ACCOUNT_BALANCES);
const int CCAPI_EXECUTION_GET_ACCOUNT_POSITIONS = int(::ccapi::Request::Operation::GET_ACCOUNT_POSITIONS);

typedef bool eventHandlerFunc(void *obj, const ccapi::Event& event, ccapi::Session* session);
class MyEventHandler : public ccapi::EventHandler {
 public:
  MyEventHandler() = delete;
  MyEventHandler(eventHandlerFunc *eventHandle, void *myEventHandlerObj) {
    this->eventHandle = eventHandle;
    this->myEventHandlerObj = myEventHandlerObj;
  }

  bool processEvent(const ccapi::Event& event, ccapi::Session* session) override {
    std::cout << "Received an event:\n" + event.toStringPretty(2, 2) << std::endl;
    this->eventHandle(this->myEventHandlerObj, event, session);
    return true;
  }
 private:
  eventHandlerFunc *eventHandle;
  void *myEventHandlerObj;
};

class CCApiWrapper {
    public:
        CCApiWrapper() = delete;
        CCApiWrapper(std::string exchaneName, eventHandlerFunc *eventHandler, void *myEventHandlerObj);
        ~CCApiWrapper();
        void Start();
        void Stop();
        void Request(int operation, std::string instrument = "", const std::string client_order_id = "", const std::string side = "", 
                        const double quantity = 0,const double price = 0, const double stop_price = 0,
                        const std::string order_type = "",const std::string time_in_force = "", 
                        std::string correlationId = "",
                        std::map<std::string, std::string> credential = {}) const ;
    private:
        std::string exchangeName;
        ::ccapi::Session *session;
        MyEventHandler *myEventHandler;

};

