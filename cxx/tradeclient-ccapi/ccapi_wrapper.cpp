#include "ccapi_wrapper.h"
#include "ccapi_cpp/ccapi_session.h"
#include "ccapi_cpp/ccapi_request.h"


//using ::ccapi::MyEventHandler;
using ::ccapi::Request;
using ::ccapi::Session;
using ::ccapi::SessionConfigs;
using ::ccapi::SessionOptions;
using ::ccapi::UtilSystem;


CCApiWrapper::CCApiWrapper(std::string exchangeName, eventHandlerFunc *eventHandler, void *myEventHandlerObj) {
  this->exchangeName = exchangeName;
  this->myEventHandler = new MyEventHandler(eventHandler, myEventHandlerObj);
}

CCApiWrapper::~CCApiWrapper() {
  this->Stop();
}

void CCApiWrapper::Start() {
  SessionOptions sessionOptions;
  SessionConfigs sessionConfigs;
//  MyEventHandler eventHandler;
  this->session = new Session(sessionOptions, sessionConfigs, this->myEventHandler);
//  Session session(sessionOptions, sessionConfigs, &eventHandler);
}

void CCApiWrapper::Stop() {
  if (this->session != nullptr) {
    this->session->stop();
    delete this->session;
    this->session = nullptr;
  }

  if (this->myEventHandler != nullptr) {
    delete this->myEventHandler;
    this->myEventHandler = nullptr;
  }
}

void CCApiWrapper::Request(int operation, std::string instrument, std::string correlationId,
             std::map<std::string, std::string> credential)  const {
  ::ccapi::Request request(Request::Operation(operation), this->exchangeName, instrument);
  request.appendParam({
      {"side", "SELL"},
      {"type", "STOP_LOSS_LIMIT"},
      {"quantity", "0.0005"},
      {"stopPrice", "20001"},
      {"price", "20000"},
      {"timeInForce", "GTC"},
  });

  this->session->sendRequest(request);
}
