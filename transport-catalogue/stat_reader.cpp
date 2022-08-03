#include "stat_reader.h"

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
			os << "Stop " << stop->name << ": buses";
			for (auto& bus : stop->buses) {
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
				if (stat->buses.size() == 0) {
					out << "Stop " << name << ": no buses" << std::endl;
				}
				else {
					out << stat;
				}
			}
		}

		// если задачи усложнятся то можно будет сделать класс-принтер
		/*void CatalogueReporter::RoutePrinter(std::ostream& out, std::string_view name, transport_catalogue::RouteStatPtr stat) {
			if (stat == nullptr) {
				out << "Bus " << name << ": not found" << std::endl;
			}
			else {
				out << stat;
			}
		}*/
	}
}