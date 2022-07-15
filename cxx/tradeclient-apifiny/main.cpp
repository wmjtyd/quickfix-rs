#include <string>
#include <iostream>
#include <fstream>

#include "ITradeclient.h"

void fromAppCallback(std::string message, std::string sessionId)
{
    std::cout << "message:" << message << std::endl;
    std::cout << "sessionId:" << sessionId << std::endl;
}

int main( int argc, char** argv )
{
    if ( argc < 2 )
    {
        std::cout << "usage: " << argv[ 0 ]
                  << " FILE." << std::endl;
        return 0;
    }
    std::string file = argv[ 1 ];

    try
    {

        auto clientApifiny = create_client(TradeClient_Apifiny, file, fromAppCallback);
        clientApifiny->start();
//        clientApifiny->put_order();
        clientApifiny->stop();


        delete clientApifiny;

        return 0;
    }
    catch ( std::exception & e )
    {
        std::cout << e.what();
        return 1;
    }
}

