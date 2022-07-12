/****************************************************************************
** Copyright (c) 2001-2014
**
** This file is part of the QuickFIX FIX Engine
**
** This file may be distributed under the terms of the quickfixengine.org
** license as defined by quickfixengine.org and appearing in the file
** LICENSE included in the packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.quickfixengine.org/LICENSE for licensing information.
**
** Contact ask@quickfixengine.org if any conditions of this licensing are
** not clear to you.
**
****************************************************************************/

#ifdef _MSC_VER
#pragma warning(disable : 4503 4355 4786)
#endif

#include "include/Tradeclient.h"

#include "../vendor/quickfix-cpp/src/getopt-repl.h"
#include "quickfix/SessionSettings.h"

#include <fstream>
#include <iostream>
#include <string>

TradeClient::TradeClient(const std::string &filepath)
    : application(), settings(filepath), store_factory(settings),
      log_factory(settings),
      initiator(new FIX::SocketInitiator(application, store_factory, settings,
                                         log_factory)) {}

TradeClient::~TradeClient() {
  delete this->initiator;
  this->initiator = nullptr;
}

auto TradeClient::start() const -> void {
  try {
    this->initiator->start();
  } catch (std::exception &e) {
    std::cout << e.what() << std::endl;
  }
}

auto TradeClient::stop() const -> void { this->initiator->stop(); }

auto TradeClient::put_order(const std::string &order_id,
                            const std::string &quoteid,
                            const std::string &symbol,
                            const std::string &currency, const int side,
                            const int quantity, const int price,
                            const int time_in_force) const -> void {
  this->application.new_order_single(order_id, symbol, side, quantity, price,
                                     time_in_force);
}

auto create_client(const std::string &filepath)
    -> std::unique_ptr<TradeClient> {
  TradeClient *trade_client = nullptr;
  try {
    trade_client = new TradeClient(filepath);
  } catch (std::exception &e) {
    std::cout << e.what() << std::endl;
  }

  return std::unique_ptr<TradeClient>(trade_client);
}
