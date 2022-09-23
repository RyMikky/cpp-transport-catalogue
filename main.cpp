#include "json_reader.h"
#include "simple_reader.h"
#include <iostream> 

using namespace std;

int main()
{
    setlocale(LC_ALL, "Russian");
    setlocale(LC_NUMERIC, "English");

    transport_catalogue::json_reader::JsonReader(std::cin, std::cout).single_block_process();
  
}