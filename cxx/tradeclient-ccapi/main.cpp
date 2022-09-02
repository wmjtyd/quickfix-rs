#include <string>
#include <iostream>
#include <fstream>

#include "CLI/App.hpp"
#include "CLI/Formatter.hpp"
#include "CLI/Config.hpp"

#include "ITradeclient.h"

void fromAppCallback(std::string message, std::string sessionId)
{
    std::cout << "message:" << message << std::endl;
    std::cout << "sessionId:" << sessionId << std::endl;
}

int main( int argc, char** argv )
{
    std::vector<std::string> coinpairs;
    std::string exchangeName;

    std::string symbol;
    char side;
    std::string configfile = "";
    int interval = 60;
    double quantity = 0.0;
    char time_in_force;
    double price;

    CLI::App app{"app: tradeclient-ccapi"};
    CLI::App* subcom = app.add_subcommand("test", "description");
                         
    app.add_option("-e,--exchane", exchangeName, "exchane name");
    // app.add_option("-c,--coin", coinpairs, "coinpairs, -c [BTC-ETH, BTC-USDT]");
    
    app.add_option("-s,--symbol", symbol, "symbol, -s BTCUSDT");
    app.add_option("-s,--side", side, "side, -s buy|sell");
    app.add_option("-q,--quantity", quantity, "quantity, -q 1.22");
    app.add_option("-p,--price", price, "price, -p 1.22");
    app.add_option("-t,--time_in_force", time_in_force, "time_in_force, -q 1.22");
    

    app.add_option("-q,--configfile", configfile, "-f ./configfile");

    CLI11_PARSE(app, argc, argv);
    // if ( argc < 2 )
    // {
    //     std::cout << "usage: " << argv[ 0 ]
    //               << " FILE." << std::endl;
    //     return 0;
    // }
    // std::string file = argv[ 1 ];

    try
    {

        auto clientApifiny = create_client(TradeClientType_CCApi, configfile, fromAppCallback);
        clientApifiny->start();
        clientApifiny->put_order(symbol, side,
                                 quantity, price,
                                 time_in_force);
        clientApifiny->stop();

        clientApifiny.release();
        //delete clientApifiny;

        return 0;
    }
    catch ( std::exception & e )
    {
        std::cout << e.what();
        return 1;
    }
}

