#pragma once    
#include "domain.h"

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

	// Основная программа
	class TransportCatalogue
	{
		friend class RequestHandler;
	public:
		TransportCatalogue() = default;
		~TransportCatalogue() = default;
		
		// ---------------------------------------------- блок методов загрузки информации ------------------------------------------------------
		
		void RangeCalculate(Route&);                                                      // Расчёт длины маршрута
		void AddStop(Stop&&);                                                             // Добавить остановку
		void AddRoute(Route&&);                                                           // Добавить маршрут
		void AddDistance(StopPtr, StopPtr, size_t);                                       // Добавить расстояние между двумя остановками
		
		// ---------------------------------------------- блок методов получения информации -----------------------------------------------------
		
		size_t GetDistance(StopPtr, StopPtr);                                             // Получить дистпнцию в прямом или обратном направлении
		size_t GetDistanceSimple(StopPtr, StopPtr);                                       // Получить дистанцию в прямом направлении
		StopPtr GetStopByName(const std::string_view) const;                              // Возвращает указатель на остановку по ее имени
		RoutePtr GetRouteByName(const std::string_view) const;                            // Возвращает указатель на маршрут по его имени
		RouteStatPtr GetRouteInfo(const std::string_view) const;                          // Возвращает указатель на результат запроса о маршруте
		StopStatPtr GetBusesForStopInfo(const std::string_view) const;                    // Возвращает указатель на результат запроса о остановке
		std::map<std::string, RendererData> GetRendererData();                            // Возвращает данные для рендеринга маршрутов

	private:
		
		std::deque<Stop> _all_stops_data;                                                 // основной массив данных по остановкам
		std::unordered_map<std::string_view, StopPtr> _all_stops_map;                     // мап указателей остановок
		
		std::deque<Route> _all_buses_data;                                                // основной массив маршрутов
		std::unordered_map<std::string_view, RoutePtr> _all_buses_map;                    // мап указателей маршрутов

		std::unordered_map<std::pair<StopPtr, StopPtr>, size_t, PairPointersHasher> _distances_map;         // расстояния между остановками
	
		std::string_view GetStopName(StopPtr);                                            // Возвращает string_view с именем остановки по указателю на экземпляр структуры Stop	
		std::string_view GetStopName(const Stop);                                         // Возвращает string_view с именем остановки для экземпляра структуры Stop
		
		std::string_view GetBusName(RoutePtr);                                            // Возвращает string_view с номером автобуса по указателю на экземпляр структуры Route
		std::string_view GetBusName(const Route);                                         // Возвращает string_view с номером автобуса для экземпляра структуры Route
	};

}