﻿#include "transport_router.h"

namespace transport_catalogue {

	namespace router {

		// ------------------------------- struct RouterSettings -------------------------------

		RouterSettings::RouterSettings(size_t time, double velocity) 
			: _bus_wait_time(time), _bus_velocity(velocity), _IsDefault(false) {
		}

		// возвращает флаг типа настроек
		bool RouterSettings::IsDefault() const {
			return _IsDefault;
		}

		// Задать время ожидания автобуса
		RouterSettings& RouterSettings::SetBusWaitTime(size_t time) {
			_bus_wait_time = time;
			_IsDefault = false;
			return *this;
		}
		// Задать скорость движения автобуса
		RouterSettings& RouterSettings::SetBusVelocity(double velocity) {
			_bus_velocity = velocity;
			_IsDefault = false;
			return *this;
		}

		// Получить время ожидания автобуса
		size_t RouterSettings::GetBusWaitTime() const {
			return _bus_wait_time;
		}
		// Получить скорость движения автобуса
		double RouterSettings::GetBusVelocity() const {
			return _bus_velocity;
		}

		// ------------------------------- struct RouterSettings END ---------------------------



		// ------------------------------- class TransportRouter -------------------------------

		// конструктор для вызова из обработчика запросов
		TransportRouter::TransportRouter(transport_catalogue::TransportCatalogue& tc)
			: _transport_catalogue(tc), _graphs(tc.GetStopsCount() * 2) {
		}

		// конструктор для вызова из обработчика запросов
		TransportRouter::TransportRouter(transport_catalogue::TransportCatalogue& tc, const RouterSettings& settings) 
			: _transport_catalogue(tc), _settings(settings), _graphs(tc.GetStopsCount() * 2) {
		}

		// конструктор для вызова из обработчика запросов
		TransportRouter::TransportRouter(transport_catalogue::TransportCatalogue& tc, RouterSettings&& settings)
			: _transport_catalogue(tc), _settings(std::move(settings)), _graphs(tc.GetStopsCount() * 2) {
		}

		// загрузка настроек роутинга
		TransportRouter& TransportRouter::SetRouterSettings(const RouterSettings& settings) {
			_settings = settings;
			return *this;
		}

		// загрузка настроек роутинга
		TransportRouter& TransportRouter::SetRouterSettings(RouterSettings&& settings) {
			_settings = std::move(settings);
			return *this;
		}

		// загрузка настроек роутинга
		TransportRouter& TransportRouter::SetRouterTransportCatalogue(transport_catalogue::TransportCatalogue& tc) {
			_transport_catalogue = tc;
			return *this;
		}

		// загрузить данные по графам
		TransportRouter& TransportRouter::SetRouterGraphs(graph::DirectedWeightedGraph<double>&& graphs) {
			_graphs = std::move(graphs);
			return *this;
		}

		// загрузить данные точек ожидания роутера
		TransportRouter& TransportRouter::SetRouterWaitPoints(std::unordered_map<std::string_view, size_t>&& wait_points) {
			_wait_points = std::move(wait_points);
			return *this;
		}

		// загрузить данные точек перемещения роутера
		TransportRouter& TransportRouter::SetRouterMovePoints(std::unordered_map<std::string_view, size_t>&& move_points) {
			_move_points = std::move(move_points);
			return *this;
		}

		// установить флаг состояния данных
		TransportRouter& TransportRouter::SetRouterDataStatus(RouterDataStatus status) {
			_data_status = status;
			return *this;
		}

		// получить данные графов роутера
		const graph::DirectedWeightedGraph<double>& TransportRouter::GetRouterGraphs() const {
			return _graphs;
		}

		// получить данные точек ожидания роутера
		const std::unordered_map<std::string_view, size_t>& TransportRouter::GetRouterWaitPoints() const {
			return _wait_points;
		}

		// получить данные точек перемещения роутера
		const std::unordered_map<std::string_view, size_t>& TransportRouter::GetRouterMovePoints() const {
			return _move_points;
		}

		transport_catalogue::RouterData TransportRouter::MakeRoute(std::string_view from, std::string_view to) {
			// Проверяем наличие подготовленного роутера
			if (!_router) {
				ImportRoutingDataFromCatalogue();
			}

			transport_catalogue::RouterData result;
			// Проверяем наличие указанных остановок в базе роутера
			if (_wait_points.count(from) && _move_points.count(to)) 
			{
				auto data = _router.get()->BuildRoute(
					_wait_points.at(from), _move_points.at(to));

				if (data.has_value()) // Если маршрут проложен
				{
					result._is_founded = true;
					for (auto& item_id : data.value().edges) {
						const auto& edge = _graphs.GetEdge(item_id);
						result._route_items.push_back(transport_catalogue::RouterItem()
							.SetItemName(edge.GetEdgeName())
							.SetItemEdgeType(edge.GetEdgeType())
							.SetItemTime(edge.GetEdgeWeight())
							.SetItemSpanCount(edge.GetEdgeSpanCount()));

						result._route_time += edge.weight;
					}
				}
			}
			return result;
		}

		// Загрзка информации, самый тяжелый блок обработки
		TransportRouter& TransportRouter::ImportRoutingDataFromCatalogue() {
			
			size_t points_counter = 0;
			// Загружаем данные по остановкам
			for (const auto& stop : _transport_catalogue.GetAllStopsData()) {

				_wait_points.insert({ stop->_name, points_counter });
				_move_points.insert({ stop->_name, ++points_counter });
				
				_graphs.AddEdge(graph::Edge<double>()
					.SetEdgeType(graph::EdgeType::wait)
					.SetVertexFromId(_wait_points.at(stop->_name))
					.SetVertexToId(_move_points.at(stop->_name))
					.SetEdgeWeight(static_cast<double>(_settings.GetBusWaitTime()))
					.SetEdgeName(stop->_name)
					.SetEdgeSpanCount(0));
				++points_counter;
			}

			// Загружаем данные по маршрутам
			// Необходимо загрузить абсолютно все варианты по всем перегонам между любыми остановками
			for (const auto& bus : _transport_catalogue.GetAllBusesData()) {

				// движемся по маршруту от первой остановки
				for (size_t from_stop_id = 0; from_stop_id != bus->_stops.size(); ++from_stop_id) {

					int span_count = 0;

					// до каждой из последующих остановок
					for (size_t to_stop_id = from_stop_id + 1; to_stop_id != bus->_stops.size(); ++to_stop_id) {

						double route_distance = 0.0;

						// считаем расстояния по тем же точкам
						for (size_t current_point = from_stop_id + 1; current_point <= to_stop_id; ++current_point) {
							route_distance += static_cast<double>(_transport_catalogue
								.GetDistance(bus->_stops[current_point - 1], bus->_stops[current_point]));
						}

						_graphs.AddEdge(graph::Edge<double>()
							.SetEdgeType(graph::EdgeType::move)
							.SetVertexFromId(_move_points.at(bus->_stops[from_stop_id]->_name))
							.SetVertexToId(_wait_points.at(bus->_stops[to_stop_id]->_name))
							.SetEdgeWeight(route_distance / (_settings.GetBusVelocity() * METER_IN_KILO / MIN_IN_HOUR))
							.SetEdgeName(bus->_route_name)
							.SetEdgeSpanCount(++span_count));
					}

				}
				
			}
			_router = std::make_unique<graph::Router<double>>(_graphs);
			return *this;
		}


		// ------------------------------- class TransportRouter END ---------------------------



	} // namespace router


} // namespace transport_catalogue