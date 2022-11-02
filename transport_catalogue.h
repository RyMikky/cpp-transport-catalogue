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
	using DistancesMap = std::unordered_map<std::pair<StopPtr, StopPtr>, size_t, PairPointersHasher>;

	enum CatalogueDataStatus {
		EMPTY, READY, UPLOADED, DOWNLOADED
	};

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
		void AddRouteFromSerializer(Route&&);                                             // Добавить маршрут из блока сериализации
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
		const DistancesMap& GetAllDistancesData() const;                                  // Возвращает мап указателей на остановки и расстояния между ними
		CatalogueDataStatus GetCatalogueDataStatus() const;                               // Возвращает флаг состояния данных каталога

		// ---------------------------------------------- блок прочих методов работы класса -----------------------------------------------------

		TransportCatalogue& SetDataStatus(CatalogueDataStatus);                           // Назначение флага состояния данных
		bool ClearAllData();                                                              // Очищает все поля класса;

	private:
		
		std::deque<Stop> _all_stops_data;                                                 // Основной массив данных по остановкам
		std::deque<StopPtr> _all_stops_to_router;                                         // Массив указателей на остановки для роутера
		std::unordered_map<std::string_view, StopPtr> _all_stops_map;                     // Мап указателей остановок для быстрого поиска по базе
		
		std::deque<Route> _all_buses_data;                                                // Основной массив маршрутов
		std::deque<RoutePtr> _all_buses_to_router;                                        // Массив указателей на маршруты для роутера
		std::unordered_map<std::string_view, RoutePtr> _all_buses_map;                    // Мап указателей маршрутов для быстрого поиска по базе

		DistancesMap _all_distances_map;                                                  // Мап расстояния между остановками

		CatalogueDataStatus _data_status = EMPTY;                                         // Начальное состояние данных - пусто
	
		std::string_view GetStopName(StopPtr);                                            // Возвращает string_view с именем остановки по указателю на экземпляр структуры Stop	
		std::string_view GetStopName(const Stop);                                         // Возвращает string_view с именем остановки для экземпляра структуры Stop
		
		std::string_view GetBusName(RoutePtr);                                            // Возвращает string_view с номером автобуса по указателю на экземпляр структуры Route
		std::string_view GetBusName(const Route);                                         // Возвращает string_view с номером автобуса для экземпляра структуры Route
	};

} // namespace transport_catalogue