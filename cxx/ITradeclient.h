#pragma once

#include "quickfix/SessionID.h"
#include "rust/cxx.h"

#include <memory>

enum class FixMessageType : uint8_t;

struct QuickFixMessage;
struct TradeClientContext;

const uint32_t TradeClient_Apifiny = 1;
const uint32_t TradeClient_Wuint32_tmut = 2;

class ITradeClient {
public:
  //    TradeClient(const std::string &filepath) {};
  //    ~TradeClient() ;

  virtual auto start() const -> void = 0;
  virtual auto stop() const -> void = 0;

  virtual auto put_order(const std::string &quoteid, const std::string &symbol,
                         const std::string &currency, const char side,
                         const uint32_t quantity, const uint32_t price,
                         const char time_in_force) const
      -> std::unique_ptr<std::string> = 0;
};

auto create_client(uint32_t type, const std::string &filepath,
                   rust::Box<TradeClientContext> app_ctx,
                   rust::Fn<void(const QuickFixMessage, const FIX::SessionID &,
                                 const rust::Box<TradeClientContext> &)>
                       inbound_callback) -> std::unique_ptr<ITradeClient>;
