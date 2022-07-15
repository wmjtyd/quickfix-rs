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
#pragma warning( disable : 4503 4355 4786 )
#endif

#include "quickfix/config.h"

#include "quickfix/FileStore.h"
#include "quickfix/SocketInitiator.h"
#ifdef HAVE_SSL
#include "quickfix/ThreadedSSLSocketInitiator.h"
#include "quickfix/SSLSocketInitiator.h"
#endif
#include "quickfix/SessionSettings.h"
#include "quickfix/Log.h"
#include "Application.h"
#include <string>
#include <iostream>
#include <fstream>

#include "../../src/getopt-repl.h"

#include "tradeclient.h"


TradeClientApifiny::TradeClientApifiny(const std::string &filepath, const FromAppCallback fromAppCallback)
        : application(fromAppCallback), settings(filepath),
          store_factory(settings), log_factory(settings),
          initiator(new FIX::SocketInitiator(application, store_factory, settings,
                                             log_factory)) {}

TradeClientApifiny::~TradeClientApifiny() {
    delete this->initiator;
    this->initiator = nullptr;
}

auto TradeClientApifiny::start() const -> void {
    try {
        this->initiator->start();
    } catch (std::exception &e) {
        std::cout << e.what() << std::endl;
    }
}

auto TradeClientApifiny::stop() const -> void { this->initiator->stop(); }

auto TradeClientApifiny::put_order(const std::string &order_id,
                            const std::string &quoteid,
                            const std::string &symbol,
                            const std::string &currency, const int side,
                            const int quantity, const int price,
                            const int time_in_force) const -> void {
    this->application.new_order_single(order_id, symbol, side, quantity, price,
                                       time_in_force);
}

ITradeClient* create_client(int type, const std::string &filepath, const FromAppCallback fromAppCallback) {
    ITradeClient* pitradeclient = nullptr;
    if (TradeClient_Apifiny == type)
    {
        pitradeclient = new TradeClientApifiny(filepath, fromAppCallback);
    }
    else if (TradeClient_Wintmut == type)
    {

    } else {
        return nullptr;
    }

    return pitradeclient;
}
