#pragma once
#include <iostream>

#include "transport_catalogue.h"                       // ������ ������������� ��������
#include "map_renderer.h"                              // ������ ������������
#include "domain.h"                                    // ������ �������� ��������

//#include "simple_reader.h"                           // ������ ������������ input � stat reader
#include "json_reader.h"                               // ������ �����/������ KSON

using namespace std::literals;

namespace transport_catalogue {

	namespace request_handler {

		// �������� ����� ���������� ��������
		// ������ ��������� ������ �������-�����������
		class RequestHandler
		{
		protected:
			transport_catalogue::TransportCatalogue& _catalogue;
			std::istream& _input;
			std::ostream& _output;

		public:
			RequestHandler() = default;
			RequestHandler(transport_catalogue::TransportCatalogue&, std::istream&, std::ostream&);

			virtual ~RequestHandler() = default;
			virtual void AddDistanceProcess() = 0;                               // ���������� �������� �� ���������� ��������� �� �������
			virtual void AddStopProcess() = 0;                                   // ���������� �������� �� ���������� ���������
			virtual void AddRouteProcess() = 0;                                  // ���������� �������� �� ���������� ��������
			virtual void StopInfoProcess() = 0;                                  // ���������� �������� �� ��������� ���������� �� ���������
			virtual void RouteInfoProcess() = 0;                                 // ���������� �������� �� ��������� ���������� �� ��������
			virtual void StatInfoProcess() = 0;                                  // ���������� �������� �� ��������� ���������� � ����� ������
			virtual void RequestsProcess() = 0;                                  // ���������� ������� ������� ��������
		private:
			virtual void RequestProcessor() = 0;                                 // ����� �����, �������������� �� ������������
		};

		//// �����-��������� �� �������� RequestHandler
		//// �������� � ������ ��������� �������� ������� ������� �� ��������� ������
		//class RequestHandlerSimpleTerminal : protected RequestHandler {
		//public:
		//	RequestHandlerSimpleTerminal() = default;
		//	RequestHandlerSimpleTerminal(transport_catalogue::TransportCatalogue&, std::istream&, std::ostream&);

		//	SimpleRequestPtr AddSimpleRequest(std::string);

		//	// ��������� ������ � ��� ������� ����� ����� ��������
		//	void AddInProcessLine(SimpleRequestPtr request) {
		//		_current_simple_requests[request->_type].push_back(request);
		//	}

		//	void AddSimpleRequest(SimpleRequest&&);                               // ��������� ������ �� ������� ���������
		//	Route MakeRoute(SimpleRequestPtr);                                    // ������ ������� �� ������ �������

		//	// ������ ��������� �� ������ �������
		//	Stop MakeStop(SimpleRequestPtr request) {
		//		return { std::string(request->_key_name), request->_coord.lat, request->_coord.lng };
		//	}

		//	void AddDistanceProcess(SimpleRequestPtr);                            // ���������� �������� �� ���������� ��������� �� �������
		//	void AddDistanceProcess() override;                                   // ���������� �������� �� ���������� ��������� �� �������
		//	void AddStopProcess() override;                                       // ���������� �������� �� ���������� ���������
		//	void AddRouteProcess() override;                                      // ���������� �������� �� ���������� ��������
		//	void StopInfoProcess() override;                                      // ���������� �������� �� ��������� ���������� �� ���������
		//	void RouteInfoProcess() override;                                     // ���������� �������� �� ��������� ���������� �� ��������
		//	void StatInfoProcess() override;                                      // ���������� �������� �� ��������� ���������� � ����� ������
		//	void RequestsProcess() override;                                      // ���������� ������� ������� ��������

		//private:

		//	void RequestProcessor() override;                                     // ����� �����, �������������� �� ������������

		//	// ������� ����������� �������� �������� ����
		//	std::deque<SimpleRequest> _simple_requests_history = {};
		//	// ��� ������� � ��������� �������� �������� ����
		//	std::map<RequestType, std::deque<SimpleRequestPtr>> _current_simple_requests = {};
		//	// ��������� ��������� ��� sub-�������� �� ���������� ���������
		//	std::map<std::string_view, std::map<std::string_view, int64_t>*> _current_distance_simple_requests = {};
		//};

		//// �����-��������� �� �������� RequestHandler
		//// �������� � ������ ��������� ���� ������� �������� ��������� � ����� ������
		//class RequestHandlerSimple : protected RequestHandler {
		//public:
		//	RequestHandlerSimple() = default;
		//	RequestHandlerSimple(transport_catalogue::TransportCatalogue&, std::istream&, std::ostream&);

		//	SimpleRequestPtr AddSimpleRequest(std::string);

		//	// ��������� ������ � ��� ������� ����� ����� ��������
		//	void AddInProcessLine(SimpleRequestPtr request) {
		//		_current_simple_requests[request->_type].push_back(request);
		//	}

		//	void AddSimpleRequest(SimpleRequest&&);                               // ��������� ������ �� ������� ���������
		//	Route MakeRoute(SimpleRequestPtr);                                    // ������ ������� �� ������ �������

		//	// ������ ��������� �� ������ �������
		//	Stop MakeStop(SimpleRequestPtr request) {
		//		return { std::string(request->_key_name), request->_coord.lat, request->_coord.lng };
		//	}

		//	void AddDistanceProcess(SimpleRequestPtr);                            // ���������� �������� �� ���������� ��������� �� �������
		//	void AddDistanceProcess() override;                                   // ���������� �������� �� ���������� ��������� �� �������
		//	void AddStopProcess() override;                                       // ���������� �������� �� ���������� ���������
		//	void AddRouteProcess() override;                                      // ���������� �������� �� ���������� ��������
		//	void StopInfoProcess() override;                                      // ���������� �������� �� ��������� ���������� �� ���������
		//	void RouteInfoProcess() override;                                     // ���������� �������� �� ��������� ���������� �� ��������
		//	void StatInfoProcess() override;                                      // ���������� �������� �� ��������� ���������� � ����� ������
		//	void RequestsProcess() override;                                      // ���������� ������� ������� ��������

		//private:

		//	void RequestProcessor() override;                                     // ����� �����, �������������� �� ������������

		//	// ������� ����������� �������� �������� ����
		//	std::deque<SimpleRequest> _simple_requests_history = {};
		//	// ��� ������� � ��������� �������� �������� ����
		//	std::map<RequestType, std::deque<SimpleRequestPtr>> _current_simple_requests = {};
		//	// ��������� ��������� ��� sub-�������� �� ���������� ���������
		//	std::map<std::string_view, std::map<std::string_view, int64_t>*> _current_distance_simple_requests = {};
		//};

		// �����-��������� �� �������� RequestHandler
		// �������� � ������ ��������� JSON-������
		class RequestHandlerJSON : protected RequestHandler {
		public:
			RequestHandlerJSON() = default;
			RequestHandlerJSON(transport_catalogue::TransportCatalogue&, std::istream&, std::ostream&);

			json::Dict& AddJSONlibary(json::Dict&&);                              // ���������� JSON-����� � ����
			void AddJSONRequest(JsonRequest&&);                                   // ��������� ������ �� ������� ���������

			Route MakeRoute(JsonRequestPtr);                                      // ������ ������� �� ������ �������
			Stop MakeStop(JsonRequestPtr request);                                // ������ ��������� �� ������ �������

			void RenderSetupProcess(json::Dict);                                  // ���������� �������� �������
			//void RenderProcess();                                                 // �������� ������ � ������, ������ �������, ������� �������
			void RenderProcess(map_renderer::MapRenderer&);                       // ������ ������� �� ������� �� JSON

			void AddDistanceProcess(JsonRequestPtr);                              // ���������� �������� �� ���������� ��������� �� �������
			void AddDistanceProcess() override;                                   // ���������� �������� �� ���������� ��������� �� �������
			void AddStopProcess() override;                                       // ���������� �������� �� ���������� ���������
			void AddRouteProcess() override;                                      // ���������� �������� �� ���������� ��������
			void StopInfoProcess() override;                                      // ���������� �������� �� ��������� ���������� �� ���������
			void RouteInfoProcess() override;                                     // ���������� �������� �� ��������� ���������� �� ��������
			void StatInfoProcess() override;                                      // ���������� �������� �� ��������� ���������� � ����� ������
			void RequestsProcess() override;                                      // ���������� ������� ������� ��������

		private:

			void RequestProcessor() override;                                     // ����� �����, ���������� �� ������������

			map_renderer::RendererSettings _render_settings;
			//map_renderer::MapRenderer _render;                                  // ������� ����������� ������ �������� XML � �������� ����� ������	
			std::deque<JsonRequest> _json_requests_history;                       // ������� ����������� �������� JSON-�������
			std::deque<json::Dict> _json_dicts;                                   // ������� ���������� JSON-������
			// ��� ������� � ��������� �������� JSON-�������
			std::map<RequestType, std::deque<JsonRequestPtr>> _current_json_requests = {};
			// ��������� ��������� ��� sub-�������� �� ���������� ���������
			std::map<std::string, std::map<std::string, int64_t>*> _current_distance_json_requests = {};
		};

	} // namespace request_handler

} // namespace transport_catalogue



// ����� RequestHandler ������ ���� ������, ����������� �������������� JSON reader-�
// � ������� ������������ ����������.
// ��. ������� �������������� �����: https://ru.wikipedia.org/wiki/�����_(������_��������������)
/*
class RequestHandler {
public:
	// MapRenderer ����������� � ��������� ����� ��������� �������
	RequestHandler(const TransportCatalogue& db, const renderer::MapRenderer& renderer);

	// ���������� ���������� � �������� (������ Bus)
	std::optional<BusStat> GetBusStat(const std::string_view& bus_name) const;

	// ���������� ��������, ���������� �����
	const std::unordered_set<BusPtr>* GetBusesByStop(const std::string_view& stop_name) const;

	// ���� ����� ����� ����� � ��������� ����� ��������� �������
	svg::Document RenderMap() const;

private:
	// RequestHandler ���������� ��������� �������� "������������ ����������" � "������������ �����"
	const TransportCatalogue& db_;
	const renderer::MapRenderer& renderer_;
};
*/