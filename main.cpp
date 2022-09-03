#include "request_handler.h"

#include <iostream> 

using namespace std;

int main()
{
    setlocale(LC_ALL, "Russian");
    setlocale(LC_NUMERIC, "English");

    transport_catalogue::TransportCatalogue tc;

    transport_catalogue::request_handler::RequestHandlerJSON rhj(tc, std::cin, std::cout);
  
    return 0;
}