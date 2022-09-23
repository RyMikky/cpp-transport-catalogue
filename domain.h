#pragma once
#include "geo.h"
#include "graph.h"

#include <string>
#include <string_view>
#include <set>
#include <map>
#include <vector>

using namespace std::literals;

namespace transport_catalogue {

	// каталог базовых структур для работы основной системы

	struct Stop;
	struct Route;

	using StopPtr = const Stop*;
	using RoutePtr = const Route*;

	// Остановка
	struct Stop
	{
		Stop() = default;
		Stop(const std::string_view, const double, const double);
		Stop(StopPtr);

		std::string _name;                   // Название остановки
		geo::Coordinates _coords{ 0L,0L };   // Координаты

		bool operator==(const Stop&) const;
	};

	// Хешер для останровок
	class StopHasher {
	public:
		std::size_t operator()(const StopPtr) const noexcept;
	private:
		std::hash<std::string> _hasher;
	};

	// Структура ответа на запрос о остановке
	struct StopStat
	{
		explicit StopStat(std::string_view, std::set<std::string_view>&);
		std::string_view _name;
		std::set<std::string_view> _buses;
	};

	using StopStatPtr = const StopStat*;

	// Хешер для pair<StopPtr, StopPtr>
	class PairPointersHasher
	{
	public:
		std::size_t operator()(const std::pair<StopPtr, StopPtr>) const noexcept;
	private:
		std::hash<const void*> _hasher;
	};

	// Маршрут
	struct Route {

		Route() = default;
		Route(RoutePtr);

		std::string _route_name;              // номер/название маршрута 
		std::vector<StopPtr> _stops;          // массив указателей на остановки маршрута
		size_t _unique_stops_qty = 0U;        // уникальные остановки
		double _geo_route_length = 0L;        // длина маршрута (геометрическое расстояние)
		size_t _real_route_length = 0U;       // действительная длина маршрута в метрах
		double _curvature = 1L;               // отклонение действительного расстояния от геометрического
		bool _is_circular = false;            // тип маршрута

		bool operator==(const Route&) const;
	};

	// Хешер для маршрутов
	class RouteHasher {
	public:
		std::size_t operator()(const RoutePtr) const noexcept;
	private:
		std::hash<std::string> _hasher;
	};

	// Структура ответа на инфо по маршруту
	struct RouteStat
	{

		explicit RouteStat(size_t, size_t, size_t, double, std::string_view);
		size_t _stops_on_route = 0U;           // количество остановок на маршруте
		size_t _unique_stops = 0U;             // количество уникальных остановок
		size_t _real_route_length = 0U;        // действительная длина маршрута
		double _curvature = 1L;                // отклонение маршрута
		std::string _name;                     // номер/имя маршрута
	};

	using RouteStatPtr = const RouteStat*;

	// блок структур запросов

	enum RequestType {
		null_req = 0,
		add_stop, add_route, add_dist, stop_info, route_info, stat_info, map_render, print_history, routing

		// stat_info - общий тип запроса на получение информации по маршруту или остановке
		// нужен по условию задачи, так как все запросы на получение информации должны 
		// обрабатываться в порядке поступления, а не по очереди как с было у меня

		// add_dist - пока не использован, так как добавление дистанций оформленно как
		// sub-запрос к добавлению остановки, если поле _distances имеет данные

		// print_history - пока не использован, так как система в статическом режиме
		// имеет смысл при онлайн работе с возможностью конвейерной обработки запросов
		// которые задаются в случайном порядке
	};

	// структура запроса для работы c терминалом
	struct SimpleRequest;
	using SimpleRequestPtr = SimpleRequest*;

	// структура запроса для работы с json
	struct JsonRequest;
	using JsonRequestPtr = JsonRequest*;

	// запрос на получение данных для рендера
	struct RendererRequest;
	using RendererDataPtr = RendererRequest*;

	// структура запроса для работы с терминалом
	struct SimpleRequest {
		SimpleRequest() = default;

		explicit SimpleRequest(std::string&& line);

		SimpleRequestPtr GetPtr();

		size_t _id = 0;                                          // id запроса
		RequestType _type = RequestType::null_req;               // тип запроса
		std::string_view _key = ""sv;                            // ключ запроса (Bus, Stop, Map, Route)
		std::string_view _name = ""sv;                           // имя запроса (маршрута, остановки)
		std::string_view _from = ""sv;                           // начало пути для запроса Route
		std::string_view _to = ""sv;                             // конечная пути для запроса Route
		geo::Coordinates _coord = { 0L, 0L };                    // координаты, если остановка
		std::vector<std::string_view> _stops = {};               // остановки, если маршрут
		std::map<std::string_view, int64_t> _distances = {};     // расстояния до остановок
		bool _is_circular = true;                                // тип маршрута
		std::string _input_line = ""s;                           // входная строка
	};

	// структура запроса для работы с json
	struct JsonRequest {
		JsonRequest() = default;

		JsonRequestPtr GetPtr();

		size_t _id = 0;                                          // id запроса
		RequestType _type = RequestType::null_req;               // тип запроса
		std::string _key = ""s;                                  // ключ запроса (Bus, Stop, Map, Route)
		std::string _name = ""s;                                 // имя запроса (маршрута, остановки)
		std::string _from = ""s;                                 // начало пути для запроса Route
		std::string _to = ""s;                                   // конечная пути для запроса Route
		geo::Coordinates _coord = { 0L, 0L };                    // координаты, если остановка
		std::vector<std::string> _stops = {};                    // остановки, если маршрут
		std::map<std::string, int64_t> _distances = {};          // расстояния до остановок
		bool _is_circular = true;                                // тип маршрута

	};

	// запрос на обновление параметров рендера
	struct RendererRequest {
		RendererRequest() = default;
		RendererRequest(bool, std::vector<StopPtr>);

		bool _is_circular = false;
		std::vector<StopPtr> _stops;
	};

	// Структура ответа рендера в виде строки
	struct RendererData {
		std::string _svg_line = {};
	};

	// Структура участка построенного маршрута 
	struct RouterItem
	{
		RouterItem& SetItemName(std::string_view);                    // Задать название участка маршрута
		RouterItem& SetItemSpanCount(int);                            // Задать количество преодаленных остановок на участке маршруте
		RouterItem& SetItemTime(double);                              // Задать время прохождения участка маршрута
		RouterItem& SetItemEdgeType(graph::EdgeType);                 // Задать тип участка маршрута

		std::string_view GetItemName() const;                         // Получить название участка маршрута
		int GetItemSpanCount() const;                                 // Получить количество преодаленных остановок на участке маршруте
		double GetItemTime() const;                                   // Получить время прохождения участка маршрута
		graph::EdgeType GetItemType() const;                          // Получить тип участка маршрута

		std::string _item_name = {};                                  // Название участка маршрута
		int _span_count = 0;                                          // Количество преодаленных остановок на участке маршруте
		double _time = 0.0;                                           // Время прохождения участка маршрута
		graph::EdgeType _type;                                        // Тип участка маршрута
	};
	// Структура построенного маршрута
	struct RouterData
	{
		double _route_time = 0.0;                                     // Время в пути на маршруте
		std::vector<RouterItem> _route_items;                         // Участки маршрута
		bool _is_founded = false;                                     // Статус маршрута
	};

} // namespace transport_catalogue