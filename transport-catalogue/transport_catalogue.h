#pragma once
#include "geo.h"           

#include <deque>
#include <map>             
#include <vector>
#include <string>
#include <string_view>
#include <set>
#include <ostream>         
#include <unordered_set>
#include <unordered_map>
#include <algorithm>       

namespace transport_catalogue
{

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
		std::hash<std::string> hasher_;
	};

	// Структура ответа на запрос о остановке
	struct StopStat
	{
		explicit StopStat(std::string_view, std::set<std::string_view>&);
		std::string_view name;
		std::set<std::string_view> buses;
	};

	using StopStatPtr = const StopStat*;

	// Хешер для pair<StopPtr, StopPtr>
	class PairPointersHasher
	{
	public:
		std::size_t operator()(const std::pair<StopPtr, StopPtr>) const noexcept;
	private:
		std::hash<const void*> hasher_;
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
		std::hash<std::string> hasher_;
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


	// Основная программа
	class TransportCatalogue
	{
		friend class RequestHandler;
	public:
		TransportCatalogue() = default;
		~TransportCatalogue() = default;
		
		// Расчёт длины маршрута
		void RangeCalculate(Route&);
		
		// Добавить остановку
		void AddStop(Stop&&);
		// Добавить маршрут
		void AddRoute(Route&&);
		// Добавить расстояние между двумя остановками
		void AddDistance(StopPtr, StopPtr, size_t);
		
		// Получить дистпнцию в прямом или обратном направлении
		size_t GetDistance(StopPtr, StopPtr);

		// Получить дистанцию в прямом направлении
		size_t GetDistanceSimple(StopPtr, StopPtr); 

		// Возвращает указатель на остановку по ее имени
		StopPtr GetStopByName(const std::string_view) const;

		// Возвращает указатель на маршрут по его имени
		RoutePtr GetRouteByName(const std::string_view) const;

		// Возвращает указатель на результат запроса о маршруте
		RouteStatPtr GetRouteInfo(const std::string_view) const;

		// Возвращает указатель на результат запроса о остановке
		StopStatPtr GetBusesForStopInfo(const std::string_view) const;

	private:
		// основной массив данных по остановкам
		std::deque<Stop> _all_stops_data;
		// мап указателей остановок
		std::unordered_map<std::string_view, StopPtr> _all_stops_map;
		// основной массив маршрутов
		std::deque<Route> _all_buses_data;
		// мап указателей маршрутов
		std::unordered_map<std::string_view, RoutePtr> _all_buses_map;
		// расстояния между остановками
		std::unordered_map<std::pair<StopPtr, StopPtr>, size_t, PairPointersHasher> _distances_map;    

		// Возвращает string_view с именем остановки по указателю на экземпляр структуры Stop
		std::string_view GetStopName(StopPtr);
		// Возвращает string_view с именем остановки для экземпляра структуры Stop
		std::string_view GetStopName(const Stop);

		// Возвращает string_view с номером автобуса по указателю на экземпляр структуры Route
		std::string_view GetBusName(RoutePtr);
		// Возвращает string_view с номером автобуса для экземпляра структуры Route
		std::string_view GetBusName(const Route);
	};

}