#include <string>
#include <iostream>
#include <fstream>
#include <csignal>

#include "CLI/App.hpp"
#include "CLI/Formatter.hpp"
#include "CLI/Config.hpp"

#include "ITradeclient.h"

void fromAppCallback(std::string message, std::string sessionId)
{
    std::cout << "message:" << message << std::endl;
    std::cout << "sessionId:" << sessionId << std::endl;
}

/*

binance api doc: https://binance-docs.github.io/apidocs/spot/en/#new-order-trade


限价GFD，
限价FDK，
市价剩转限价GFD，
市价FOK，
市价IOC -o 1 -t 3
期权限价申报和市价申报指令：
限价GFD：限价申报，当日有效，可手工撤单；版
市价IOC（权FAK）：按最优报价最大限度成交，不成部分系统自动撤单；
市价剩余转限价GFD：按市场最优价成交，未成转限价（已成交部分价格）；
限价FOK：限价成交，不成自动撤单；
市价FOK：市价全部成交，否则自动撤单。
***************************************






-s side
1 = Buy
2 = Sell
3 = Buy minus
4 = Sell plus
5 = Sell short
6 = Sell short exempt

-s 2 -o 4 -t 1 -p 20000 -P
  request.appendParam({
      {"side", "SELL"},
      {"type", "STOP_LOSS_LIMIT"},
      {"quantity", "0.0005"},
      {"stopPrice", "20001"},
      {"price", "20000"},
      {"timeInForce", "GTC"},
  });
*/

bool stoped = false;
//
void signal_handler(int signal)
{
  std::cout << "signal_handler:" << signal << std::endl;
  if (signal == SIGINT || signal == SIGKILL)
  {
    stoped = true;
  }
}

int main( int argc, char** argv )
{
    std::signal(SIGINT, signal_handler);
    std::signal(SIGKILL, signal_handler);


    std::vector<std::string> coinpairs;
    std::string exchangeName;

    std::string symbol;
    char side;
    std::string configfile = "";
    int interval = 60;
    double quantity = 0.0;
    char time_in_force;
    char order_type;
    double price;
    double stop_price;

    CLI::App app{"app: tradeclient-ccapi"};
    CLI::App* subcom = app.add_subcommand("test", "description");
                         
    app.add_option("-e,--exchane", exchangeName, "exchane name");
    // app.add_option("-c,--coin", coinpairs, "coinpairs, -c [BTC-ETH, BTC-USDT]");
    
    app.add_option("-b,--symbol", symbol, "symbol, -s BTCUSDT");
    app.add_option("-s,--side", side, "side, -s buy|sell"); //https://www.onixs.biz/fix-dictionary/4.4/tagNum_54.html
    app.add_option("-q,--quantity", quantity, "quantity, -q 1.22");
    app.add_option("-p,--price", price, "price, -p 1.22");
    app.add_option("--stop_price", stop_price, "stop_price, -p 1.22");
    app.add_option("-t,--time_in_force", time_in_force, "time_in_force, -q 1.22"); //https://www.onixs.biz/fix-dictionary/4.4/tagNum_59.html
    app.add_option("-o,--order_type", order_type, "order_type, -o 1.22"); //https://www.onixs.biz/fix-dictionary/4.4/tagNum_40.html
    

    app.add_option("-f,--configfile", configfile, "-f ./configfile");

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

        printf("put_order: exchange(%s) symbol(%s) side(%d) quantity(%f) price(%f) order_type(%d) time_in_force(%d)\n", 
                           exchangeName, symbol, side, quantity, price, order_type, time_in_force);
        auto clientApifiny = create_client(TradeClientType_CCApi, configfile, fromAppCallback);
        clientApifiny->start();
        clientApifiny->put_order(symbol, side,
                                 quantity, 
                                 price, stop_price,
                                 order_type,
                                 time_in_force);
        while(true)
        {
            if (stoped)
            {
            break;
            }
        }                                 
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


