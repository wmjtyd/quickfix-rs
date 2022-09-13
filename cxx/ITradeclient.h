#pragma once

#include "quickfix/SessionID.h"

#ifdef USE_TRADECLIENT_RUST_INTERFACE
#include "rust/cxx.h"
#endif

#include "common.h"

#include <memory>

enum class FixMessageType : uint8_t;
enum class TradingClientType : uint8_t;

struct QuickFixMessage;
struct TradingClientContext;

enum TradeClientType {
  TradeClientType_Apifiny = 1,
  TradeClientType_Wintmut = 2,
  TradeClientType_CCApi = 3,
};

class ITradeClient {
public:
  //    TradeClient(const std::string &filepath) {};
  //    ~TradeClient() ;

  virtual auto start() const -> void = 0;
  virtual auto stop() const -> void = 0;

  virtual auto put_order(const std::string &symbol, const char side,
                         const double quantity, const double price, const double stop_price,
                         const char order_type,
                         const char time_in_force) const
      -> std::unique_ptr<std::string> = 0;

  virtual auto put_order(const NewOrderSingle &aNewOrderSingle) const -> std::unique_ptr<std::string> = 0;

  virtual auto cancel_order(const std::string &order_id) const -> void = 0;
};

#ifdef USE_TRADECLIENT_RUST_INTERFACE
auto create_client(
    const TradingClientType type, const std::string &filepath,
    rust::Box<TradingClientContext> ctx,
    rust::Fn<void(const QuickFixMessage, const rust::Box<TradingClientContext> &)>
        inbound_callback) -> std::unique_ptr<ITradeClient>;
#else
auto create_client(
    const TradeClientType type, const std::string &filepath, FromAppCallback cb) -> std::unique_ptr<ITradeClient>;
#endif        