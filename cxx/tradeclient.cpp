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

#include "../vendor/quickfix-cpp/include/quickfix/config.h"

#include "../vendor/quickfix-cpp/include/quickfix/FileStore.h"
#include "../vendor/quickfix-cpp/include/quickfix/SocketInitiator.h"
#ifdef HAVE_SSL
#include "../vendor/quickfix-cpp/include/quickfix/SSLSocketInitiator.h"
#include "../vendor/quickfix-cpp/include/quickfix/ThreadedSSLSocketInitiator.h"
#endif
#include "../vendor/quickfix-cpp/include/quickfix/Log.h"
#include "../vendor/quickfix-cpp/include/quickfix/SessionSettings.h"
#include "include/application.h"
#include <fstream>
#include <iostream>
#include <string>

#include "../vendor/quickfix-cpp/src/getopt-repl.h"

#include "../vendor/quickfix-cpp/include/quickfix/Log.h"
#include "../vendor/quickfix-cpp/include/quickfix/SessionSettings.h"

#include <fstream>
#include <iostream>
#include <string>

#include "include/tradeclient.h"

int main(int argc, char **argv) {
  if (argc < 2) {
    std::cout << "usage: " << argv[0] << " FILE." << std::endl;
    return 0;
  }
  std::string file = argv[1];

#ifdef HAVE_SSL
  std::string isSSL;
  if (argc > 2) {
    isSSL.assign(argv[2]);
  }
#endif

  FIX::Initiator *initiator = 0;
  try {
    FIX::SessionSettings settings(file);

    Application application;
    FIX::FileStoreFactory storeFactory(settings);
    FIX::ScreenLogFactory logFactory(settings);
#ifdef HAVE_SSL
    if (isSSL.compare("SSL") == 0)
      initiator = new FIX::ThreadedSSLSocketInitiator(application, storeFactory,
                                                      settings, logFactory);
    else if (isSSL.compare("SSL-ST") == 0)
      initiator = new FIX::SSLSocketInitiator(application, storeFactory,
                                              settings, logFactory);
    else
#endif
      initiator = new FIX::SocketInitiator(application, storeFactory, settings,
                                           logFactory);

    initiator->start();
    application.run();
    initiator->stop();
    delete initiator;

    return 0;
  } catch (std::exception &e) {
    std::cout << e.what();
    delete initiator;
    return 1;
  }
}

// filepath = filepath of fix42.xml, such as filepath = ./fix42.xml
TradeClient *TradeClient::create_client(const std::string &filepath) {
  std::string file = filepath;

#ifdef HAVE_SSL
  std::string isSSL;
  if (argc > 2) {
    isSSL.assign(argv[2]);
  }
#endif

  FIX::Initiator *initiator = 0;
  try {
    FIX::SessionSettings settings(file);

    Application application;
    FIX::FileStoreFactory storeFactory(settings);
    FIX::ScreenLogFactory logFactory(settings);
#ifdef HAVE_SSL
    if (isSSL.compare("SSL") == 0)
      initiator = new FIX::ThreadedSSLSocketInitiator(application, storeFactory,
                                                      settings, logFactory);
    else if (isSSL.compare("SSL-ST") == 0)
      initiator = new FIX::SSLSocketInitiator(application, storeFactory,
                                              settings, logFactory);
    else
#endif
      initiator = new FIX::SocketInitiator(application, storeFactory, settings,
                                           logFactory);

    //        initiator->start();
    //        application.run();
    //        initiator->stop();
    //        delete initiator;

    auto client = new (TradeClient);
    client->mApplication = &application;
    client->mInitiator = initiator;
    return client;
  } catch (std::exception &e) {
    std::cout << e.what();
    delete initiator;
    return NULL;
  }
}

int TradeClient::run() {
  try {

    mInitiator->start();
    mApplication->run();
    mInitiator->stop();
    delete mInitiator;
    mInitiator = NULL;

    return 0;
  } catch (std::exception &e) {
    std::cout << e.what();
    delete mInitiator;
    mInitiator = NULL;
    return 1;
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
  TradeClient trade_client;
  return std::unique_ptr<TradeClient>(trade_client.create_client(filepath));
}
