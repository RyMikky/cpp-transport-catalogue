/////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                             //
//        class Transport Сatalogue                                                            //
//        Самостоятельныый модуль-класс - основная библиотека остановок и маршрутов.           //
//        Основной функционал - аккумуляция информации по элементам, поиск по базе,            //
//        выдача требуемых данных с помощью методов-геттеров. API документирован ниже.         //
//        Требования и зависимости:                                                            //
//           1. domain.h - общая библиотека структур для работы программы                      //
//           2. C++17 (STL)                                                                    //
//                                                                                             //
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once    
#include "domain.h"              // модуль с общими структурами данных

#include <deque>
#include <map>
#include <vector>
#include <string>
#include <string_view>
#include <unordered_map>
#include <algorithm>

namespace transport_catalogue
{


	class TransportCatalogue
	{
		friend class RequestHandler;
	public:
		TransportCatalogue() = default;
		~TransportCatalogue() = default;
		
		// ---------------------------------------------- блок методов загрузки информации ------------------------------------------------------
		
		void RangeCalculate(Route&);                                                      // Расчёт длины маршрута
		void AddStopsProcess(Stop&&);                                                             // Добавить остановку
		void AddRoute(Route&&);                                                           // Добавить маршрут
		void AddDistance(StopPtr, StopPtr, size_t);                                       // Добавить расстояние между двумя остановками
		
		// ---------------------------------------------- блок методов получения информации -----------------------------------------------------
		
		size_t GetStopsCount() const;                                                     // Получить количество остановок в базе
		size_t GetDistance(StopPtr, StopPtr);                                             // Получить дистанцию в прямом или обратном направлении
		size_t GetDistanceSimple(StopPtr, StopPtr);                                       // Получить дистанцию в прямом направлении
		StopPtr GetStopByName(const std::string_view) const;                              // Возвращает указатель на остановку по ее имени
		RoutePtr GetRouteByName(const std::string_view) const;                            // Возвращает указатель на маршрут по его имени
		RouteStatPtr GetRouteInfo(const std::string_view) const;                          // Возвращает указатель на результат запроса о маршруте
		StopStatPtr GetBusesForStopInfo(const std::string_view) const;                    // Возвращает указатель на результат запроса о остановке
		std::map<std::string, RendererRequest> GetDataToRenderer();                       // Возвращает данные для рендеринга маршрутов
		const std::deque<StopPtr>& GetAllStopsData() const;                               // Возвращает дек указателей на остановки для роутера
		const std::deque<RoutePtr>& GetAllBusesData() const;                              // Возвращает дек указателей на маршруты для роутера

	private:
		
		std::deque<Stop> _all_stops_data;                                                 // основной массив данных по остановкам
		std::deque<StopPtr> _all_stops_to_router;                                         // массив указателей на остановки для роутера
		std::unordered_map<std::string_view, StopPtr> _all_stops_map;                     // мап указателей остановок для быстрого поиска по базе
		
		std::deque<Route> _all_buses_data;                                                // основной массив маршрутов
		std::deque<RoutePtr> _all_buses_to_router;                                        // массив указателей на маршруты для роутера
		std::unordered_map<std::string_view, RoutePtr> _all_buses_map;                    // мап указателей маршрутов для быстрого поиска по базе

		std::unordered_map<std::pair<StopPtr, StopPtr>, size_t, PairPointersHasher> _distances_map;         // расстояния между остановками
	
		std::string_view GetStopName(StopPtr);                                            // Возвращает string_view с именем остановки по указателю на экземпляр структуры Stop	
		std::string_view GetStopName(const Stop);                                         // Возвращает string_view с именем остановки для экземпляра структуры Stop
		
		std::string_view GetBusName(RoutePtr);                                            // Возвращает string_view с номером автобуса по указателю на экземпляр структуры Route
		std::string_view GetBusName(const Route);                                         // Возвращает string_view с номером автобуса для экземпляра структуры Route
	};

} // namespace transport_catalogue