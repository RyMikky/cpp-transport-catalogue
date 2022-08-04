#pragma once

#include <iostream>

#include "transport_catalogue.h"
#include "stat_reader.h"

using namespace std::literals;

namespace transport_catalogue {

	struct Request;
	using RequestPtr = Request*;

	struct StatRequest;
	using StatRequestPtr = StatRequest*;

	enum RequestType {
		null_req = 0,
		add_stop, add_route, add_dist, stop_info, route_info, stat_info, print_history

		// stat_info - ����� ��� ������� �� ��������� ���������� �� �������� ��� ���������
		// ����� �� ������� ������, ��� ��� ��� ������� �� ��������� ���������� ������ 
		// �������������� � ������� �����������, � �� �� ������� ��� � ���� � ����

		// add_dist - ���� �� �����������, ��� ��� ���������� ��������� ���������� ���
		// sub-������ � ���������� ���������, ���� ���� _distances ����� ������

		// print_history - ���� �� �����������, ��� ��� ������� � ����������� ������
		// ����� ����� ��� ������ ������ � ������������ ����������� ��������� ��������
		// ������� �������� � ��������� �������
	};

	// ��������� ������� ������ ����
	struct Request {
		Request() = default;

		explicit Request(std::string&& line) : _input_line(line) {
		}

		RequestPtr GetPtr() {
			return this;
		}

		RequestType _type = RequestType::null_req;               // ��� �������
		std::string_view _key = ""sv;                            // ���� �������
		std::string_view _key_name = ""sv;                       // ��� ������� (��������, ���������)
		geo::Coordinates _coord = { 0L, 0L };                    // ����������, ���� ���������
		std::vector<std::string_view> _stops = {};               // ���������, ���� �������
		std::map<std::string_view, int64_t> _distances = {};     // ���������� �� ���������
		bool _is_circular = true;                                // ��� ��������
		std::string _input_line = ""s;                           // ������� ������
	};

	// ��������� ������� ����_������� �� ��������� ����������
	struct StatRequest {
		explicit StatRequest(std::string&& line) : _input_line(line) {
		}

		StatRequestPtr GetPtr() {
			return this;
		}
	
		RequestType _type = RequestType::stat_info;              // ��� �������
		std::string_view _key = ""sv;                            // ���� �������
		std::string_view _key_name = ""sv;                       // ��� ������� (��������, ���������)
		std::string _input_line = ""s;                           // ������� ������
	};

	// �������� ����� ���������� ��������
	// ��������� ������� �������� � �������
	// ��������� ������� ��������
	class RequestHandler
	{
	public:

		// ������ ����� ������ � ������� � �������� ������ � ��� �������� ��������
		RequestPtr AddRequest(std::string);

		// ��������� ������ � ��� ������� ����� ����� ��������
		void AddInProcessLine(RequestPtr request) {
			_current_requests[request->_type].push_back(request);
		}

		// ��������� ������ �� ������� ���������
		void AddRequest(Request&&);

		// ������ ������� �� ������ �������
		Route MakeRoute(TransportCatalogue&, RequestPtr);

		// ������ ��������� �� ������ �������
		Stop MakeStop(RequestPtr request) {
			return { std::string(request->_key_name), request->_coord.lat, request->_coord.lng };
		}

		// ���������� �������� �� ���������� ��������� �� �������
		void AddDistanceProcess(TransportCatalogue&);

		// ���������� �������� �� ���������� ��������� �� �������
		void AddDistanceProcess(TransportCatalogue&, RequestPtr);

		// ���������� �������� �� ���������� ���������
		void AddStopProcess(TransportCatalogue&);

		// ���������� �������� �� ���������� ��������
		void AddRouteProcess(TransportCatalogue&);

		// ���������� �������� �� ��������� ���������� �� ���������
		void StopInfoProcess(TransportCatalogue&, std::ostream&);

		// ���������� �������� �� ��������� ���������� �� ��������
		void RouteInfoProcess(TransportCatalogue&, std::ostream&);

		// ���������� �������� �� ��������� ���������� � ����� ������
		void StatInfoProcess(TransportCatalogue&, std::ostream&);

		// ���������� ������� ������� ��������
		void RequestsProcess(TransportCatalogue&, std::ostream&);

	private:
		
		// ������� ���� ����������� ��������
		std::deque<Request> _requests_history;
		// ��� ������� � ��������� ��������
		std::map<RequestType, std::deque<RequestPtr>> _current_requests = {};
		// ��������� ��������� ��� sub-�������� �� ���������� ���������
		std::map<std::string_view, std::map<std::string_view, int64_t>*> _current_distance_requests = {};
		// ������� ��� ������ ����������� �� ������
		//CatalogueReporter _printer;
	};

	namespace input_reader {

		namespace detail {

			// ������� ���� �� ����������-������������
			std::vector<std::string_view> SplitIntoWords(std::string_view, std::string_view);

			// ������� �������� �� ���������� ��������� - ���������
			void ParseDistances(RequestPtr, std::string_view);

			// ������� �������� �� ���������� ��������� - ����������
			size_t ParseCoordinates(RequestPtr, std::string_view);
		}

		// ������� �������� �� ���������� ���������
		void ParseStop(RequestPtr, std::string_view);

		// ������� �������� �� ���������� ��������
		void ParseRoute(RequestPtr, std::string_view);

		// ������� ��������� ��������
		void ParseAtribute(RequestPtr, std::string_view);

		// ������ ������� �������, ����������� �����
		void ParseRequest(RequestPtr);

		// ����� ����� � �������, ����� ��������������� ������
		// ������ ������� � ������ � RequestHander - �������� ���������� ��������
		void RequestProcessor(TransportCatalogue, std::istream&, std::ostream&);

		// ����� ����� � �������, ����������� ����� ������
		// ����� ������ ���������
		// ����� - ���������� �������� �� �������� ����������
		// ���������� ���������� �������� ���������� ��������
		// ����� - ���������� �������� �� ��������� ����������
		// ���������� ���������� �������� ���������� ��������
		// ������� ���� ���������� ��������
		// ������ ������� ��������� ��������
		void RequestProcessorSimple(TransportCatalogue, std::istream&, std::ostream&);
	}
}