#pragma once

#include <string>
#include <map>

#include "ccapi_cpp/ccapi_session.h"


const int CCAPI_EXECUTION_CREATE_ORDER = int(::ccapi::Request::Operation::CREATE_ORDER);
const int CCAPI_EXECUTION_CANCEL_ORDER = int(::ccapi::Request::Operation::CANCEL_ORDER);




typedef bool eventHandlerFunc(const ccapi::Event& event, ccapi::Session* session);
class MyEventHandler : public ccapi::EventHandler {
 public:
  MyEventHandler() = delete;
  MyEventHandler(eventHandlerFunc *eventHandle) {this->eventHandle = eventHandle;}

  bool processEvent(const ccapi::Event& event, ccapi::Session* session) override {
    std::cout << "Received an event:\n" + event.toStringPretty(2, 2) << std::endl;
    this->eventHandle(event, session);
    return true;
  }
 private:
  eventHandlerFunc *eventHandle;
};

class CCApiWrapper {
    public:
        CCApiWrapper() = delete;
        CCApiWrapper(std::string exchaneName, eventHandlerFunc *eventHandler);
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

