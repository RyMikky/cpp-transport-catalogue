#include "request_handler.h"

#include <iostream> 

using namespace std;

int main()
{
    setlocale(LC_ALL, "Russian");
    setlocale(LC_NUMERIC, "English");

    //{

    //    json::Print(
    //        json::Document{
    //            json::Builder{}
    //            .StartDict()
    //                .Key("key1"s).Value(123)
    //                .Key("key2"s).Value("value2"s)
    //                .Key("key3"s).StartArray()
    //                    .Value(456)
    //                    .StartDict().EndDict()
    //                    .StartDict()
    //                        .Key(""s)
    //                        .Value(nullptr)
    //                    .EndDict()
    //                    .Value(""s)
    //                .EndArray()
    //            .EndDict()
    //            .Build()
    //        },
    //        cout
    //    );
    //    cout << endl;

    //    json::Print(
    //        json::Document{
    //            json::Builder{}
    //            .Value("just a string"s)
    //            .Build()
    //        },
    //        cout
    //    );
    //    cout << endl;

    //}

    transport_catalogue::TransportCatalogue tc;

    transport_catalogue::request_handler::RequestHandlerJSON rhj(tc, std::cin, std::cout);
  
    return 0;
}