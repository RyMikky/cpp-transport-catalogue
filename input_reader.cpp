#include "input_reader.h"

namespace transport_catalogue {

	// ������ ����� ������ � ������� � �������� ������ � ��� �������� ��������
	RequestPtr RequestHandler::AddRequest(std::string line) {
		Request& ptr = _requests_history.emplace_back(std::move(line));
		return ptr.GetPtr();
	}

	// ��������� ������ �� ������� ���������
	void RequestHandler::AddRequest(Request&& request) {
		Request& ptr = _requests_history.emplace_back(std::move(request));
		_current_requests[ptr._type].push_back(&ptr);
	}

	// ������ ������� �� ������ �������
	Route RequestHandler::MakeRoute(TransportCatalogue& tc, RequestPtr request) {
		Route result;
		result._is_circular = request->_is_circular;
		result._route_name = request->_key_name;
		for (auto stop : request->_stops) {
			result._stops.push_back(tc.GetStopByName(stop));
		}
		return result;
	}

	// ���������� �������� �� ���������� ��������� �� �������
	void RequestHandler::AddDistanceProcess(TransportCatalogue& tc) {
		if (_current_distance_requests.size() != 0) {
			for (auto this_stop : _current_distance_requests) {
				StopPtr this_stop_ = tc.GetStopByName(this_stop.first);
				for (auto& dist : *this_stop.second) {
					tc.AddDistance(this_stop_, tc.GetStopByName(dist.first), dist.second);
				}
			}
		}
		_current_distance_requests.clear();
	}

	// ���������� �������� �� ���������� ��������� �� �������
	void RequestHandler::AddDistanceProcess(TransportCatalogue& tc, RequestPtr request) {
		StopPtr this_stop_ = tc.GetStopByName(request->_key_name);
		for (auto& dist : request->_distances) {
			tc.AddDistance(this_stop_, tc.GetStopByName(dist.first), dist.second);
		}
	}

	// ���������� �������� �� ���������� ���������
	void RequestHandler::AddStopProcess(TransportCatalogue& tc) {
		if (_current_requests[RequestType::add_stop].size() != 0) {
			for (RequestPtr request : _current_requests.at(RequestType::add_stop)) {
				tc.AddStop(std::move(MakeStop(request)));

				if (request->_distances.size() != 0) {
					_current_distance_requests[request->_key_name] = &request->_distances;
					//AddDistanceProcessing(tc, request);
				}
			}
		}
	}

	// ���������� �������� �� ���������� ��������
	void RequestHandler::AddRouteProcess(TransportCatalogue& tc) {
		if (_current_requests[RequestType::add_route].size() != 0) {
			for (RequestPtr request : _current_requests.at(RequestType::add_route)) {
				tc.AddRoute(std::move(MakeRoute(tc, request)));
			}
		}
	}

	// ���������� �������� �� ��������� ���������� �� ���������
	void RequestHandler::StopInfoProcess(TransportCatalogue& tc, std::ostream& out) {
		if (_current_requests[RequestType::stop_info].size() != 0) {
			for (RequestPtr request : _current_requests.at(RequestType::stop_info)) {
				auto report = tc.GetBusesForStopInfo(request->_key_name);
				stat_reporter::StopInfoPrinter(out, request->_key_name, report);
			}
		}
	}

	// ���������� �������� �� ��������� ���������� �� ��������
	void RequestHandler::RouteInfoProcess(TransportCatalogue& tc, std::ostream& out) {
		if (_current_requests[RequestType::route_info].size() != 0) {
			for (RequestPtr request : _current_requests.at(RequestType::route_info)) {
				auto report = tc.GetRouteInfo(request->_key_name);
				stat_reporter::RouteInfoPrinter(out, request->_key_name, report);
			}
		}
	}

	// ���������� �������� �� ��������� ���������� � ����� ������
	void RequestHandler::StatInfoProcess(TransportCatalogue& tc, std::ostream& out) {
		if (_current_requests[RequestType::stat_info].size() != 0) {
			for (RequestPtr request : _current_requests.at(RequestType::stat_info)) {
				if (request->_key == "Bus") {
					auto report = tc.GetRouteInfo(request->_key_name);
					stat_reporter::RouteInfoPrinter(out, request->_key_name, report);
				}
				else {
					auto report = tc.GetBusesForStopInfo(request->_key_name);
					stat_reporter::StopInfoPrinter(out, request->_key_name, report);
				}
			}
		}
	}

	// ���������� ������� ������� ��������
	void RequestHandler::RequestsProcess(TransportCatalogue& tc, std::ostream& out) {

		AddStopProcess(tc);          // ��������� �������� �� ���������� ���������
		AddDistanceProcess(tc);      // ��������� sub-�������� �� ���������� ���������
		AddRouteProcess(tc);         // ��������� �������� �� ���������� ���������
		StatInfoProcess(tc, out);    // ��������� �������� �� ����� ���������� ��� ���������� �� ����

		//StopInfoProcess(tc, out);    // ��������� �������� �� ����� ���������� �� ���������
		//RouteInfoProcess(tc, out);   // ��������� �������� �� ����� ���������� �� ��������

		_current_requests.clear();
	}

	namespace input_reader {

		namespace detail {

			// ������� ���� �� ����������-������������
			std::vector<std::string_view> SplitIntoWords(std::string_view splitter, std::string_view line) {
				std::vector<std::string_view> result;

				line.remove_prefix(std::min(line.find_first_not_of(" "), line.size()));
				const int64_t pos_end = line.npos;
				while (line.size() != 0) {

					int64_t next_splitter_ = line.find(splitter);
					if (next_splitter_ == pos_end) {
						result.push_back(line.substr(0));
						line.remove_prefix(line.size());
					}
					else {
						result.push_back(line.substr(0, next_splitter_));
						line.remove_prefix(next_splitter_ + 2);
					}

					line.remove_prefix(std::min(line.find_first_not_of(" "), line.size()));
				}

				return result;
			}

			// ������� �������� �� ���������� ��������� - ���������
			void ParseDistances(RequestPtr request, std::string_view line) {
				size_t pos_ = 0;
				int64_t distance_ = 0;
				while (line.size() != 0)
				{
					line.remove_prefix(std::min(line.find_first_not_of(" "), line.size()));

					pos_ = line.find_first_of('m');
					distance_ = std::stoi(std::string(line.substr(0, pos_)));
					pos_ = line.find_first_of(" to ");
					line.remove_prefix(pos_ + 4);

					pos_ = line.find_first_of(',');

					if (pos_ == line.npos) {
						request->_distances[line.substr(0)] = distance_;
						line.remove_prefix(line.size());
					}
					else {
						request->_distances[line.substr(0, pos_)] = distance_;
						line.remove_prefix(pos_ + 1);
					}
					pos_ = 0; distance_ = 0;
				}
			}

			// ������� �������� �� ���������� ��������� - ����������
			size_t ParseCoordinates(RequestPtr request, std::string_view line) {
				size_t coord_end_pos_ = 0;
				geo::Coordinates coord_;
				size_t pos_ = line.find_first_not_of(" ");
				coord_end_pos_ += pos_;
				line.remove_prefix(std::min(pos_, line.size()));
				pos_ = line.find_first_of(',');
				coord_end_pos_ += pos_ + 1;
				coord_.lat = std::stod(std::string(line.substr(0, pos_)));
				line.remove_prefix(pos_ + 1);

				pos_ = line.find_first_of(',');
				if (pos_ == line.npos) {
					coord_.lng = std::stod(std::string(line));
					line.remove_prefix(line.size());
					coord_end_pos_ = 0;
				}
				else {
					coord_.lng = std::stod(std::string(line.substr(0, pos_)));
					line.remove_prefix(pos_ + 1);
					coord_end_pos_ += pos_ + 1;
				}
				request->_coord = std::move(coord_);
				return coord_end_pos_;
			}

		}

		// ������� �������� �� ���������� ���������
		void ParseStop(RequestPtr request, std::string_view line) {
			// ������ line - " 55.595884, 37.209755, 9900m to Rasskazovka, 100m to Marushkino"

			size_t pos_ = detail::ParseCoordinates(request, line);

			// TODO ����� ����� ������� ���������� �� ��� �������, �� ���� ������� ���, ��������� ��� ����� ������

			if (pos_ != 0) {
				line.remove_prefix(pos_);
				detail::ParseDistances(request, line);
			}
			request->_type = RequestType::add_stop;

			//geo::Coordinates coord_;
			//line.remove_prefix(std::min(line.find_first_not_of(" "), line.size()));
			//size_t pos_ = line.find_first_of(',');
			//coord_.lat = std::stod(std::string(line.substr(0, pos_)));
			//line.remove_prefix(pos_ + 1);

			//pos_ = line.find_first_of(',');
			//if (pos_ == line.npos) {
			//	coord_.lng = std::stod(std::string(line));
			//}
			//else {
			//	coord_.lng = std::stod(std::string(line.substr(0, pos_)));
			//	line.remove_prefix(pos_ + 1);

			//	// �������� ���������� �� ���������
			//	ParseDistances(request, line);
			//}
			//request->_coord = std::move(coord_);
			//request->_type = RequestType::add_stop;
		}

		// ������� �������� �� ���������� ��������
		void ParseRoute(RequestPtr request, std::string_view line) {
			std::string_view splitter_;
			if (line.find('>') != line.npos) {
				splitter_ = " > "sv;
			}
			else {
				request->_is_circular = false;
				splitter_ = " - "sv;
			}
			request->_stops = detail::SplitIntoWords(splitter_, line);
			request->_type = RequestType::add_route;
		}

		// ������� ��������� ��������
		void ParseAtribute(RequestPtr request, std::string_view line) {
			size_t pos = line.find(':');

			if (pos == line.npos) {
				// ������� �� ��������� ����������
				request->_key_name = line.substr(0, pos);
				request->_type = RequestType::stat_info;
			}
			else {

				request->_key_name = line.substr(0, pos);
				line.remove_prefix(pos + 1);

				// ������ �������� �� ���� �����
				if (request->_key == "Bus") {
					// ������� �� ���������� ��������
					ParseRoute(request, line);
				}
				else {
					// ������� �� ���������� ���������
					ParseStop(request, line);
				}
			}
		}

		// ������ ������� �������, ����������� �����
		void ParseRequest(RequestPtr request)
		{
			std::string_view line = request->_input_line;
			line.remove_prefix(std::min(line.find_first_not_of(" "), line.size()));

			// ���������� ���� �������
			size_t pos = line.find_first_of(' ');
			request->_key = line.substr(0, pos);

			line.remove_prefix(pos);
			line.remove_prefix(std::min(line.find_first_not_of(" "), line.size()));

			// ������ ��������
			ParseAtribute(request, line);
		}

		// ����� ����� � �������, ����� ��������������� ������
		// ������ ������� � ������ � RequestHander - �������� ���������� ��������
		void RequestProcessor(TransportCatalogue tc, std::istream& input, std::ostream& output)
		{
			bool InWork = true;
			RequestHandler request_handler_;

			while (InWork)
			{
				try
				{
					std::string request_count_;
					std::cin >> request_count_;

					if (request_count_ == "") {
						InWork = false;
					}

					// ����� �� ��������� ��� ����� "0"
					if (std::stoi(request_count_) == 0)
					{
						std::cerr << "Stop Processing" << std::endl;
						std::cerr << "If You want EXIT press \"y\" or press anykey" << std::endl;
						char choise; std::cin >> choise;

						if (choise == 'y') {
							InWork = false;
						}
						continue;
					}

					// �������� ���� ��������� �������� �� �������� ����������
					for (int i = 0; i < std::stoi(request_count_); i++)
					{
						std::string key_word_;
						std::string request_line_;
						std::cin >> key_word_;

						std::getline(input, request_line_);

						// ������� ����� ������ � ������� ��������
						RequestPtr request = request_handler_.AddRequest(key_word_ + request_line_);
						//RequestPtr request = request_handler_.AddRequest(request_line_);
						ParseRequest(request); // ������ ������
						request_handler_.AddInProcessLine(request); // �������� � ������� ��������� �������� ����

						key_word_.clear(); request_line_.clear(); // ������� ������
					}


					request_handler_.RequestsProcess(tc, output);
					request_count_.clear();
				}
				catch (const std::exception& e)
				{
					std::cerr << e.what() << std::endl << std::endl;
					std::cerr << "You must enter correct Request"sv << std::endl << std::endl;
					std::cerr << "Example:"sv << std::endl;
					std::cerr << "2"sv << std::endl;
					std::cerr << "Stop Tolstopaltsevo : 55.611087, 37.208290"sv << std::endl;
					std::cerr << "Stop Marushkino : 55.595884, 37.209755"sv << std::endl << std::endl;
					std::cerr << "Posible key_words:"sv << std::endl;
					std::cerr << "\"Bus\", \"Stop\", \"Print\""sv << std::endl << std::endl;
				}
			}
		}

		// ����� ����� � �������, ����������� ����� ������
		// ����� ������ ���������
		// ����� - ���������� �������� �� �������� ����������
		// ���������� ���������� �������� ���������� ��������
		// ����� - ���������� �������� �� ��������� ����������
		// ���������� ���������� �������� ���������� ��������
		// ������� ���� ���������� ��������
		// ������ ������� ��������� ��������
		void RequestProcessorSimple(TransportCatalogue tc, std::istream& input, std::ostream& output)
		{
			RequestHandler request_handler_;
			try
			{
				std::string input_request_count_;
				std::cin >> input_request_count_;

				// ��������� �������� �� ���������� ����������
				for (int i = 0; i < std::stoi(input_request_count_); i++)
				{
					std::string key_word_;
					std::string request_line_;
					std::cin >> key_word_;

					std::getline(input, request_line_);

					// ������� ����� ������ � ������� ��������
					RequestPtr request = request_handler_.AddRequest(key_word_ + request_line_);

					ParseRequest(request); // ������ ������
					request_handler_.AddInProcessLine(request); // �������� � ������� ��������� �������� ����
					key_word_.clear(); request_line_.clear(); // ������� ������
				}

				std::string stat_request_count_;
				std::cin >> stat_request_count_;

				// ��������� �������� �� ��������� ����������
				for (int i = 0; i < std::stoi(stat_request_count_); i++)
				{
					std::string key_word_;
					std::string request_line_;
					std::cin >> key_word_;

					std::getline(input, request_line_);

					// ������� ����� ������ � ������� ��������
					RequestPtr request = request_handler_.AddRequest(key_word_ + request_line_);

					ParseRequest(request); // ������ ������
					request_handler_.AddInProcessLine(request); // �������� � ������� ��������� �������� ����
					key_word_.clear(); request_line_.clear(); // ������� ������
				}
				request_handler_.RequestsProcess(tc, output);
			}
			catch (const std::exception& e)
			{
				std::cerr << e.what() << std::endl << std::endl;
				std::cerr << "You must enter correct Request"sv << std::endl << std::endl;
				std::cerr << "Example:"sv << std::endl;
				std::cerr << "2"sv << std::endl;
				std::cerr << "Stop Tolstopaltsevo : 55.611087, 37.208290"sv << std::endl;
				std::cerr << "Stop Marushkino : 55.595884, 37.209755"sv << std::endl << std::endl;
				std::cerr << "Posible key_words:"sv << std::endl;
				std::cerr << "\"Bus\", \"Stop\", \"Print\""sv << std::endl << std::endl;
			}
		}
	}
}