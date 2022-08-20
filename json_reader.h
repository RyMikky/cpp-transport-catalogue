#pragma once
#include "json.h"
#include "domain.h"

#include <sstream>
#include <cassert>

using namespace std::literals;

namespace json_reader {

    namespace detail {

        json::Document LoadJSON(const std::string&);                                           // �������� JSON �� ���������� ������
        json::Document LoadJSON(std::istream&);                                                // �������� JSON �� ����������� ������

        std::string PrintJSON(const json::Node&);                                              // ������ Node � ����� � std::string
        std::ostream& operator<<(std::ostream&, const json::Node&);                            // ������ ����� Node � �����

        json::Document RouteReportNodeMaker(size_t, transport_catalogue::RouteStatPtr);        // ��������� json-���������� �� ���������
        json::Document StopReportNodeMaker(size_t, transport_catalogue::StopStatPtr);          // ��������� json-���������� �� ����������
        json::Document MapRenderNodeMaker(size_t, std::stringstream&);                         // ��������� json-���������� �� �������

    } // namespace detail

    void AddRouteParser(transport_catalogue::JsonRequestPtr, const json::Dict&);               // ������� ������� �� ���������� ��������
    void AddStopParser(transport_catalogue::JsonRequestPtr, const json::Dict&);                // ������� ������� �� ���������� ���������

    void BaseRequestParser(transport_catalogue::JsonRequestPtr, const json::Dict&);            // ������� ������� �� ���������� ����������
    void StatRequestParser(transport_catalogue::JsonRequestPtr, const json::Dict&);            // ������� ������� �� ��������� ����������


    // ������� ��������� ������� ������
    // �������� �� ���� ����� ��� ������ 
    // �������� json::Document
    // ��������� ������������ ��������� �������� ����
    // �� ������� ������ ��� ������� ��� �������
    // � ������ �������������� ����������� ���������� � ���������
    // ����� � catch ��������� assert ����� ��������� ������������
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

    // ���� ������ ����������

    class JSONPrinter {
    public:

        void AddDocument(const json::Node& node);                                               // ���������� ��������� � �������

        // ������� ������������ MakeResult();

        json::Document RouteReportNodeMaker(size_t, transport_catalogue::RouteStatPtr);         // ��������� ���������� �� ���������
        json::Document StopReportNodeMaker(size_t, transport_catalogue::StopStatPtr);           // ��������� ���������� �� ����������
        json::Document MapRenderNodeMaker(size_t, std::stringstream&);                          // ��������� json-���������� �� �������

        JSONPrinter& PrintResult(std::ostream&);                                                // ����� ��������

    private:
        json::Array _json_base = {};
    };


} // namespace json_reader