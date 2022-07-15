#ifndef TRADECLIENT_H
#define TRADECLIENT_H

#include "Application.h"

#include "quickfix/FileStore.h"
#include "quickfix/Log.h"
#include "quickfix/SocketInitiator.h"
#include "quickfix/config.h"
//#include "rust/cxx.h"

#include <iostream>
#include <memory>
#include "itradeclient.h"
#include "common.h"
struct ApplicationContext;

class TradeClientApifiny : public ITradeClient {
public:
    TradeClientApifiny(const std::string &filepath, const FromAppCallback fromAppCallback);
  ~TradeClientApifiny();

  auto start() const -> void;
  auto stop() const -> void;

  auto put_order(const std::string &order_id, const std::string &quoteid,
                 const std::string &symbol, const std::string &currency,
                 const int side, const int quantity, const int price,
                 const int time_in_force) const -> void;

private:
  Application application;

  FIX::SessionSettings settings;
  FIX::FileStoreFactory store_factory;
  FIX::ScreenLogFactory log_factory;
  FIX::Initiator *initiator;
};



#endif
