#include "request_handler.h"

namespace transport_catalogue {

	namespace request_handler {

		// ------------------------- class RequestHandler ----------------------------

		RequestHandler::RequestHandler(transport_catalogue::TransportCatalogue& cat, std::istream& in, std::ostream& out)
			: _catalogue(cat), _input(in), _output(out) {
		}

		// ------------------------- class RequestHandler END ------------------------



		//// ------------------------- class RequestHandlerSimple ----------------------

		//RequestHandlerSimple::RequestHandlerSimple(
		//	transport_catalogue::TransportCatalogue& cat, std::istream& in,
		//	std::ostream& out) : RequestHandler(cat, in, out) {
		//	RequestProcessor();
		//};

		//// ---------------------- ���� ����������� ������� ------------------------------

		//// ������ ����� ������ � ������� � �������� ������ � ��� �������� ��������
		//SimpleRequestPtr RequestHandlerSimple::AddSimpleRequest(std::string line) {
		//	SimpleRequest& ptr = _simple_requests_history.emplace_back(std::move(line));
		//	return ptr.GetPtr();
		//}

		//// ��������� ������ �� ������� ���������
		//void RequestHandlerSimple::AddSimpleRequest(SimpleRequest&& request) {
		//	SimpleRequest& ptr = _simple_requests_history.emplace_back(std::move(request));
		//	_current_simple_requests[ptr._type].push_back(&ptr);
		//}

		//// ������ ������� �� ������ �������
		//Route RequestHandlerSimple::MakeRoute(SimpleRequestPtr request) {
		//	Route result;
		//	result._is_circular = request->_is_circular;
		//	result._route_name = request->_key_name;
		//	for (auto stop : request->_stops) {
		//		result._stops.push_back(_catalogue.GetStopByName(stop));
		//	}
		//	return result;
		//}

		//// ���������� �������� �� ���������� ��������� �� �������
		//void RequestHandlerSimple::AddDistanceProcess(SimpleRequestPtr request) {
		//	StopPtr this_stop_ = _catalogue.GetStopByName(request->_key_name);
		//	for (auto& dist : request->_distances) {
		//		_catalogue.AddDistance(this_stop_, _catalogue.GetStopByName(dist.first), dist.second);
		//	}
		//}

		//// ---------------------- ���� ���������������� ������� -------------------------

		//// ���������� �������� �� ���������� ��������� �� �������
		//void RequestHandlerSimple::AddDistanceProcess() {
		//	if (_current_distance_simple_requests.size() != 0) {
		//		for (auto this_stop : _current_distance_simple_requests) {
		//			StopPtr this_stop_ = _catalogue.GetStopByName(this_stop.first);
		//			for (auto& dist : *this_stop.second) {
		//				_catalogue.AddDistance(this_stop_, _catalogue.GetStopByName(dist.first), dist.second);
		//			}
		//		}
		//	}
		//	_current_distance_simple_requests.clear();
		//}

		//// ���������� �������� �� ���������� ���������
		//void RequestHandlerSimple::AddStopProcess() {
		//	if (_current_simple_requests[RequestType::add_stop].size() != 0) {
		//		for (SimpleRequestPtr request : _current_simple_requests.at(RequestType::add_stop)) {
		//			_catalogue.AddStop(std::move(MakeStop(request)));

		//			if (request->_distances.size() != 0) {
		//				_current_distance_simple_requests[request->_key_name] = &request->_distances;
		//				//AddDistanceProcessing(tc, request);
		//			}
		//		}
		//	}
		//}

		//// ���������� �������� �� ���������� ��������
		//void RequestHandlerSimple::AddRouteProcess() {
		//	// ���������� ������� ��������
		//	if (_current_simple_requests[RequestType::add_route].size() != 0) {
		//		for (SimpleRequestPtr request : _current_simple_requests.at(RequestType::add_route)) {
		//			_catalogue.AddRoute(std::move(MakeRoute(request)));
		//		}
		//	}
		//}

		//// ���������� �������� �� ��������� ���������� �� ���������
		//void RequestHandlerSimple::StopInfoProcess() {
		//	if (_current_simple_requests[RequestType::stop_info].size() != 0) {
		//		for (SimpleRequestPtr request : _current_simple_requests.at(RequestType::stop_info)) {
		//			auto report = _catalogue.GetBusesForStopInfo(request->_key_name);
		//			stat_reporter::StopInfoPrinter(_output, request->_key_name, report);
		//		}
		//	}
		//}

		//// ���������� �������� �� ��������� ���������� �� ��������
		//void RequestHandlerSimple::RouteInfoProcess() {
		//	if (_current_simple_requests[RequestType::route_info].size() != 0) {
		//		for (SimpleRequestPtr request : _current_simple_requests.at(RequestType::route_info)) {
		//			auto report = _catalogue.GetRouteInfo(request->_key_name);
		//			stat_reporter::RouteInfoPrinter(_output, request->_key_name, report);
		//		}
		//	}
		//}

		//// ���������� �������� �� ��������� ���������� � ����� ������
		//void RequestHandlerSimple::StatInfoProcess() {
		//	if (_current_simple_requests[RequestType::stat_info].size() != 0) {
		//		for (SimpleRequestPtr request : _current_simple_requests.at(RequestType::stat_info)) {
		//			if (request->_key == "Bus") {
		//				auto report = _catalogue.GetRouteInfo(request->_key_name);
		//				stat_reporter::RouteInfoPrinter(_output, request->_key_name, report);
		//			}
		//			else {
		//				auto report = _catalogue.GetBusesForStopInfo(request->_key_name);
		//				stat_reporter::StopInfoPrinter(_output, request->_key_name, report);
		//			}
		//		}
		//	}
		//}

		//// ���������� ������� ������� ��������
		//void RequestHandlerSimple::RequestsProcess() {

		//	AddStopProcess();                 // ��������� �������� �� ���������� ���������
		//	AddDistanceProcess();             // ��������� sub-�������� �� ���������� ���������
		//	AddRouteProcess();                // ��������� �������� �� ���������� ���������
		//	StatInfoProcess();                // ��������� �������� �� ����� ���������� ��� ���������� �� ����

		//	//StopInfoProcess();              // ��������� �������� �� ����� ���������� �� ���������
		//	//RouteInfoProcess();             // ��������� �������� �� ����� ���������� �� ��������

		//	_current_simple_requests.clear();
		//}

		//// ����� ����� � �������, ����������� ����� ������
		//// ����� ������ ������� ��������� input_reader
		//// ����� - ���������� �������� �� �������� ����������
		//// ���������� ���������� �������� ���������� ��������
		//// ����� - ���������� �������� �� ��������� ����������
		//// ���������� ���������� �������� ���������� ��������
		//// ������� ���� ���������� ��������
		//// ������ ������� ��������� ��������

		//void RequestHandlerSimple::RequestProcessor()
		//{
		//	try
		//	{
		//		std::string input_request_count_;
		//		std::cin >> input_request_count_;

		//		// ��������� �������� �� ���������� ����������
		//		for (int i = 0; i < std::stoi(input_request_count_); i++)
		//		{
		//			std::string key_word_;
		//			std::string request_line_;
		//			std::cin >> key_word_;

		//			std::getline(_input, request_line_);

		//			// ������� ����� ������ � ������� ��������
		//			SimpleRequestPtr request = AddSimpleRequest(key_word_ + request_line_);


		//			simple_reader::ParseRequest(request);             // ������ ������
		//			AddInProcessLine(request);                        // �������� � ������� ��������� �������� ����
		//			key_word_.clear(); request_line_.clear();         // ������� ������
		//		}

		//		std::string stat_request_count_;
		//		std::cin >> stat_request_count_;

		//		// ��������� �������� �� ��������� ����������
		//		for (int i = 0; i < std::stoi(stat_request_count_); i++)
		//		{
		//			std::string key_word_;
		//			std::string request_line_;
		//			std::cin >> key_word_;

		//			std::getline(_input, request_line_);

		//			// ������� ����� ������ � ������� ��������
		//			SimpleRequestPtr request = AddSimpleRequest(key_word_ + request_line_);

		//			simple_reader::ParseRequest(request); // ������ ������
		//			AddInProcessLine(request); // �������� � ������� ��������� �������� ����
		//			key_word_.clear(); request_line_.clear(); // ������� ������
		//		}
		//		RequestsProcess();
		//	}
		//	catch (const std::exception& e)
		//	{
		//		std::cerr << e.what() << std::endl << std::endl;
		//		std::cerr << "You must enter correct Request"sv << std::endl << std::endl;
		//		std::cerr << "Example:"sv << std::endl;
		//		std::cerr << "2"sv << std::endl;
		//		std::cerr << "Stop Tolstopaltsevo : 55.611087, 37.208290"sv << std::endl;
		//		std::cerr << "Stop Marushkino : 55.595884, 37.209755"sv << std::endl << std::endl;
		//		std::cerr << "Posible key_words:"sv << std::endl;
		//		std::cerr << "\"Bus\", \"Stop\", \"Print\""sv << std::endl << std::endl;
		//	}
		//}

		//// ------------------------- class RequestHandlerSimple END ------------------



		//// ------------------------- class RequestHandlerSimpleTerminal --------------

		//RequestHandlerSimpleTerminal::RequestHandlerSimpleTerminal(
		//	transport_catalogue::TransportCatalogue& cat, std::istream& in, 
		//	std::ostream& out) : RequestHandler(cat, in, out) {
		//	RequestProcessor();
		//};

		//// ---------------------- ���� ����������� ������� ------------------------------

		//// ������ ����� ������ � ������� � �������� ������ � ��� �������� ��������
		//SimpleRequestPtr RequestHandlerSimpleTerminal::AddSimpleRequest(std::string line) {
		//	SimpleRequest& ptr = _simple_requests_history.emplace_back(std::move(line));
		//	return ptr.GetPtr();
		//}

		//// ��������� ������ �� ������� ���������
		//void RequestHandlerSimpleTerminal::AddSimpleRequest(SimpleRequest&& request) {
		//	SimpleRequest& ptr = _simple_requests_history.emplace_back(std::move(request));
		//	_current_simple_requests[ptr._type].push_back(&ptr);
		//}

		//// ������ ������� �� ������ �������
		//Route RequestHandlerSimpleTerminal::MakeRoute(SimpleRequestPtr request) {
		//	Route result;
		//	result._is_circular = request->_is_circular;
		//	result._route_name = request->_key_name;
		//	for (auto stop : request->_stops) {
		//		result._stops.push_back(_catalogue.GetStopByName(stop));
		//	}
		//	return result;
		//}

		//// ���������� �������� �� ���������� ��������� �� �������
		//void RequestHandlerSimpleTerminal::AddDistanceProcess(SimpleRequestPtr request) {
		//	StopPtr this_stop_ = _catalogue.GetStopByName(request->_key_name);
		//	for (auto& dist : request->_distances) {
		//		_catalogue.AddDistance(this_stop_, _catalogue.GetStopByName(dist.first), dist.second);
		//	}
		//}

		//// ---------------------- ���� ���������������� ������� -------------------------

		//// ���������� �������� �� ���������� ��������� �� �������
		//void RequestHandlerSimpleTerminal::AddDistanceProcess() {
		//	if (_current_distance_simple_requests.size() != 0) {
		//		for (auto this_stop : _current_distance_simple_requests) {
		//			StopPtr this_stop_ = _catalogue.GetStopByName(this_stop.first);
		//			for (auto& dist : *this_stop.second) {
		//				_catalogue.AddDistance(this_stop_, _catalogue.GetStopByName(dist.first), dist.second);
		//			}
		//		}
		//	}
		//	_current_distance_simple_requests.clear();
		//}

		//// ���������� �������� �� ���������� ���������
		//void RequestHandlerSimpleTerminal::AddStopProcess() {
		//	if (_current_simple_requests[RequestType::add_stop].size() != 0) {
		//		for (SimpleRequestPtr request : _current_simple_requests.at(RequestType::add_stop)) {
		//			_catalogue.AddStop(std::move(MakeStop(request)));

		//			if (request->_distances.size() != 0) {
		//				_current_distance_simple_requests[request->_key_name] = &request->_distances;
		//				//AddDistanceProcessing(tc, request);
		//			}
		//		}
		//	}
		//}

		//// ���������� �������� �� ���������� ��������
		//void RequestHandlerSimpleTerminal::AddRouteProcess() {
		//	// ���������� ������� ��������
		//	if (_current_simple_requests[RequestType::add_route].size() != 0) {
		//		for (SimpleRequestPtr request : _current_simple_requests.at(RequestType::add_route)) {
		//			_catalogue.AddRoute(std::move(MakeRoute(request)));
		//		}
		//	}
		//}

		//// ���������� �������� �� ��������� ���������� �� ���������
		//void RequestHandlerSimpleTerminal::StopInfoProcess() {
		//	if (_current_simple_requests[RequestType::stop_info].size() != 0) {
		//		for (SimpleRequestPtr request : _current_simple_requests.at(RequestType::stop_info)) {
		//			auto report = _catalogue.GetBusesForStopInfo(request->_key_name);
		//			stat_reporter::StopInfoPrinter(_output, request->_key_name, report);
		//		}
		//	}
		//}

		//// ���������� �������� �� ��������� ���������� �� ��������
		//void RequestHandlerSimpleTerminal::RouteInfoProcess() {
		//	if (_current_simple_requests[RequestType::route_info].size() != 0) {
		//		for (SimpleRequestPtr request : _current_simple_requests.at(RequestType::route_info)) {
		//			auto report = _catalogue.GetRouteInfo(request->_key_name);
		//			stat_reporter::RouteInfoPrinter(_output, request->_key_name, report);
		//		}
		//	}
		//}

		//// ���������� �������� �� ��������� ���������� � ����� ������
		//void RequestHandlerSimpleTerminal::StatInfoProcess() {
		//	if (_current_simple_requests[RequestType::stat_info].size() != 0) {
		//		for (SimpleRequestPtr request : _current_simple_requests.at(RequestType::stat_info)) {
		//			if (request->_key == "Bus") {
		//				auto report = _catalogue.GetRouteInfo(request->_key_name);
		//				stat_reporter::RouteInfoPrinter(_output, request->_key_name, report);
		//			}
		//			else {
		//				auto report = _catalogue.GetBusesForStopInfo(request->_key_name);
		//				stat_reporter::StopInfoPrinter(_output, request->_key_name, report);
		//			}
		//		}
		//	}
		//}

		//// ���������� ������� ������� ��������
		//void RequestHandlerSimpleTerminal::RequestsProcess() {

		//	AddStopProcess();                 // ��������� �������� �� ���������� ���������
		//	AddDistanceProcess();             // ��������� sub-�������� �� ���������� ���������
		//	AddRouteProcess();                // ��������� �������� �� ���������� ���������
		//	StatInfoProcess();                // ��������� �������� �� ����� ���������� ��� ���������� �� ����

		//	//StopInfoProcess();              // ��������� �������� �� ����� ���������� �� ���������
		//	//RouteInfoProcess();             // ��������� �������� �� ����� ���������� �� ��������

		//	_current_simple_requests.clear();
		//}

		//// ����� ����� � �������, ����������� ����� ������
		//// ����� ������ ������� ��������� input_reader
		//// ����� - ���������� �������� �� �������� ����������
		//// ���������� ���������� �������� ���������� ��������
		//// ����� - ���������� �������� �� ��������� ����������
		//// ���������� ���������� �������� ���������� ��������
		//// ������� ���� ���������� ��������
		//// ������ ������� ��������� ��������

		//void RequestHandlerSimpleTerminal::RequestProcessor()
		//{
		//	bool InWork = true;

		//	while (InWork)
		//	{
		//		try
		//		{
		//			std::string request_count_;
		//			_input >> request_count_;

		//			if (request_count_ == "") {
		//				InWork = false;
		//			}

		//			// ����� �� ��������� ��� ����� "0"
		//			if (std::stoi(request_count_) == 0)
		//			{
		//				std::cerr << "Stop Processing" << std::endl;
		//				std::cerr << "If You want EXIT press \"y\" or press anykey" << std::endl;
		//				char choise; std::cin >> choise;

		//				if (choise == 'y') {
		//					InWork = false;
		//				}
		//				continue;
		//			}

		//			// �������� ���� ��������� �������� �� �������� ����������
		//			for (int i = 0; i < std::stoi(request_count_); i++)
		//			{
		//				std::string key_word_;
		//				std::string request_line_;
		//				std::cin >> key_word_;

		//				std::getline(_input, request_line_);

		//				SimpleRequestPtr request = AddSimpleRequest(key_word_ + request_line_);        // ������� ����� ������ � ������� ��������
		//				simple_reader::ParseRequest(request);                                          // ������ ������
		//				AddInProcessLine(request);                                                     // �������� � ������� ��������� �������� ����

		//				key_word_.clear(); request_line_.clear();                                      // ������� ������
		//			}


		//			RequestsProcess();
		//			request_count_.clear();
		//		}
		//		catch (const std::exception& e)
		//		{
		//			std::cerr << e.what() << std::endl << std::endl;
		//			std::cerr << "You must enter correct Request"sv << std::endl << std::endl;
		//			std::cerr << "Example:"sv << std::endl;
		//			std::cerr << "2"sv << std::endl;
		//			std::cerr << "Stop Tolstopaltsevo : 55.611087, 37.208290"sv << std::endl;
		//			std::cerr << "Stop Marushkino : 55.595884, 37.209755"sv << std::endl << std::endl;
		//			std::cerr << "Posible key_words:"sv << std::endl;
		//			std::cerr << "\"Bus\", \"Stop\", \"Print\""sv << std::endl << std::endl;
		//		}
		//	}
		//}

		//// ------------------------- class RequestHandlerSimpleTerminal END ----------

		

		// ------------------------- class RequestHandlerJSON ------------------------

		RequestHandlerJSON::RequestHandlerJSON(
			transport_catalogue::TransportCatalogue& cat, std::istream& in,
			std::ostream& out) : RequestHandler(cat, in, out)/*, _render(_map_stream)*/ {
			RequestProcessor();
		}

		// ���������� JSON-����� � ����
		json::Dict& RequestHandlerJSON::AddJSONlibary(json::Dict&& dict) {
			return _json_dicts.emplace_back(std::move(dict));
		}

		// ��������� ������ �� ������� ���������
		void RequestHandlerJSON::AddJSONRequest(JsonRequest&& request) {
			JsonRequest& ptr = _json_requests_history.emplace_back(std::move(request));
			_current_json_requests[ptr._type].push_back(&ptr);
		}

		// ������ ������� �� ������ �������
		Route RequestHandlerJSON::MakeRoute(JsonRequestPtr request) {
			Route result;
			result._is_circular = request->_is_circular;
			result._route_name = request->_name;
			for (auto stop : request->_stops) {
				result._stops.push_back(_catalogue.GetStopByName(stop));
			}
			return result;
		}

		// ������ ��������� �� ������ �������
		Stop RequestHandlerJSON::MakeStop(JsonRequestPtr request) {
			return { std::string(request->_name), request->_coord.lat, request->_coord.lng };
		}

		// ���������� �������� �������
		void RequestHandlerJSON::RenderSetupProcess(json::Dict settings) {
			map_renderer::RendererSettings _render_setup;
			for (auto& item : settings) {
				if (item.first == "width"s) 
				{
					_render_settings.SetWidth(item.second.AsDouble());
				} 
				else if (item.first == "height"s) 
				{
					_render_settings.SetHeight(item.second.AsDouble());
				}
				else if (item.first == "padding"s) 
				{
					_render_settings.SetPadding(item.second.AsDouble());
				}
				else if (item.first == "line_width"s) 
				{
					_render_settings.SetLineWidth(item.second.AsDouble());
				}
				else if (item.first == "stop_radius"s) 
				{
					_render_settings.SetStopRadius(item.second.AsDouble());
				}
				else if (item.first == "bus_label_font_size"s) 
				{
					_render_settings.SetBusLabelFont(static_cast<size_t>(item.second.AsInt()));
				}
				else if (item.first == "bus_label_offset"s)
				{
					map_renderer::LabelOffset offset(item.second.AsArray()[0].AsDouble(), item.second.AsArray()[1].AsDouble());
					_render_settings.SetBusLabelOffset(offset);
				}
				else if (item.first == "stop_label_font_size"s)
				{
					_render_settings.SetStopLabelFont(static_cast<size_t>(item.second.AsInt()));
				}
				else if (item.first == "stop_label_offset"s)
				{
					map_renderer::LabelOffset offset(item.second.AsArray()[0].AsDouble(), item.second.AsArray()[1].AsDouble());
					_render_settings.SetStopLabelOffset(offset);
				}
				else if (item.first == "underlayer_color"s)
				{
					if (item.second.IsString()) {
						_render_settings.SetUnderlayerColor(item.second.AsString());
					}
					else {
						auto array = item.second.AsArray();
						if (array.size() == 3) {
							_render_settings.SetUnderlayerColor(svg::Rgb(array[0].AsInt(), array[1].AsInt(), array[2].AsInt()));
						}
						else {
							_render_settings.SetUnderlayerColor(svg::Rgba(array[0].AsInt(), array[1].AsInt(), array[2].AsInt(), array[3].AsDouble()));
						}
					}
				}
				else if (item.first == "underlayer_width"s)
				{
					_render_settings.SetUnderlayerWidth(item.second.AsDouble());
				}
				else if (item.first == "color_palette"s)
				{
					if (item.second.IsArray()) {
						_render_settings.ResetColorPalette();
						for (auto& color : item.second.AsArray()) {
							if (color.IsString()) {
								_render_settings.AddColorInPalette(color.AsString());
							}
							else {
								auto array = color.AsArray();
								if (array.size() == 3) {
									_render_settings.AddColorInPalette(svg::Rgb(array[0].AsInt(), array[1].AsInt(), array[2].AsInt()));
								}
								else {
									_render_settings.AddColorInPalette(svg::Rgba(array[0].AsInt(), array[1].AsInt(), array[2].AsInt(), array[3].AsDouble()));
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
			//_render.SetRendererSettings(std::move(_render_setup));
		}

		//void RequestHandlerJSON::RenderProcess() {
		//	// ��������� ������ �� ��������
		//	auto data = _catalogue.GetRenderData();
		//	
		//	// �������� � ������� ������ ��� ���������
		//	for (auto& route : data) {
		//		_render.AddRenderData(route);
		//	}

		//	// ������������� ��������� � ����� ���������
		//	_render.CoordsProject();
		//	_render.RenderProcess();
		//}

		// ������ ������� �� ������� �� JSON
		void RequestHandlerJSON::RenderProcess(map_renderer::MapRenderer& _render) {
			// ��������� ������ �� ��������
			auto data = _catalogue.GetRenderData();

			// �������� � ������� ������ ��� ���������
			for (auto& route : data) {
				_render.AddRenderData(route);
			}

			// ������������� ��������� � ����� ���������
			_render.CoordsProject();
			_render.RenderProcess();
		}

		// ���������� �������� �� ���������� ��������� �� �������
		void RequestHandlerJSON::AddDistanceProcess(JsonRequestPtr request) {
			StopPtr this_stop_ = _catalogue.GetStopByName(request->_name);
			for (auto& dist : request->_distances) {
				_catalogue.AddDistance(this_stop_, _catalogue.GetStopByName(dist.first), dist.second);
			}
		}

		// ���������� �������� �� ���������� ��������� �� �������
		void RequestHandlerJSON::AddDistanceProcess() {
			if (_current_distance_json_requests.size() != 0) {
				for (auto this_stop : _current_distance_json_requests) {
					StopPtr this_stop_ = _catalogue.GetStopByName(this_stop.first);
					for (auto& dist : *this_stop.second) {
						_catalogue.AddDistance(this_stop_, _catalogue.GetStopByName(dist.first), dist.second);
					}
				}
			}
			_current_distance_json_requests.clear();
		}

		// ���������� �������� �� ���������� ���������
		void RequestHandlerJSON::AddStopProcess() {
			if (_current_json_requests[RequestType::add_stop].size() != 0) {
				for (JsonRequestPtr request : _current_json_requests.at(RequestType::add_stop)) {
					_catalogue.AddStop(std::move(MakeStop(request)));

					if (request->_distances.size() != 0) {
						_current_distance_json_requests[request->_name] = &request->_distances;
					}
				}
			}
		}

		// ���������� �������� �� ���������� ��������
		void RequestHandlerJSON::AddRouteProcess() {
			if (_current_json_requests[RequestType::add_route].size() != 0) {
				for (JsonRequestPtr request : _current_json_requests.at(RequestType::add_route)) {
					_catalogue.AddRoute(std::move(MakeRoute(request)));
				}
			}
		}

		// ���������� �������� �� ��������� ���������� �� ���������
		void RequestHandlerJSON::StopInfoProcess() {
			if (_current_json_requests[RequestType::stop_info].size() != 0) {
				json_reader::JSONPrinter printer;
				for (JsonRequestPtr request : _current_json_requests.at(RequestType::stop_info)) {
					auto report = _catalogue.GetBusesForStopInfo(request->_name);
					printer.AddDocument(printer.StopReportNodeMaker(request->_id, report).GetRoot());
				}
				printer.PrintResult(_output);
			}
		}

		// ���������� �������� �� ��������� ���������� �� ��������
		void RequestHandlerJSON::RouteInfoProcess() {
			if (_current_json_requests[RequestType::route_info].size() != 0) {
				json_reader::JSONPrinter printer;
				for (JsonRequestPtr request : _current_json_requests.at(RequestType::route_info)) {
					auto report = _catalogue.GetRouteInfo(request->_name);
					printer.AddDocument(printer.RouteReportNodeMaker(request->_id, report).GetRoot());
				}
				printer.PrintResult(_output);
			}
		}

		// ���������� �������� �� ��������� ���������� � ����� ������
		void RequestHandlerJSON::StatInfoProcess() {
			// ������� �������������� �� ������� ����������
			if (_current_json_requests[RequestType::stat_info].size() != 0) {
				// ��� ������ ���������� ������ �������
				json_reader::JSONPrinter printer;
				for (JsonRequestPtr request : _current_json_requests.at(RequestType::stat_info)) {
					// ������� �� ������ ���������� �� ���������
					if (request->_key == "Bus") {
						auto report = _catalogue.GetRouteInfo(request->_name);

						printer.AddDocument(printer.RouteReportNodeMaker(request->_id, report).GetRoot());
					}
					// ������� �� ������ ���������� �� ����������
					else if (request->_key == "Stop") {
						auto report = _catalogue.GetBusesForStopInfo(request->_name);

						printer.AddDocument(printer.StopReportNodeMaker(request->_id, report).GetRoot());
					}
					// ������� �� ��������� ����� ���������
					else {
						std::stringstream _render_output;
						map_renderer::MapRenderer _render(_render_output, _render_settings);
						RenderProcess(_render);

						printer.AddDocument(printer.MapRenderNodeMaker(request->_id, _render_output).GetRoot());
					}
				}
				printer.PrintResult(_output);
			}
		}

		// ���������� ������� ������� ��������
		void RequestHandlerJSON::RequestsProcess() {

			AddStopProcess();                 // ��������� �������� �� ���������� ���������
			AddDistanceProcess();             // ��������� sub-�������� �� ���������� ���������
			AddRouteProcess();                // ��������� �������� �� ���������� ���������
			//RenderProcess();                // ������ ������� � ��������������� ������
			StatInfoProcess();                // ��������� �������� �� ����� ���������� ��� ���������� �� ����

			//StopInfoProcess();              // ��������� �������� �� ����� ���������� �� ���������
			//RouteInfoProcess();             // ��������� �������� �� ����� ���������� �� ��������

			_current_json_requests.clear();   // ������� ������� ������� ���������
		}

		// ����� �����, ���������� �� ������������
		void RequestHandlerJSON::RequestProcessor()
		{
			try
			{
				json::Dict dict = json_reader::JSONReader(_input);
				auto request_ = AddJSONlibary(std::move(dict));

				// ������ ������� �� �������� ������ � ����
				if (request_.count("base_requests") != 0) { 

					auto base_requests_ = request_["base_requests"].AsArray();
					for (size_t i = 0; i != base_requests_.size(); i++) {
						const json::Dict node = base_requests_[i].AsMap();

						JsonRequest base_request_;
						json_reader::BaseRequestParser(&base_request_, node);
						AddJSONRequest(std::move(base_request_));
					}
				}

				// ������ ��������� ������� ������������
				if (request_.count("render_settings") != 0) {
					RenderSetupProcess(request_["render_settings"].AsMap());
				}

				// ������ ������� �� ��������� ������ �� ����
				if (request_.count("stat_requests") != 0) {

					auto stat_requests_ = request_["stat_requests"].AsArray();
					for (size_t i = 0; i != stat_requests_.size(); i++) {
						const json::Dict node = stat_requests_[i].AsMap();
						JsonRequest stat_request_;
						json_reader::StatRequestParser(&stat_request_, node);

						AddJSONRequest(std::move(stat_request_));
					}
				}

				RequestsProcess();
			}
			catch (const std::exception& e)
			{
				std::cerr << e.what() << std::endl << std::endl;
			}
		}
		
		// ------------------------- class RequestHandlerJSON END --------------------

	} // namespace request_handler

} // namespace transport_catalogue