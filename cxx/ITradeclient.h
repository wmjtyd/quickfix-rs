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
#ifdef USE_TRADECLIENT_RUST_INTERFACE
  #ifndef CXXBRIDGE1_STRUCT_RustExecutionReport
  #define CXXBRIDGE1_STRUCT_RustExecutionReport
struct RustExecutionReport final {
  char exec_type;
  ::rust::String symbol;
  ::rust::String order_id;
  ::rust::String client_ord_id;
  ::rust::String exec_id;
  double cum_qty;
  double order_qty;
  char ord_status;
  char side;
  double asset_free;
  double asset_locked;
  ::rust::String error_message;

  using IsRelocatable = ::std::true_type;
};
  #endif // CXXBRIDGE1_STRUCT_RustExecutionReport
#endif  

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

  // 存在问题：这里存在的问题是，无法初始化结构体
  virtual auto put_order(const NewOrderSingle &aNewOrderSingle) const -> std::unique_ptr<std::string> = 0;

  virtual auto cancel_order(const std::string &symbol, const std::string &order_id) const -> void = 0;

  // 存在问题：这里 rust 无法初始化结构体
  virtual auto cancel_order(const OrderCancelRequest &aOrderCancelRequest) const -> void = 0;

  virtual auto cancel_open_orders(const std::string &symbol) const -> void = 0;

  virtual auto get_order(const std::string &symbol, const std::string &order_id) const -> void = 0;
  virtual auto get_open_orders(const std::string &symbol) const -> void = 0;

  virtual auto get_recent_trades(const std::string &symbol) const -> void = 0;

  virtual auto get_account_balances() const -> void = 0;

  virtual auto get_accounts() const -> void = 0;
  virtual auto get_account_postions() const -> void = 0;  

  virtual auto subscribe(
    const std::string &symbol, 
    const std::vector<std::string> eventNames, 
    std::string correlationId = ""
  ) const -> void = 0;
  
};

#ifdef USE_TRADECLIENT_RUST_INTERFACE
auto create_client(
    const std::string &exchangeName, const TradingClientType type, const std::string &filepath,
    rust::Box<TradingClientContext> ctx,
    rust::Fn<void(const RustExecutionReport, const rust::Box<TradingClientContext> &)>
        inbound_callback) -> std::unique_ptr<ITradeClient>;
#else
auto create_client(
    const std::string &exchangeName, const TradeClientType type, const std::string &filepath,
    FromAppCallbackExecutionReport cb) -> std::unique_ptr<ITradeClient>;
#endif        