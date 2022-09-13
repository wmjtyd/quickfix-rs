#include "ccapi_wrapper.h"
#include "ccapi_cpp/ccapi_session.h"
#include "ccapi_cpp/ccapi_request.h"


//using ::ccapi::MyEventHandler;
using ::ccapi::Request;
using ::ccapi::Session;
using ::ccapi::SessionConfigs;
using ::ccapi::SessionOptions;
using ::ccapi::UtilSystem;


namespace ccapi {
class MyLogger final : public Logger {
 public:
  void logMessage(const std::string& severity, const std::string& threadId, const std::string& timeISO, const std::string& fileName,
                  const std::string& lineNumber, const std::string& message) override {
    std::lock_guard<std::mutex> lock(m);
    std::cout << threadId << ": [" << timeISO << "] {" << fileName << ":" << lineNumber << "} " << severity << std::string(8, ' ') << message << std::endl;
  }

 private:
  std::mutex m;
};
MyLogger myLogger;
Logger* Logger::logger = &myLogger;
} 


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
  auto url = sessionConfigs.getUrlRestBase().at(this->exchangeName);
  printf("CCApiWrapper::Start exchange(%s) baseurl(%s) \n", this->exchangeName.c_str(), url.c_str());
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

void CCApiWrapper::Request(int operation, std::string instrument, const std::string side, 
                        const double quantity,const double price, const double stop_price,
                        const std::string order_type,const std::string time_in_force,
                        std::string correlationId,
                        std::map<std::string, std::string> credential)  const {
  printf("CCApiWrapper 1: put_order: exchange(%s) symbol(%s) side(%s) quantity(%f) price(%f) order_type(%s) time_in_force(%s)\n", 
                           this->exchangeName.c_str(), instrument.c_str(), side.c_str(), 
                           quantity, price, order_type.c_str(), time_in_force.c_str());                          
  ::ccapi::Request request(Request::Operation(operation), this->exchangeName, instrument);
  std::map<std::string, std::string> param;
  param = {
      {"side", side},
      {"type", order_type},
      {"quantity", std::to_string(quantity)},
      // {"stopPrice", std::to_string(stop_price)},
      {"price", std::to_string(price)},
      // {"timeInForce", time_in_force},
  };
  if (stop_price != 0) {
    param["stopPrice"] = std::to_string(stop_price);
  }

  if (time_in_force != "") {
    param["timeInForce"] = time_in_force;
  }
  request.appendParam(param);
  // request.appendParam({
  //     {"side", "SELL"},
  //     {"type", "STOP_LOSS_LIMIT"},
  //     {"quantity", "0.0005"},
  //     {"stopPrice", "20001"},
  //     {"price", "20000"},
  //     {"timeInForce", "GTC"},
  // });
  printf("CCApiWrapper 2: put_order: exchange(%s) symbol(%s) side(%s) quantity(%f) price(%f) order_type(%s) time_in_force(%s)\n", 
                           this->exchangeName.c_str(), instrument.c_str(), side.c_str(), 
                           quantity, price, order_type.c_str(), time_in_force.c_str());

  this->session->sendRequest(request);
}
