/////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                             //
//        class JsonReader                                                                     //
//        Несамостоятельныый модуль-класс - базовый I/O класс программы                        //
//        Основной функционал - получение, парсинг и обработка JSON запросов                   //
//        Для данной реализации является точкой входа программы                                //
//        Требования и зависимости:                                                            //
//           1. request_handler.h - обработчик запросов к базе, роутеру и рендеру              //
//           2. json_builder.h - конструктор и библиотека работы с JSON объектами              //
//           3. C++17 (STL)                                                                    //
//                                                                                             // 
//        class JsonPrinter                                                                    //
//        Несамостоятельныый модуль-класс - базовый I/O класс                                  //
//        Основной функционал - вывод информации в заданый поток                               //
//        Требования и зависимости:                                                            //
//           1. json_builder.h - конструктор и библиотека работы с JSON объектами              //
//           2. C++17 (STL)                                                                    //
//                                                                                             // 
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "json_builder.h"
#include "request_handler.h"

#include <map>
#include <deque>
#include <sstream>
#include <cassert>
#include <optional>
#include <fstream>


using namespace std::literals;

namespace transport_catalogue {

    namespace json_reader {

        namespace detail {

            json::Document LoadJson(const std::string&);                                           // загрузка JSON из переданной строки
            json::Document LoadJson(std::istream&);                                                // загрузка JSON из переданного потока

            std::string PrintJson(const json::Node&);                                              // печать Node и вывод в std::string
            std::ostream& operator<<(std::ostream&, const json::Node&);                            // прямой вывод Node в поток

        } // namespace detail

        // блок вывода информации
        class JsonPrinter {
        public:

            void AddDocument(const json::Node& node);                                               // добавление документа в принтер

            json::Document RouteReportNodeMaker(size_t, transport_catalogue::RouteStatPtr);         // получение json-результата по маршрутам
            json::Document StopReportNodeMaker(size_t, transport_catalogue::StopStatPtr);           // получение json-результата по остановкам
            json::Document TransportRouterNodeMaker(size_t, transport_catalogue::RouterData);       // получение json-результата по роутеру
            json::Document MapRendererNodeMaker(size_t, transport_catalogue::RendererData);         // получение json-результата по рендеру

            JsonPrinter& PrintResult(std::ostream&);                                                // вывод принтера
            JsonPrinter& ClearPrinterBase();                                                        // очистить базу принтера результатов

        private:
            json::Array _json_base = {};
        };

        enum ProgramMode {
            test_mode,                      // режим работы в режиме ожидания и вызова тестов
            simple_mode,                    // режим работы с разовым простым блоком запросов. Спринт 12. Итоговое
            make_base_mode,                 // режим работы с сериализацией данных и блочной обработкой. Спринт 14. Итоговое 3.
            make_base_mode_vo_router,       // режим работы с сериализацией данных и блочной обработкой. Спринт 14. Итоговое 1.
            process_requests_mode           // режим работы с сериализацией данных и блочной обработкой. Спринт 14. Итоговое 1.
        };

        class JsonReader {

        private:

            std::istream& _input;
            std::ostream& _output;
            transport_catalogue::json_reader::JsonPrinter _printer;                    // собственный принтер для вывода результатов
            transport_catalogue::request_handler::RequestHandler _handler;             // обработчик запросов к базе данных транспортного каталога

        public:
            JsonReader(std::istream&, std::ostream&);                                  // запускает базовый режим работы
            JsonReader(std::istream&, std::ostream&, ProgramMode);                     // запускает режим по переданному аргументу

            JsonReader& test_mode();                                    // запуск работы в режиме ожидания и вызова тестов
            JsonReader& single_block_mode();                            // запуск работы в режиме обработки одного блока. Спринт 12. Итоговое
            JsonReader& make_base_mode();                               // запуск работы в режиме обработки make_base. Спринт 14. Итоговое 3.
            JsonReader& make_base_mode_vo_router();                     // запуск работы в режиме обработки make_base без роутера. Спринт 14. Итоговое 1.
            JsonReader& process_requests_mode();                        // запуск работы в режиме обработки process_requests. Спринт 14. Итоговое 1.

            JsonReader& reset_all_modules();                            // сбросить состояние всех компонентов программы, применяется для блока тестирования

        private:
            std::deque<json::Dict> _json_dicts;                                                      // история полученных JSON-блоков
            std::deque<transport_catalogue::JsonRequest> _json_requests_history;                     // история поступивших запросов JSON-формата
            // мап текущих к обработке запросов JSON-формата
            std::map<transport_catalogue::RequestType, std::deque<transport_catalogue::JsonRequestPtr>> _current_json_requests = {};

            json::Node LoadJson(const std::string&);                                                 // загрузка JSON из переданной строки
            json::Node LoadJson(std::istream&);                                                      // загрузка JSON из переданного потока

            // ---------------------------------------- парсеры запросов -------------------------------------------------------------------------------

            std::string FileNameParser(const json::Dict&);                                           // парсинг имени файла для сериализации

            void AddRouteParser(transport_catalogue::JsonRequestPtr, const json::Dict&);             // парсинг запроса на добавление маршрута
            void AddStopParser(transport_catalogue::JsonRequestPtr, const json::Dict&);              // парсинг запроса на добавление остановки

            void BaseRequestParser(transport_catalogue::JsonRequestPtr, const json::Dict&);          // парсинг запросов на добавление информации
            void StatRequestParser(transport_catalogue::JsonRequestPtr, const json::Dict&);          // парсинг запроса на получение информации

            void RendererSetupParser(transport_catalogue::map_renderer::RendererSettings&, const json::Dict&);       // парсинг настрек рендера карт
            void RouterSetupParser(transport_catalogue::router::RouterSettings&, const json::Dict&);                 // парсинг настроек роутера маршрутов

            // ---------------------------------------- блок процессинга -------------------------------------------------------------------------------

            JsonReader& BaseRequestProcessor(const json::Array&);                                    // обработка блоков загрузки информации
            JsonReader& RendererSetupProcessor(const json::Dict&);                                   // обработка блока загрузки настройки рендера
            JsonReader& RouterSetupProcessor(const json::Dict&);                                     // обработка блока загрузки настройки роутера
            JsonReader& StatsRequestProcessor(const json::Array&);                                   // обработка блоков получения информации
            JsonReader& PrintResultProcessor(std::ostream&);                                         // печать информации после обработки вывода

            // ---------------------------------------- базовый сплиттер команд из JSON ----------------------------------------------------------------

            JsonReader& SimpleModeSplitter(const json::Dict&);                                       // сплиттер блоков запросов базового режима

            // ---------------------------------------- сплитеры режима сериализации данных ------------------------------------------------------------

            JsonReader& MakeBaseModeSplitter(const json::Dict&);                                     // сплиттер блоков запросов на создание базы
            JsonReader& MakeBaseModeVORSplitter(const json::Dict&);                                  // сплиттер блоков запросов на создание базы без данных роутера
            JsonReader& RequestProcessModeSplitter(const json::Dict&);                               // сплиттер блоков запросов на получение информации

            std::optional<json::Dict> JsonBlockReading();                                            // чтение JSON блока из потока

        };

    } // namespace json_reader

} //namespace transport_catalogue 
