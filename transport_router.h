/////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                             //
//        class Transport Router                                                               //
//        Несамостоятельныый модуль-класс - роутер маршрутов.                                  //
//        Основной функционал - поиск и прокладка маршрута между заданными остановками.        //
//        Требования и зависимости:                                                            //
//           1. transport_catalogue.h - база маршщрутов и остановок для роутинга               //
//           2. router.h - сторонняя библиотека роутинга                                       //
//           3. C++17 (STL)                                                                    //
//                                                                                             //
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "router.h"                                                       // Дополнительная сторонняя библиотека
#include "transport_catalogue.h"                                          // Требует подключение к транспортному каталогу

#include <vector>
#include <memory>
#include <unordered_map>
#include <string_view>

inline const double METER_IN_KILO = 1000.0;                               // Количество меторв в одном километре
inline const double MIN_IN_HOUR = 60.0;                                   // Количество минут в одном часу

namespace transport_catalogue {

	namespace router {

		// Структура настрек роутера
		struct RouterSettings {

			RouterSettings() = default;
			RouterSettings(size_t, double);

			RouterSettings& SetBusWaitTime(size_t);                       // Задать время ожидания автобуса
			RouterSettings& SetBusVelocity(double);                       // Задать скорость движения автобуса
			
			size_t GetBusWaitTime() const;                                // Получить время ожидания автобуса
			double GetBusVelocity() const;                                // Получить скорость движения автобуса

			size_t _bus_wait_time = {};                                   // Время ожидания подхода автобуса
			double _bus_velocity = {};                                    // Скорость движения автобуса в км/ч
		};

		class TransportRouter {
		private:
			transport_catalogue::TransportCatalogue& _catalogue;
		public:
			TransportRouter() = default;
			TransportRouter(transport_catalogue::TransportCatalogue&);                                  // конструктор для вызова из обработчика запросов
			TransportRouter(transport_catalogue::TransportCatalogue&, const RouterSettings&);           // конструктор для вызова из обработчика запросов
			TransportRouter(transport_catalogue::TransportCatalogue&, RouterSettings&&);                // конструктор для вызова из обработчика запросов

			TransportRouter& SetRouterSettings(const RouterSettings&);                                  // загрузка настроек роутинга
			TransportRouter& SetRouterSettings(RouterSettings&&);                                       // загрузка настроек роутинга
			TransportRouter& SetRouterTransportCatalogue(transport_catalogue::TransportCatalogue&);     // переназначение транспортного каталога

			TransportRouter& ImportRoutingData();                                                       // загрузка данных для роутинга из каталога

			transport_catalogue::RouterData MakeRoute(std::string_view, std::string_view);              // проложить маршрут от точки А до точки Б

		private:
			RouterSettings _settings;

			graph::DirectedWeightedGraph<double> _graphs;                                               // блок со всеми остановками и маршрутами
			std::unique_ptr<graph::Router<double>> _router = nullptr;                                   // готовый роутер со всеми возможными маршрутами
			std::unordered_map<std::string_view, size_t> _wait_points;                                  // массив id остановок по названию для роутинга
			std::unordered_map<std::string_view, size_t> _move_points;                                  // массив id перегонов по названию для роутинга
		};

	} // namespace router

} // namespace transport_catalogue