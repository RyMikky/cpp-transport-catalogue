#include "json_reader.h"

namespace json_reader {

    namespace detail {

        //�������� JSON �� ���������� ������
        json::Document LoadJSON(const std::string& s) {
            std::istringstream strm(s);
            return json::Load(strm);
        }

        // �������� JSON �� ����������� ������
        json::Document LoadJSON(std::istream& input) {
            return json::Load(input);
        }

        // ������ Node � ����� � std::string
        std::string PrintJSON(const json::Node& node) {
            std::ostringstream out;
            json::Print(json::Document{ node }, out);
            return out.str();
        }

        // ������ ����� Node � �����
        std::ostream& operator<<(std::ostream& out, const json::Node& node) {
            json::Print(json::Document{ node }, out);
            return out;
        }

        // ��������� json-���������� �� ���������
        json::Document RouteReportNodeMaker(size_t id, transport_catalogue::RouteStatPtr route_stat) {
            if (route_stat == nullptr) {
                return json::Document(json::Dict{ { "request_id", static_cast<int>(id)}, {"error_message", "not found"} });
            }
            else {
                return json::Document(json::Dict{
                    {"request_id", static_cast<int>(id)},
                    {"curvature", route_stat->_curvature},
                    {"route_length", static_cast<int>(route_stat->_real_route_length)},
                    {"stop_count", static_cast<int>(route_stat->_stops_on_route)},
                    {"unique_stop_count", static_cast<int>(route_stat->_unique_stops)}
                    });
            }
        }

        // ��������� json-���������� �� ����������
        json::Document StopReportNodeMaker(size_t id, transport_catalogue::StopStatPtr stop_stat) {
            if (stop_stat == nullptr) {
                return json::Document(json::Dict{ { "request_id", static_cast<int>(id)}, {"error_message", "not found"} });
            }
            else {
                json::Array buses;
                for (auto& bus : stop_stat->_buses) {
                    buses.push_back(detail::LoadJSON(std::string(bus)).GetRoot());
                }
                return json::Document(json::Dict{ {"buses", buses}, {"request_id", static_cast<int>(id)} });
            }
        }

        // ��������� json-���������� �� �������
        json::Document MapRenderNodeMaker(size_t id, std::stringstream& stream) {
            return json::Document(json::Dict{ { "map", stream.str()}, {"request_id", static_cast<int>(id)} });
        }

    } // namespace detail


    // ������� ������� �� ���������� ��������
    void AddRouteParser(transport_catalogue::JsonRequestPtr request, const json::Dict& node) {
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
    void AddStopParser(transport_catalogue::JsonRequestPtr request, const json::Dict& node) {
        if (node.count("latitude") != 0) {
            request->_coord.lat = node.at("latitude").AsDouble();
        }

        if (node.count("longitude") != 0) {
            request->_coord.lng = node.at("longitude").AsDouble();
        }

        if (node.count("road_distances") != 0) {
            auto distances_ = node.at("road_distances").AsMap();
            for (auto& item : distances_) {
                request->_distances[item.first] = static_cast<int64_t>(item.second.AsInt());
            }
        }

    }

    // ������� ������� �� ���������� ����������
    void BaseRequestParser(transport_catalogue::JsonRequestPtr request, const json::Dict& node) {

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
    void StatRequestParser(transport_catalogue::JsonRequestPtr request, const json::Dict& node) {
        // ���������� id �������
        if (node.count("id") != 0) {
            request->_id = node.at("id").AsInt();
        }
        
        // ���������� ����� �������� ��� �������� ���������
        if (node.count("name") != 0) {
            request->_name = node.at("name").AsString();
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
            else {
                throw std::invalid_argument("invalid_argument StatRequest Type. WHERE::json_reader.cpp::185::StatRequestParser::197");
            }
        }

        request->_type = transport_catalogue::RequestType::stat_info;
    }

    // ���������� ��������� � �������
    void JSONPrinter::AddDocument(const json::Node& node) {
        _json_base.push_back(std::move(node));
    }

    // ��������� ���������� �� ���������
    json::Document JSONPrinter::RouteReportNodeMaker(size_t id, transport_catalogue::RouteStatPtr route_stat) {
        if (route_stat == nullptr) {
            return json::Document(json::Dict{ { "request_id", static_cast<int>(id)}, {"error_message", "not found"} });
        }
        else {
            return json::Document(json::Dict{ 
                {"request_id", static_cast<int>(id)}, 
                {"curvature", route_stat->_curvature},
                {"route_length", static_cast<int>(route_stat->_real_route_length)},
                {"stop_count", static_cast<int>(route_stat->_stops_on_route)},
                {"unique_stop_count", static_cast<int>(route_stat->_unique_stops)}
                });
        }
    }

    // ��������� ���������� �� ����������
    json::Document JSONPrinter::StopReportNodeMaker(size_t id, transport_catalogue::StopStatPtr stop_stat) {
        if (stop_stat == nullptr) {
            return json::Document(json::Dict{ { "request_id", static_cast<int>(id)}, {"error_message", "not found"} });
        }
        else {
            json::Array buses;
            for (auto& bus : stop_stat->_buses) {
                buses.push_back(json::Node{std::string(bus)});
            }
            return json::Document(json::Dict{ {"buses", buses}, {"request_id", static_cast<int>(id)}});
        }
    }

    // ��������� json-���������� �� �������
    json::Document JSONPrinter::MapRenderNodeMaker(size_t id, std::stringstream& stream) {
        return json::Document(json::Dict{ { "map", stream.str()}, {"request_id", static_cast<int>(id)}});
    }

    // ����� ��������
    JSONPrinter& JSONPrinter::PrintResult(std::ostream& out) {
        json::PrintContext context(out, 0, 4);
        if (_json_base.size() != 0) {
            json::Print(json::Document(_json_base), context._out);
        }
        else {
            throw std::invalid_argument("Render Base is Empty");
        }
        
        return *this;
    }

} // namespace json_reader