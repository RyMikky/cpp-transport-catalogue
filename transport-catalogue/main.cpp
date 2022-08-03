#include "input_reader.h"

#include <iostream> 

using namespace std;

int main()
{
    transport_catalogue::TransportCatalogue tc;
    transport_catalogue::input_reader::RequestProcessorSimple(tc, std::cin, std::cout);

    return 0;
}