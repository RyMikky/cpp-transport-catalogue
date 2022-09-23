#include "json_reader.h"

namespace transport_catalogue {

    namespace json_reader {

        namespace detail {

            //�������� JSON �� ���������� ������
            json::Document LoadJson(const std::string& s) {
                std::istringstream strm(s);
                return json::Load(strm);
            }

            // �������� JSON �� ����������� ������
            json::Document LoadJson(std::istream& input) {
                return json::Load(input);
            }

            // ������ Node � ����� � std::string
            std::string PrintJson(const json::Node& node) {
                std::ostringstream out;
                json::Print(json::Document{ node }, out);
                return out.str();
            }

            // ������ ����� Node � �����
            std::ostream& operator<<(std::ostream& out, const json::Node& node) {
                json::Print(json::Document{ node }, out);
                return out;
            }

        } // namespace detail



        // ---------------------------------------- class JsonPrinter ---------------------------------------------------

        // ���������� ��������� � �������
        void JsonPrinter::AddDocument(const json::Node& node) {
            _json_base.push_back(std::move(node));
        }

        // ��������� json-���������� �� ���������
        json::Document JsonPrinter::RouteReportNodeMaker(size_t id, transport_catalogue::RouteStatPtr route_stat) {
            if (route_stat == nullptr) {
                return json::Document(
                    json::Builder()
                    .StartDict()
                    .Key("request_id").Value(static_cast<int>(id))
                    .Key("error_message").Value("not found")
                    .EndDict()
                    .Build());
            }
            else {
                return json::Document(
                    json::Builder()
                    .StartDict()
                    .Key("request_id").Value(static_cast<int>(id))
                    .Key("curvature").Value(route_stat->_curvature)
                    .Key("route_length").Value(static_cast<int>(route_stat->_real_route_length))
                    .Key("stop_count").Value(static_cast<int>(route_stat->_stops_on_route))
                    .Key("unique_stop_count").Value(static_cast<int>(route_stat->_unique_stops))
                    .EndDict()
                    .Build());
            }
        }

        // ��������� json-���������� �� ����������
        json::Document JsonPrinter::StopReportNodeMaker(size_t id, transport_catalogue::StopStatPtr stop_stat) {
            if (stop_stat == nullptr) {
                return json::Document(
                    json::Builder()
                    .StartDict()
                    .Key("request_id").Value(static_cast<int>(id))
                    .Key("error_message").Value("not found")
                    .EndDict()
                    .Build());
            }
            else {
                json::Array buses;
                for (auto& bus : stop_stat->_buses) {
                    buses.push_back(json::Node{ std::string(bus) });
                }
                return json::Document(
                    json::Builder()
                    .StartDict()
                    .Key("buses").Value(buses)
                    .Key("request_id").Value(static_cast<int>(id))
                    .EndDict()
                    .Build());
            }
        }

        // ��������� json-���������� �� �������
        json::Document JsonPrinter::TransportRouterNodeMaker(size_t id, transport_catalogue::RouterData data) {
            if (!data._is_founded) {
                return json::Document(
                    json::Builder()
                    .StartDict()
                    .Key("request_id").Value(static_cast<int>(id))
                    .Key("error_message").Value("not found")
                    .EndDict()
                    .Build());
            }
            else {
                json::Array route_items;
                for (auto& item : data._route_items) {

                    if (item.GetItemType() == graph::wait)
                    {
                        route_items.push_back(
                            json::Builder()
                            .StartDict()
                            .Key("type").Value(std::string("Wait"))
                            .Key("stop_name").Value(std::string(item.GetItemName()))
                            .Key("time").Value(item.GetItemTime())
                            .EndDict()
                            .Build());
                    }
                    else
                    {
                        route_items.push_back(
                            json::Builder()
                            .StartDict()
                            .Key("type").Value(std::string("Bus"))
                            .Key("bus").Value(std::string(item.GetItemName()))
                            .Key("span_count").Value(item.GetItemSpanCount())
                            .Key("time").Value(item.GetItemTime())
                            .EndDict()
                            .Build());
                    }

                }
                return json::Document(
                    json::Builder()
                    .StartDict()
                    .Key("request_id").Value(static_cast<int>(id))
                    .Key("total_time").Value(data._route_time)
                    .Key("items").Value(route_items)
                    .EndDict()
                    .Build());
            }
        }

        // ��������� json-���������� �� �������
        json::Document JsonPrinter::MapRendererNodeMaker(size_t id, transport_catalogue::RendererData data) {
            return json::Document(
                json::Builder()
                .StartDict()
                .Key("map").Value(data._svg_line)
                .Key("request_id").Value(static_cast<int>(id))
                .EndDict()
                .Build());
        }

        // ����� ��������
        JsonPrinter& JsonPrinter::PrintResult(std::ostream& out) {
            json::PrintContext context(out, 4, 0);
            if (_json_base.size() != 0) {
                json::Print(json::Document(_json_base), context._out);
            }
            else {
                throw std::invalid_argument("Render Base is Empty");
            }

            return *this;
        }

        // ---------------------------------------- class JsonPrinter END -----------------------------------------------


        // ---------------------------------------- class JsonReader ----------------------------------------------------

        JsonReader::JsonReader(std::istream& in, std::ostream& out)
            : _input(in), _output(out) {
        }

        JsonReader& JsonReader::single_block_process() {
            // ������ �����
            auto json_request = JsonBlockReading();

            if (json_request.has_value()) {
                // ��������� ��������� �����
                RequestProcessSplitter(json_request.value()).PrintResultProcessor(_output);
            }
            else {
                throw std::invalid_argument("JSON Data is Empty");
            }
            return *this;
        }

        //�������� JSON �� ���������� ������
        json::Node JsonReader::LoadJson(const std::string& s) {
            std::istringstream strm(s);
            return json::Load(strm).GetRoot();
        }

        // �������� JSON �� ����������� ������
        json::Node JsonReader::LoadJson(std::istream& input) {
            return json::Load(input).GetRoot();
        }

        // ������� ������� �� ���������� ��������
        void JsonReader::AddRouteParser(transport_catalogue::JsonRequestPtr request, const json::Dict& node) {
            if (node.count("is_roundtrip") != 0) {
                if (node.at("is_roundtrip").AsBool()) {
                    request->_is_circular = true;
                }
                else {
                    request->_is_circular = false;
                }
            }

            if (node.count("stops") != 0) {
                auto stops_ = node.at("stops").AsArray();
                for (auto stop : stops_) {
                    request->_stops.push_back(stop.AsString());
                }
            }
        }

        // ������� ������� �� ���������� ���������
        void JsonReader::AddStopParser(transport_catalogue::JsonRequestPtr request, const json::Dict& node) {
            if (node.count("latitude") != 0) {
                request->_coord.lat = node.at("latitude").AsDouble();
            }

            if (node.count("longitude") != 0) {
                request->_coord.lng = node.at("longitude").AsDouble();
            }

            if (node.count("road_distances") != 0) {
                auto distances_ = node.at("road_distances").AsDict();
                for (auto& item : distances_) {
                    request->_distances[item.first] = static_cast<int64_t>(item.second.AsInt());
                }
            }

        }

        // ������� ������� �� ���������� ����������
        void JsonReader::BaseRequestParser(transport_catalogue::JsonRequestPtr request, const json::Dict& node) {

            // ���������� ����� �������� ��� �������� ���������
            if (node.count("name") != 0) {
                request->_name = node.at("name").AsString();
            }

            // ���������� ��� ������� � ������ �������� ����
            if (node.count("type") != 0) {
                if (node.at("type").AsString() == "Bus") {
                    request->_type = transport_catalogue::RequestType::add_route;
                    request->_key = "Bus";
                    AddRouteParser(request, node);
                }
                else {
                    request->_type = transport_catalogue::RequestType::add_stop;
                    request->_key = "Stop";
                    AddStopParser(request, node);
                }
            }
        }

        // ������� ������� �� ��������� ����������
        void JsonReader::StatRequestParser(transport_catalogue::JsonRequestPtr request, const json::Dict& node) {
            // ���������� id �������
            if (node.count("id") != 0) {
                request->_id = node.at("id").AsInt();
            }

            // ���������� ����� �������� ��� �������� ���������
            if (node.count("name") != 0) {
                request->_name = node.at("name").AsString();
            }

            // ���������� ��������� ��������� ��� �������� ����
            if (node.count("from") != 0) {
                request->_from = node.at("from").AsString();
            }

            // ���������� �������� ��������� ��� �������� ����
            if (node.count("to") != 0) {
                request->_to = node.at("to").AsString();
            }

            // ���������� ��� ������� � ������ �������� ����
            if (node.count("type") != 0) {
                if (node.at("type").AsString() == "Bus") {
                    request->_key = "Bus";
                }
                else if (node.at("type").AsString() == "Stop") {
                    request->_key = "Stop";
                }
                else if (node.at("type").AsString() == "Map") {
                    request->_key = "Map";
                }
                else if (node.at("type").AsString() == "Route") {
                    request->_key = "Route";
                }
                else {
                    throw std::invalid_argument("invalid_argument StatRequest Type\n WHERE::json_reader.cpp::131::StatRequestParser::153");
                }
            }

            request->_type = transport_catalogue::RequestType::stat_info;
        }

        // ������� ������� ������� ����
        void JsonReader::RendererSetupParser(transport_catalogue::map_renderer::RendererSettings& settings, const json::Dict& node) {
            for (auto& item : node) {
                if (item.first == "width"s)
                {
                    settings.SetWidth(item.second.AsDouble());
                }
                else if (item.first == "height"s)
                {
                    settings.SetHeight(item.second.AsDouble());
                }
                else if (item.first == "padding"s)
                {
                    settings.SetPadding(item.second.AsDouble());
                }
                else if (item.first == "line_width"s)
                {
                    settings.SetLineWidth(item.second.AsDouble());
                }
                else if (item.first == "stop_radius"s)
                {
                    settings.SetStopRadius(item.second.AsDouble());
                }
                else if (item.first == "bus_label_font_size"s)
                {
                    settings.SetBusLabelFont(static_cast<size_t>(item.second.AsInt()));
                }
                else if (item.first == "bus_label_offset"s)
                {
                    transport_catalogue::map_renderer::LabelOffset offset(item.second.AsArray()[0].AsDouble(), item.second.AsArray()[1].AsDouble());
                    settings.SetBusLabelOffset(offset);
                }
                else if (item.first == "stop_label_font_size"s)
                {
                    settings.SetStopLabelFont(static_cast<size_t>(item.second.AsInt()));
                }
                else if (item.first == "stop_label_offset"s)
                {
                    transport_catalogue::map_renderer::LabelOffset offset(item.second.AsArray()[0].AsDouble(), item.second.AsArray()[1].AsDouble());
                    settings.SetStopLabelOffset(offset);
                }
                else if (item.first == "underlayer_color"s)
                {
                    if (item.second.IsString()) {
                        settings.SetUnderlayerColor(item.second.AsString());
                    }
                    else {
                        auto array = item.second.AsArray();
                        if (array.size() == 3) {
                            settings.SetUnderlayerColor(svg::Rgb(array[0].AsInt(), array[1].AsInt(), array[2].AsInt()));
                        }
                        else {
                            settings.SetUnderlayerColor(svg::Rgba(array[0].AsInt(), array[1].AsInt(), array[2].AsInt(), array[3].AsDouble()));
                        }
                    }
                }
                else if (item.first == "underlayer_width"s)
                {
                    settings.SetUnderlayerWidth(item.second.AsDouble());
                }
                else if (item.first == "color_palette"s)
                {
                    if (item.second.IsArray()) {
                        settings.ResetColorPalette();
                        for (auto& color : item.second.AsArray()) {
                            if (color.IsString()) {
                                settings.AddColorInPalette(color.AsString());
                            }
                            else {
                                auto array = color.AsArray();
                                if (array.size() == 3) {
                                    settings.AddColorInPalette(svg::Rgb(array[0].AsInt(), array[1].AsInt(), array[2].AsInt()));
                                }
                                else {
                                    settings.AddColorInPalette(svg::Rgba(array[0].AsInt(), array[1].AsInt(), array[2].AsInt(), array[3].AsDouble()));
                                }
                            }
                        }
                    }
                    else {
                        continue;
                    }
                }
                else {
                    continue;
                }
            }
        }

        // ������� �������� ������� ���������
        void JsonReader::RouterSetupParser(transport_catalogue::router::RouterSettings& settings, const json::Dict& node) {
            for (auto& item : node) {
                if (item.first == "bus_wait_time"s)
                {
                    settings.SetBusWaitTime(static_cast<size_t>(item.second.AsInt()));
                }
                else if (item.first == "bus_velocity"s)
                {
                    settings.SetBusVelocity(item.second.AsDouble());
                }
                else
                {
                    continue;
                }
            }
        }

        // ��������� ������ �������� ����������
        JsonReader& JsonReader::BaseRequestProcessor(const json::Array& base_requests) {
            // ������ ������ ������
            for (size_t i = 0; i != base_requests.size(); i++)
            {
                transport_catalogue::JsonRequest base_request;
                // ������ � ������ ����� ������ �� ���������� ���������� � ����
                BaseRequestParser(&base_request, base_requests[i].AsDict());
                // ���������� ������ � ������� ��������
                transport_catalogue::JsonRequest& ptr = _json_requests_history.emplace_back(std::move(base_request));
                // ���������� ��������� �� ������ � ������� ������� �������� � ���������
                _current_json_requests[ptr._type].push_back(&ptr);
            }

            if (_current_json_requests.count(transport_catalogue::add_stop)) {
                // ��������� ��������� � ���� ��������, ����� �������� ��������� ��������� ��������� ���������
                _handler.AddStopsProcess(_current_json_requests.at(transport_catalogue::add_stop));
            }

            if (_current_json_requests.count(transport_catalogue::add_route)) {
                // ��������� �������� � ���� ��������
                _handler.AddRoutesProcess(_current_json_requests.at(transport_catalogue::add_route));
            }
            return *this;
        }

        // ��������� ����� �������� ��������� �������
        JsonReader& JsonReader::RendererSetupProcessor(const json::Dict& renderer_settings) {
            transport_catalogue::map_renderer::RendererSettings settings;
            // ������ � ������ ��������� ������� ����
            RendererSetupParser(settings, renderer_settings);
            // �������� �� ���������� ���������� �������� � ���� ������������� ��������
            _handler.SetRedererSettings(std::move(settings)).EnableMapRenderer();
            return *this;
        }

        // ��������� ����� �������� ��������� �������
        JsonReader& JsonReader::RouterSetupProcessor(const json::Dict& router_settings) {
            transport_catalogue::router::RouterSettings settings;
            // ������ � ������ ��������� ������� ���������
            RouterSetupParser(settings, router_settings);
            // �������� �� ���������� ���������� �������� � ���� ������������� ��������
            _handler.SetRouterSettings(std::move(settings)).EnableTransportRouter();
            return *this;
        }

        // ��������� ������ ��������� ����������
        JsonReader& JsonReader::StatsRequestProcessor(const json::Array& stat_requests) {
            // ������ ������� ������ ������ �� �������
            for (size_t i = 0; i != stat_requests.size(); i++)
            {
                transport_catalogue::JsonRequest stat_request;
                // ������ � ������ ����� ������ �� ���������� ���������� � ����
                StatRequestParser(&stat_request, stat_requests[i].AsDict());

                // ������������ ������� �� ��������� ���������� �� ���� ��� ������ � ���� �������
                transport_catalogue::JsonRequestPtr request = &(_json_requests_history.emplace_back(std::move(stat_request)));

                if (request->_key == "Bus") {
                    _printer.AddDocument(_printer.RouteReportNodeMaker(
                        request->_id, _handler.GetRouteInfo(request)).GetRoot());
                }
                // ������� �� ������ ���������� �� ����������
                else if (request->_key == "Stop") {
                    _printer.AddDocument(_printer.StopReportNodeMaker(
                        request->_id, _handler.GetStopInfo(request)).GetRoot());
                }
                // ������ �� �������� ���������
                else if (request->_key == "Route") {

                    _printer.AddDocument(_printer.TransportRouterNodeMaker(
                        request->_id, _handler.GetRouterData(request)).GetRoot());

                }
                // ������� �� ��������� ����� ���������
                else {
                    _printer.AddDocument(_printer.MapRendererNodeMaker(
                        request->_id, _handler.GetRendererData(request)).GetRoot());
                }
            }

            return *this;
        }

        // ������ ���������� ����� ��������� ������
        JsonReader& JsonReader::PrintResultProcessor(std::ostream& output) {
            _printer.PrintResult(output);
            return *this;
        }

        // �������� ������ ��������
        JsonReader& JsonReader::RequestProcessSplitter(const json::Dict& json_request) {

            // ������� ������� ������� ��������
            _current_json_requests.clear();

            // ��������� ��������� �������� �� �������� ����������
            if (json_request.count("base_requests"))
            {
                BaseRequestProcessor(json_request.at("base_requests").AsArray());
            }

            // �������� ��������� ������� �� �������� �������� �������
            if (json_request.count("render_settings")) 
            {
                RendererSetupProcessor(json_request.at("render_settings").AsDict());
            }

            // �������� ��������� ������� �� �������� �������� �������
            if (json_request.count("routing_settings")) 
            {
                RouterSetupProcessor(json_request.at("routing_settings").AsDict());
            }

            // ��������� ��������� �������� �� ������ ����������
            if (json_request.count("stat_requests")) 
            {
                StatsRequestProcessor(json_request.at("stat_requests").AsArray());
            }
            return *this;
        }

        std::optional<json::Dict> JsonReader::JsonBlockReading() {
            try
            {
                // ������� ������� �����
                json::Node result = LoadJson(_input);
                if (result.IsDict()) {
                    // ���� ����� �������� � ��������� �������� ��������
                    return result.AsDict();
                }
                else {
                    throw std::invalid_argument("Error JSON-input data. Incorrect input data format");
                }
            }
            catch (const std::exception& e)
            {
                std::cerr << "Error JSON-input data"sv << std::endl;
                std::cerr << "Incorrect input data format"sv << std::endl;
                std::cerr << "FILE::json_reader.h::43"sv << std::endl;
                std::cerr << "FUNC::JSONReader::61"sv << std::endl << std::endl;
                std::cerr << "Correct data format:"sv << std::endl;
                std::cerr << "{"sv << std::endl;
                std::cerr << "   \"base_requests\": [...] ,"sv << std::endl;
                std::cerr << "   \"render_settings\": [...] ,"sv << std::endl;
                std::cerr << "   \"routing_settings\": [...] ,"sv << std::endl;
                std::cerr << "   \"stat_requests\" : [...]"sv << std::endl;
                std::cerr << "}"sv << std::endl;
                std::cerr << e.what() << std::endl;
            }
        }

        // ---------------------------------------- class JsonReader END ------------------------------------------------

    } // namespace json_reader

} // namespace transport_catalogue