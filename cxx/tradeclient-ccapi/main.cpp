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

void fromAppCallbackExecutionReport(std::vector<ExecutionReport> &excutionReportList, std::string sessionId)
{
    for (const auto& excutionReport : excutionReportList) {
        std::cout << "executionReport.OrderID:" << executionReport.OrderID << std::endl;
        std::cout << "executionReport.OrdStatus:" << executionReport.OrdStatus << std::endl;
        std::cout << "sessionId:" << sessionId << std::endl;
    }

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

期权限价申报和市价申报指令：
限价GFD：限价申报，当日有效，可手工撤单；
市价IOC（FAK）：按最优报价最大限度成交，不成部分系统自动撤单；
市价剩余转限价GFD：按市场最优价成交，未成转限价（已成交部分价格）；
限价FOK：限价成交，不成自动撤单；
市价FOK：市价全部成交，否则自动撤单。

// 
市价
./cxx/tradeclient-ccapi/tradeclient-ccapi -e binance -b BTCUSDT --side 1 --quantity 1 --price 0.321   --order_type 1

 限价DAY
 ./cxx/tradeclient-ccapi/tradeclient-ccapi -e binance -b BTCUSDT --side 1 --quantity 1 --price 20000   --time_in_force 0 --order_type 2 //nok
限价GTC
./cxx/tradeclient-ccapi/tradeclient-ccapi -e binance -b BTCUSDT --side 1 --quantity 1 --price 20000   --time_in_force 1 --order_type 2
限价IOC
./cxx/tradeclient-ccapi/tradeclient-ccapi -e binance -b BTCUSDT --side 1 --quantity 1 --price 20000  --time_in_force 3 --order_type 2
限价FOK
./cxx/tradeclient-ccapi/tradeclient-ccapi -e binance -b BTCUSDT --side 1 --quantity 1 --price 20000  --time_in_force 4 --order_type 2
限价FOK


市价FOK

-s side
1 = Buy
2 = Sell
3 = Buy minus
4 = Sell plus
5 = Sell short
6 = Sell short exempt

-t time_in_force
0 = Day (or session)
1 = Good Till Cancel (GTC)
2 = At the Opening (OPG)
3 = Immediate or Cancel (IOC)
4 = Fill or Kill (FOK)
5 = Good Till Crossing (GTX)
6 = Good Till Date
7 = At the Close

 -o order_type
1 = Market
2 = Limit
3 = Stop
4 = Stop limit


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
/*
example:
1. ERROR_MESSAGE = {"code":-1013,"msg":"Filter failure: PRICE_FILTER"},
./cxx/tradeclient-ccapi/tradeclient-ccapi -e binance -b BTCUSDT --side 1 --quantity 1 --price 0.321 --stop_price 0.22 --time_in_force 1 --order_type 4

2. success
./cxx/tradeclient-ccapi/tradeclient-ccapi -e binance -b BTCUSDT --side 1 -q 0.123 -p 10000 --stop_price 31000 -t 1 -o 4
*/
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
    char time_in_force = -1;
    char order_type;
    double price;
    double stop_price;

    CLI::App app{"app: tradeclient-ccapi"};
    CLI::App* subcom = app.add_subcommand("test", "description");
                         
    app.add_option("-e,--exchange", exchangeName, "exchange name")->required();
    // app.add_option("-c,--coin", coinpairs, "coinpairs, -c [BTC-ETH, BTC-USDT]");
    
    app.add_option("-b,--symbol", symbol, "symbol, -s BTCUSDT")->required();
    app.add_option("-s,--side", side, "side, -s buy|sell")->required(); //https://www.onixs.biz/fix-dictionary/4.4/tagNum_54.html
    app.add_option("-q,--quantity", quantity, "quantity, -q 1.22")->required();
    app.add_option("-p,--price", price, "price, -p 1.22");
    app.add_option("--stop_price", stop_price, "stop_price, -p 1.22");
    app.add_option("-t,--time_in_force", time_in_force, "time_in_force, -t 0"); //https://www.onixs.biz/fix-dictionary/4.4/tagNum_59.html //https://www.onixs.biz/fix-dictionary/5.0.SP2/tagNum_59.html
    app.add_option("-o,--order_type", order_type, "order_type, -o 1.22")->required(); //https://www.onixs.biz/fix-dictionary/4.4/tagNum_40.html
    

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
                           exchangeName.c_str(), symbol.c_str(), side, quantity, price, order_type, time_in_force);
        auto client = create_client(TradeClientType_CCApi, configfile, fromAppCallbackExecutionReport);
        client->start();
//        client->put_order(symbol, side,
//                                 quantity,
//                                 price, stop_price,
//                                 order_type,
//                                 time_in_force);
        std::string cl_order_id = "order_xxx";
        NewOrderSingle aNewOrderSingle = NewOrderSingle(symbol, cl_order_id, side,
                                                        order_type, quantity, price, stop_price,
                                                        time_in_force);
        client->put_order(aNewOrderSingle);

        while(true)
        {
            if (stoped)
            {
            break;
            }
        }
        client->stop();

        client.release();
        //delete clientApifiny;

        return 0;
    }
    catch ( std::exception & e )
    {
        std::cout << e.what();
        return 1;
    }
}


