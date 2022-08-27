
#include "ITradeclient.h"
#include "tradeclient-apifiny/Tradeclient.h"

#include "quickfix-rs/src/lib.rs.h"

auto create_client(
    const TradingClientType type, const std::string &filepath,
    rust::Box<TradingClientContext> ctx,
    rust::Fn<void(const QuickFixMessage, const rust::Box<TradingClientContext> &)>
        inbound_callback) -> std::unique_ptr<ITradeClient> {
  ITradeClient *pitradeclient = nullptr;
  switch (type) {
  case TradingClientType::Apifiny:
    pitradeclient =
        new TradeClientApifiny(filepath, std::move(ctx), inbound_callback);
    break;
  case TradingClientType::Wintmute:
    break;
  }

  return std::unique_ptr<ITradeClient>(pitradeclient);
}
