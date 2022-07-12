#ifndef TRADECLIENT_H
#define TRADECLIENT_H

#include "include/Application.h"

#include "quickfix/FileStore.h"
#include "quickfix/Log.h"
#include "quickfix/SocketInitiator.h"
#include "quickfix/config.h"
#include "rust/cxx.h"

#include <iostream>
#include <memory>

struct QuickFixMessage;
struct ApplicationContext;

class TradeClient {
public:
  TradeClient(const std::string &, rust::Box<ApplicationContext>,
              rust::Fn<void(const QuickFixMessage, const FIX::SessionID &,
                            const rust::Box<ApplicationContext> &)>);
  ~TradeClient();

  auto start() const -> void;
  auto stop() const -> void;

  auto put_order(const std::string &_id, const std::string &,
                 const std::string &, const std::string &, const uint32_t,
                 const uint32_t, const uint32_t, const uint32_t) const -> void;

private:
  Application application;

  const FIX::SessionSettings settings;
  FIX::FileStoreFactory store_factory;
  FIX::ScreenLogFactory log_factory;
  FIX::Initiator *initiator;
};

auto create_client(const std::string &, rust::Box<ApplicationContext>,
                   rust::Fn<void(const QuickFixMessage, const FIX::SessionID &,
                                 const rust::Box<ApplicationContext> &)>)
    -> std::unique_ptr<TradeClient>;

#endif
