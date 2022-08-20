#include "simple_reader.h"

namespace simple_reader {

	using namespace transport_catalogue;

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
		void ParseDistances(transport_catalogue::SimpleRequestPtr request, std::string_view line) {
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
		size_t ParseCoordinates(transport_catalogue::SimpleRequestPtr request, std::string_view line) {
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

	} // namespace detail

	// ������� �������� �� ���������� ���������
	void ParseStop(transport_catalogue::SimpleRequestPtr request, std::string_view line) {
		// ������ line - " 55.595884, 37.209755, 9900m to Rasskazovka, 100m to Marushkino"

		size_t pos_ = detail::ParseCoordinates(request, line);

		// TODO ����� ����� ������� ���������� �� ��� �������, �� ���� ������� ���, ��������� ��� ����� ������

		if (pos_ != 0) {
			line.remove_prefix(pos_);
			detail::ParseDistances(request, line);
		}
		request->_type = transport_catalogue::RequestType::add_stop;

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
		//request->_type = transport_catalogue::RequestType::add_stop;
	}

	// ������� �������� �� ���������� ��������
	void ParseRoute(transport_catalogue::SimpleRequestPtr request, std::string_view line) {
		std::string_view splitter_;
		if (line.find('>') != line.npos) {
			splitter_ = " > "sv;
		}
		else {
			request->_is_circular = false;
			splitter_ = " - "sv;
		}
		request->_stops = detail::SplitIntoWords(splitter_, line);
		request->_type = transport_catalogue::RequestType::add_route;
	}

	// ������� ��������� ��������
	void ParseAtribute(transport_catalogue::SimpleRequestPtr request, std::string_view line) {
		size_t pos = line.find(':');

		if (pos == line.npos) {
			// ������� �� ��������� ����������
			request->_key_name = line.substr(0, pos);
			request->_type = transport_catalogue::RequestType::stat_info;
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
	void ParseRequest(transport_catalogue::SimpleRequestPtr request)
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

} // namespace input_reader

namespace transport_catalogue {

	namespace stat_reporter {

		inline std::ostream& operator<<(std::ostream& os, const transport_catalogue::RouteStatPtr& unit) {
			os << "Bus " << unit->_name << ": " << unit->_stops_on_route << " stops on route, "
				<< unit->_unique_stops << " unique stops, " << unit->_real_route_length << " route length, "
				<< unit->_curvature << " curvature" << std::endl;

			return os;
		}

		void RouteInfoPrinter(std::ostream& out, std::string_view name, transport_catalogue::RouteStatPtr stat) {
			if (stat == nullptr) {
				out << "Bus " << name << ": not found" << std::endl;
			}
			else {
				out << stat;
			}
		}

		inline std::ostream& operator<<(std::ostream& os, const transport_catalogue::StopStatPtr& stop) {
			os << "Stop " << stop->_name << ": buses";
			for (auto& bus : stop->_buses) {
				os << " " << bus;
			}
			os << std::endl;

			return os;
		}

		void StopInfoPrinter(std::ostream& out, std::string_view name, transport_catalogue::StopStatPtr stat) {
			if (stat == nullptr) {
				out << "Stop " << name << ": not found" << std::endl;
			}
			else {
				if (stat->_buses.size() == 0) {
					out << "Stop " << name << ": no buses" << std::endl;
				}
				else {
					out << stat;
				}
			}
		}

		// ���� ������ ���������� �� ����� ����� ������� �����-�������
		/*void CatalogueReporter::RoutePrinter(std::ostream& out, std::string_view name, transport_catalogue::RouteStatPtr stat) {
			if (stat == nullptr) {
				out << "Bus " << name << ": not found" << std::endl;
			}
			else {
				out << stat;
			}
		}*/

	} // namespace stat_reporter

} // namespace transport_catalogue