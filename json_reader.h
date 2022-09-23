/////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                             //
//        class JsonReader                                                                     //
//        ������������������ ������-����� - ������� I/O ����� ���������                        //
//        �������� ���������� - ���������, ������� � ��������� JSON ��������                   //
//        ��� ������ ���������� �������� ������ ����� ���������                                //
//        ���������� � �����������:                                                            //
//           1. request_handler.h - ���������� �������� � ����, ������� � �������              //
//           2. json_builder.h - ����������� � ���������� ���� � JSON ���������                //
//           3. C++17 (STL)                                                                    //
//                                                                                             // 
//        class JsonPrinter                                                                    //
//        ������������������ ������-����� - ������� I/O �����                                  //
//        �������� ���������� - ����� ���������� � ������� �����                               //
//        ���������� � �����������:                                                            //
//           1. json_builder.h - ����������� � ���������� ���� � JSON ���������                //
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


using namespace std::literals;

namespace transport_catalogue {

    namespace json_reader {

        namespace detail {

            json::Document LoadJson(const std::string&);                                           // �������� JSON �� ���������� ������
            json::Document LoadJson(std::istream&);                                                // �������� JSON �� ����������� ������

            std::string PrintJson(const json::Node&);                                              // ������ Node � ����� � std::string
            std::ostream& operator<<(std::ostream&, const json::Node&);                            // ������ ����� Node � �����

        } // namespace detail


        // ���� ������ ����������
        class JsonPrinter {
        public:

            void AddDocument(const json::Node& node);                                               // ���������� ��������� � �������

            json::Document RouteReportNodeMaker(size_t, transport_catalogue::RouteStatPtr);         // ��������� json-���������� �� ���������
            json::Document StopReportNodeMaker(size_t, transport_catalogue::StopStatPtr);           // ��������� json-���������� �� ����������
            json::Document TransportRouterNodeMaker(size_t, transport_catalogue::RouterData);       // ��������� json-���������� �� �������
            json::Document MapRendererNodeMaker(size_t, transport_catalogue::RendererData);         // ��������� json-���������� �� �������

            JsonPrinter& PrintResult(std::ostream&);                                                // ����� ��������

        private:
            json::Array _json_base = {};
        };


        class JsonReader {

        private:

            std::istream& _input;
            std::ostream& _output;
            transport_catalogue::json_reader::JsonPrinter _printer;                    // ����������� ������� ��� ������ �����������
            transport_catalogue::request_handler::RequestHandler _handler;             // ���������� �������� � ���� ������ ������������� ��������

        public:
            JsonReader(std::istream&, std::ostream&);
            JsonReader& single_block_process();                                                      // ������ ������ � ������ ��������� ������ �����

            // TODO
            // ����� �������� ����� ������ � ��������� ������

        private:
            std::deque<json::Dict> _json_dicts;                                                      // ������� ���������� JSON-������
            std::deque<transport_catalogue::JsonRequest> _json_requests_history;                     // ������� ����������� �������� JSON-�������
            // ��� ������� � ��������� �������� JSON-�������
            std::map<transport_catalogue::RequestType, std::deque<transport_catalogue::JsonRequestPtr>> _current_json_requests = {};

            json::Node LoadJson(const std::string&);                                                 // �������� JSON �� ���������� ������
            json::Node LoadJson(std::istream&);                                                      // �������� JSON �� ����������� ������

            // ---------------------------------------- ������� �������� -------------------------------------------------------------------------------

            void AddRouteParser(transport_catalogue::JsonRequestPtr, const json::Dict&);             // ������� ������� �� ���������� ��������
            void AddStopParser(transport_catalogue::JsonRequestPtr, const json::Dict&);              // ������� ������� �� ���������� ���������

            void BaseRequestParser(transport_catalogue::JsonRequestPtr, const json::Dict&);          // ������� �������� �� ���������� ����������
            void StatRequestParser(transport_catalogue::JsonRequestPtr, const json::Dict&);          // ������� ������� �� ��������� ����������

            void RendererSetupParser(transport_catalogue::map_renderer::RendererSettings&, const json::Dict&);       // ������� ������� ������� ����
            void RouterSetupParser(transport_catalogue::router::RouterSettings&, const json::Dict&);                 // ������� �������� ������� ���������

            // ---------------------------------------- ���� ����������� -------------------------------------------------------------------------------

            JsonReader& BaseRequestProcessor(const json::Array&);                                    // ��������� ������ �������� ����������
            JsonReader& RendererSetupProcessor(const json::Dict&);                                   // ��������� ����� �������� ��������� �������
            JsonReader& RouterSetupProcessor(const json::Dict&);                                     // ��������� ����� �������� ��������� �������
            JsonReader& StatsRequestProcessor(const json::Array&);                                   // ��������� ������ ��������� ����������
            JsonReader& PrintResultProcessor(std::ostream&);                                         // ������ ���������� ����� ��������� ������

            JsonReader& RequestProcessSplitter(const json::Dict&);                                   // �������� ������ ��������
            std::optional<json::Dict> JsonBlockReading();                                            // ������ JSON ����� �� ������

        };

    } // namespace json_reader

} //namespace transport_catalogue 
