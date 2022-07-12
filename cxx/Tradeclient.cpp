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

#include "quickfix/SessionSettings.h"
#include "../vendor/quickfix-cpp/src/getopt-repl.h"

#include <fstream>
#include <iostream>
#include <string>

TradeClient::TradeClient(const std::string &filepath)
    : application(), settings(filepath),
      store_factory(settings), log_factory(settings),
      initiator(new FIX::SocketInitiator(application, store_factory, settings,
                                         log_factory)) {}

TradeClient::~TradeClient() {
  delete this->initiator;
  this->initiator = nullptr;
}

bool TradeClient::run() {
  try {
    this->initiator->start();
    this->application.run();
    this->initiator->stop();

    return false;
  } catch (std::exception &e) {
    std::cout << e.what() << std::endl;

    return true;
  }
}

void TradeClient::put_order(const std::string &quoteid,
                            const std::string &symbol,
                            const std::string &currency, int side, int quantity,
                            int price, int time_in_force) {
  return;
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
