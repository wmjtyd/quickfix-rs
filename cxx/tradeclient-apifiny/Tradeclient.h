#pragma once

#include "Application.h"
#include "ITradeclient.h"

#include "quickfix/FileStore.h"
#include "quickfix/Log.h"
#include "quickfix/SocketInitiator.h"
#include "quickfix/config.h"
#include "rust/cxx.h"

#include <iostream>
#include <memory>

struct TradeClientContext;

class TradeClientApifiny : public ITradeClient {
public:
  TradeClientApifiny(
      const std::string &filepath, rust::Box<TradeClientContext> ctx,
      rust::Fn<void(const QuickFixMessage, const FIX::SessionID &,
                    const rust::Box<TradeClientContext> &)>
          inbound_callback);
  ~TradeClientApifiny();

  auto start() const -> void;
  auto stop() const -> void;

  auto put_order(const std::string &quoteid, const std::string &symbol,
                 const std::string &currency, const uint32_t side,
                 const uint32_t quantity, const uint32_t price,
                 const uint32_t time_in_force) const -> std::unique_ptr<std::string>;

private:
  Application application;

  FIX::SessionSettings settings;
  FIX::FileStoreFactory store_factory;
  FIX::ScreenLogFactory log_factory;
  FIX::Initiator *initiator;
};
