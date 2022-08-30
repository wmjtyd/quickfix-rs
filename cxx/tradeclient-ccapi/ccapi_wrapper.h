#pragma once
#define CCAPI_ENABLE_SERVICE_EXECUTION_MANAGEMENT
#define CCAPI_ENABLE_EXCHANGE_BINANCE

#include <string>
#include <map>

#include "ccapi_cpp/ccapi_session.h"


const int CCAPI_EXECUTION_CREATE_ORDER = int(::ccapi::Request::Operation::CREATE_ORDER);
const int CCAPI_EXECUTION_CANCEL_ORDER = int(::ccapi::Request::Operation::CANCEL_ORDER);




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
        void Request(int operation, std::string instrument = "", std::string correlationId = "",
            std::map<std::string, std::string> credential = {}) const ;
    private:
        std::string exchangeName;
        ::ccapi::Session *session;
        MyEventHandler *myEventHandler;

};

