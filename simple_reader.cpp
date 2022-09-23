#include "simple_reader.h"

namespace simple_reader {

	using namespace transport_catalogue;

	namespace detail {

		// парсинг слов со сплиттером-разделителем
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

		// парсинг запросов на добавление остановки - дистанции
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

		// парсинг запросов на добавление остановки - координаты
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

	// парсинг запросов на добавление остановки
	void ParseStop(transport_catalogue::SimpleRequestPtr request, std::string_view line) {
		// пример line - " 55.595884, 37.209755, 9900m to Rasskazovka, 100m to Marushkino"

		size_t pos_ = detail::ParseCoordinates(request, line);

		// TODO здесь можно сделать разделение на два запроса, но пока оставим так, посмотрим что будет дальше

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

		//	// добираем расстояния до остановок
		//	ParseDistances(request, line);
		//}
		//request->_coord = std::move(coord_);
		//request->_type = transport_catalogue::RequestType::add_stop;
	}

	// парсинг запросов на добавление маршрута
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

	// парсинг атрибутов запросов
	void ParseAtribute(transport_catalogue::SimpleRequestPtr request, std::string_view line) {
		size_t pos = line.find(':');

		if (pos == line.npos) {
			// запросы на получение информации
			request->_name = line.substr(0, pos);
			request->_type = transport_catalogue::RequestType::stat_info;
		}
		else {

			request->_name = line.substr(0, pos);
			line.remove_prefix(pos + 1);

			// парсим атрибуты по типу ключа
			if (request->_key == "Bus") {
				// запросы на добавление маршрута
				ParseRoute(request, line);
			}
			else {
				// запросы на добавление остановки
				ParseStop(request, line);
			}
		}
	}

	// начало пасинга запроса, определение ключа
	void ParseRequest(transport_catalogue::SimpleRequestPtr request)
	{
		std::string_view line = request->_input_line;
		line.remove_prefix(std::min(line.find_first_not_of(" "), line.size()));

		// определяем ключ запроса
		size_t pos = line.find_first_of(' ');
		request->_key = line.substr(0, pos);

		line.remove_prefix(pos);
		line.remove_prefix(std::min(line.find_first_not_of(" "), line.size()));

		// парсим атрибуты
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

		// если задачи усложнятся то можно будет сделать класс-принтер
		/*void CatalogueReporter::RoutePrinter(std::ostream& out, std::string_view name, transport_catalogue::RouteStatPtr stat) {
			if (stat == nullptr) {
				out << "Bus " << name << ": not found" << std::endl;
			}
			else {
				out << stat;
			}
		}*/

	} // namespace stat_reporter


	namespace simple_reader {

		SimpleReader::SimpleReader(std::istream& input, std::ostream& output)
			: _input(input), _output(output) {
		}

		SimpleReader& SimpleReader::run_terminal_process() {
			while (InProcess)
			{
				transport_catalogue::JsonRequest request;
				std::string command = ""s; std::string command_line = ""s;
				_input >> command;

				if (command == "EXIT" || command == "exit") {
					_output << "ОСТАНОВКА РАБОТЫ" << std::endl;
					_output << "Если Вы желаете прекратить работу программы нажмите \"y\"" << std::endl; 

					_output << "Для продолжения нажмите любую клавишу" << std::endl;
						
					char choise; std::cin >> choise;
					if (choise == 'y') {
						InProcess = false;
					}
				}

				if (command == "CLEAR" || command == "clear") {
					system("cls");
				}

				if (command == "HELP" || command == "COMMAND_LIST"
					|| command == "help" || command == "command_list") {
					CommandLineViewer();
				}

				if (command == "ADD_STOP" || command == "add_stop") {
					std::getline(_input, command_line);
					AddStopParser(&request, command_line);
					_handler.AddStop(&(_requests_history.emplace_back(request)));
				}

				if (command == "ADD_ROUTE" || command == "add_route") {
					std::getline(_input, command_line);
					AddRouteParser(&request, command_line);
					_handler.AddRoute(&(_requests_history.emplace_back(request)));
				}

				if (command == "ADD_DISTANCE" || command == "add_distance") {
					std::getline(_input, command_line);
					AddDistanceParser(&request, command_line);
					_handler.AddDistance(&(_requests_history.emplace_back(request)));
				}
				
			}
	
			return *this;
		}

		void SimpleReader::AddStopParser(transport_catalogue::JsonRequestPtr request, const std::string& input) {
			request->_type = transport_catalogue::add_stop;
			request->_key = "Stop";
			std::string_view line = input; 
			line.remove_prefix(std::min(line.find_first_of('{') + 1, line.size()));

			// записываем название остановки
			size_t pos = line.find_first_of('}');
			request->_name = line.substr(0, pos);
			line.remove_prefix(std::min(line.find_first_of('{') + 1, line.size()));

			// записываем широту
			pos = line.find_first_of('}');
			request->_coord.lat = std::stod(std::string(line.substr(0, pos)));
			line.remove_prefix(std::min(line.find_first_of('{') + 1, line.size()));

			// записываем долготу
			pos = line.find_first_of('}');
			request->_coord.lng = std::stod(std::string(line.substr(0, pos)));
			line.remove_prefix(std::min(line.find_first_of('{'), line.size()));

			// записываем расстояния если они есть
			while (line.size() != 0) {
				if (line.find_first_of('{') > line.size()) {
					break;
				}
				line.remove_prefix(std::min(line.find_first_of('{') + 1, line.size()));
				std::string stop_name = ""; int range = 0;	

				pos = line.find_first_of(" : ");
				stop_name = line.substr(0, pos);
				line.remove_prefix(std::min(pos + 3, line.size()));

				pos = line.find_first_of('}');
				range = std::stoi(std::string(line.substr(0, pos)));
				line.remove_prefix(std::min(line.find_first_of('{'), line.size()));

				request->_distances.insert({ stop_name, range });
			}
		}

		void SimpleReader::AddRouteParser(transport_catalogue::JsonRequestPtr request, const std::string& input) {

		}

		void SimpleReader::AddDistanceParser(transport_catalogue::JsonRequestPtr request, const std::string& input) {

		}

		SimpleReader& SimpleReader::CommandLineViewer() {
			_output << "СПИСОК КОМАНД (в произвольном регистре)"sv << std::endl;
			_output << "КОМАНДЫ ЗАГРУЗКИ ИНФОРМАЦИИ:"sv << std::endl << std::endl;

			_output << "  ADD_STOP - Добавить информацию по остановке."sv << std::endl;
			_output << "  ADD_STOP {stop_name} : {latitude}, {longitude}"sv << std::endl;
			_output << "  ADD_STOP {stop_name} : {latitude}, {longitude}, {stop1_name : distance}, {stopN_name : distance}"sv << std::endl;
			_output << "    {stop_name} - название остановки"sv << std::endl;
			_output << "    {latitude} - географическая широта остановки"sv << std::endl;
			_output << "    {longitude} - географическая долгота остановки"sv << std::endl;
			_output << "    {stopN_name : distance} - название другой остановки и расстояние до неё в метрах"sv << std::endl << std::endl;


			_output << "  ADD_ROUTE - Добавить информацию по маршруту."sv << std::endl;
			_output << "  ADD_ROUTE {route_name} : {circular or linear}, {stop1_name}, {stop2_name}, {stopN_name}"sv << std::endl;
			_output << "    {route_name} - название маршрута"sv << std::endl;
			_output << "    {circular} - кольцевой тип машрута"sv << std::endl;
			_output << "    {linear} - линейный тип машрута"sv << std::endl;
			_output << "    {stop1_name} - название остановки маршрута"sv << std::endl << std::endl;

			_output << "  ADD_DISTANCE - Добавить растояние между остановками (A-B)"sv << std::endl;
			_output << "  ADD_DISTANCE {stop_name} : {stop1_name : distance}, {stop2_name : distance}, {stopN_name : distance}"sv << std::endl;
			_output << "    {stop_name} - название остановки (A)"sv << std::endl;
			_output << "    {stopN_name : distance} - название другой остановки (B) и расстояние до неё в метрах"sv << std::endl << std::endl;

			_output << "КОМАНДЫ ПОЛУЧЕНИЯ ИНФОРМАЦИИ:"sv << std::endl << std::endl;

			_output << "  STOP_INFO - Получить информацию по остановке"sv << std::endl;
			_output << "  STOP_INFO {stop_name}"sv << std::endl;
			_output << "    {stop_name} - название остановки"sv << std::endl << std::endl;

			_output << "  BUS_INFO - Получить информацию по маршруту"sv << std::endl;
			_output << "  BUS_INFO {route_name}"sv << std::endl;
			_output << "    {route_name} - название маршрута"sv << std::endl << std::endl;

			_output << "  ROUTE_INFO - Получить информацию по маршруту"sv << std::endl;
			_output << "  ROUTE_INFO {route_name}"sv << std::endl;
			_output << "    {route_name} - название маршрута"sv << std::endl << std::endl;

			_output << "  MAKE_ROUTE - Проложить маршрут между остановками"sv << std::endl;
			_output << "  MAKE_ROUTE {stop_name_A}, {stop_name_В}"sv << std::endl;
			_output << "    {route_name_A} - остановка отправления"sv << std::endl;
			_output << "    {route_name_В} - остановка прибытия"sv << std::endl << std::endl;

			_output << "  GET_MAP - Построить карту маршрутов в SVG-формате"sv << std::endl;
			_output << "  GET_MAP {file_name} - Построить карту маршрутов и сохранить в файл"sv << std::endl << std::endl;

			_output << "  MAKE_MAP - то же, что и GET_MAP"sv << std::endl;
			_output << "  ROAD_MAP - то же, что и GET_MAP"sv << std::endl << std::endl;

			_output << "СПРАВОЧНЫЕ И ВСПОМОГАТЕЛЬНЫЕ КОМАНДЫ:"sv << std::endl << std::endl;

			_output << "  COMMAND_LIST - Получить список команд"sv << std::endl;
			_output << "  CLEAR - Очистить экран консоли"sv << std::endl;
			_output << "  HELP - Получить список команд"sv << std::endl;
			_output << "  EXIT - Выход из терминала"sv << std::endl;

			return *this;
		}

	} // namespace simple_reader 

} // namespace transport_catalogue