
#include "ITradeclient.h"
// #include "tradeclient-apifiny/Tradeclient.h"
#include "tradeclient-ccapi/TradeclientCCApi.h"

#ifdef USE_TRADECLIENT_RUST_INTERFACE
#include "quickfix-rs/src/lib.rs.h"
#endif

#ifdef USE_TRADECLIENT_RUST_INTERFACE
auto create_client(
    const TradingClientType type, const std::string &filepath,
    rust::Box<TradingClientContext> ctx,
    rust::Fn<void(const QuickFixMessage, const rust::Box<TradingClientContext> &)>
        inbound_callback) -> std::unique_ptr<ITradeClient> {
  ITradeClient *pitradeclient = nullptr;
  switch (type) {
  case TradingClientType::Apifiny:
    // pitradeclient =
        // new TradeClientApifiny(filepath, std::move(ctx), inbound_callback);
    break;
  case TradingClientType::Wintmute:
    break;
  case TradingClientType::CCApi:
    pitradeclient =
        new TradeClientCCApi(filepath, std::move(ctx), inbound_callback);
    break;
  }

  return std::unique_ptr<ITradeClient>(pitradeclient);
}
#else
auto create_client(
  const std::string &exchangeName, const TradeClientType type, const std::string &filepath, FromAppCallbackExecutionReport cb) -> std::unique_ptr<ITradeClient> {
  ITradeClient *pitradeclient = nullptr;
  switch (type) {
  case TradeClientType_Apifiny:
    // pitradeclient =
    //     new TradeClientApifiny(filepath, std::move(ctx), inbound_callback);
    break;
  case TradeClientType_Wintmut:
    break;
  case TradeClientType_CCApi:
     pitradeclient =
         new TradeClientCCApi(exchangeName, filepath, cb);
    break;
  }

  return std::unique_ptr<ITradeClient>(pitradeclient);
}
#endif