#pragma once
#include "json.h"
#include "domain.h"

#include <sstream>
#include <cassert>

using namespace std::literals;

namespace json_reader {

    namespace detail {

        json::Document LoadJSON(const std::string&);                                           // загрузка JSON из переданной строки
        json::Document LoadJSON(std::istream&);                                                // загрузка JSON из переданного потока

        std::string PrintJSON(const json::Node&);                                              // печать Node и вывод в std::string
        std::ostream& operator<<(std::ostream&, const json::Node&);                            // прямой вывод Node в поток

        json::Document RouteReportNodeMaker(size_t, transport_catalogue::RouteStatPtr);        // получение json-результата по маршрутам
        json::Document StopReportNodeMaker(size_t, transport_catalogue::StopStatPtr);          // получение json-результата по остановкам
        json::Document MapRenderNodeMaker(size_t, std::stringstream&);                         // получение json-результата по рендеру

    } // namespace detail

    void AddRouteParser(transport_catalogue::JsonRequestPtr, const json::Dict&);               // парсинг запроса на добавление маршрута
    void AddStopParser(transport_catalogue::JsonRequestPtr, const json::Dict&);                // парсинг запроса на добавление остановки

    void BaseRequestParser(transport_catalogue::JsonRequestPtr, const json::Dict&);            // парсинг запроса на добавление информации
    void StatRequestParser(transport_catalogue::JsonRequestPtr, const json::Dict&);            // парсинг запроса на получение информации


    // базовая шаблонная функция ридера
    // получает на вход поток или строку 
    // собирает json::Document
    // проверяет соответствие документу базовому типу
    // по условию задачи тип опрелен как словарь
    // в случае несоответствия выбрасывает исключение с описанием
    // также в catch добавлены assert чтобы программа остановилась
    template <typename Input>
    json::Dict JSONReader(Input& input) {
        json::Document result;
        
        try
        {
            result = detail::LoadJSON(input);
        }
        catch (const std::exception& e)
        {
            std::cerr << "JSON reading Error"sv << std::endl;
            std::cerr << "FILE::json_reader.h::43"sv << std::endl;
            std::cerr << "FUNC::JSONReader::48"sv << std::endl;
            std::cerr << e.what() << std::endl;
            assert(false);
        }

        try
        {
            if (result.GetRoot().IsMap()) {
                return result.GetRoot().AsMap();
            }
            else {
                throw std::invalid_argument("Error JSON-input data. Incorrect input data format");
            }
        }
        catch (const std::exception& e)
        {
            std::cerr << e.what() << std::endl;
            std::cerr << "FILE::json_reader.h::43"sv << std::endl;
            std::cerr << "FUNC::JSONReader::61"sv << std::endl << std::endl;
            std::cerr << "Correct data format:"sv << std::endl;
            std::cerr << "{"sv << std::endl;
            std::cerr << "   \"base_requests\": [...] ,"sv << std::endl;
            std::cerr << "   \"render_settings\": [...] ,"sv << std::endl;
            std::cerr << "   \"stat_requests\" : [...]"sv << std::endl;
            std::cerr << "}"sv << std::endl;
            assert(false);
        }
    }

    // блок вывода информации

    class JSONPrinter {
    public:

        void AddDocument(const json::Node& node);                                               // добавление документа в принтер

        // функции используемые MakeResult();

        json::Document RouteReportNodeMaker(size_t, transport_catalogue::RouteStatPtr);         // получение результата по маршрутам
        json::Document StopReportNodeMaker(size_t, transport_catalogue::StopStatPtr);           // получение результата по остановкам
        json::Document MapRenderNodeMaker(size_t, std::stringstream&);                          // получение json-результата по рендеру

        JSONPrinter& PrintResult(std::ostream&);                                                // вывод принтера

    private:
        json::Array _json_base = {};
    };


} // namespace json_reader