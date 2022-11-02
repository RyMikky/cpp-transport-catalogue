/////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                             //
//        main() - функция программы                                                           //
//        Ключи запуска:                                                                       //
//           1. "make_base" - создание базы данных каталога и запись в файл                    //
//           2. "make_base_vor" - создание базы данных без инициализации роутера               //
//           3. "process_requests" - загрузка данных из внешней базы и получение результатов   //
//           4. "simple_mode" - стандартный режим работы с единым JSON-блоком                  //
//           5. "test_mode" - запуск тестирования всех четырех режимов                         //
//              "process_requests" тестируется автоматически в режимах создания базы           //
//                                                                                             //
//        Требования и зависимости:                                                            //
//           1. json_reader.h - подключение к основной программе каталога                      //
//           2. test_system.h - модуль выполнения тестирования корректности вывода             //
//           3. C++17 (STL)                                                                    //
//                                                                                             //
/////////////////////////////////////////////////////////////////////////////////////////////////

#include <fstream>
#include <iostream>
#include <string_view>

#include "json_reader.h"
#include "test_system.h"

using namespace std::literals;

void PrintUsage(std::ostream& stream = std::cerr) {
    stream << "Usage: transport_catalogue [make_base|process_requests]\n"sv;
}

int main(int argc, char* argv[]) {

    setlocale(LC_ALL, "Russian");
    setlocale(LC_NUMERIC, "English");

    if (argc != 2) {
        PrintUsage();
        return 1;
    }

    const std::string_view mode(argv[1]);

    if (mode == "make_base"sv) {
        transport_catalogue::json_reader::JsonReader(std::cin, std::cout, transport_catalogue::json_reader::make_base_mode);
    }
    else if (mode == "make_base_vor"sv) {
        transport_catalogue::json_reader::JsonReader(std::cin, std::cout, transport_catalogue::json_reader::make_base_mode_vo_router);
    }
    else if (mode == "process_requests"sv) {
        transport_catalogue::json_reader::JsonReader(std::cin, std::cout, transport_catalogue::json_reader::process_requests_mode);
    }
    else if (mode == "simple_mode"sv) {
        transport_catalogue::json_reader::JsonReader(std::cin, std::cout, transport_catalogue::json_reader::simple_mode);
    }
    else if (mode == "test_mode"sv) {
        transport_catalogue::test_system::SimpleModeTest().RunAllTests();
        transport_catalogue::test_system::MakeBaseTest().RunAllTests();
        transport_catalogue::test_system::MakeBaseVORTest().RunAllTests();
    }
    else {
        PrintUsage();
        return 1;
    }

    

}