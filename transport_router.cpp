#include "transport_router.h"

namespace transport_catalogue {

	namespace router {

		// ------------------------------- struct RouterSettings -------------------------------

		RouterSettings::RouterSettings(size_t time, double velocity) 
			: _bus_wait_time(time), _bus_velocity(velocity) {
		}

		// ������ ����� �������� ��������
		RouterSettings& RouterSettings::SetBusWaitTime(size_t time) {
			_bus_wait_time = time;
			return *this;
		}
		// ������ �������� �������� ��������
		RouterSettings& RouterSettings::SetBusVelocity(double velocity) {
			_bus_velocity = velocity;
			return *this;
		}

		// �������� ����� �������� ��������
		size_t RouterSettings::GetBusWaitTime() const {
			return _bus_wait_time;
		}
		// �������� �������� �������� ��������
		double RouterSettings::GetBusVelocity() const {
			return _bus_velocity;
		}

		// ------------------------------- struct RouterSettings END ---------------------------



		// ------------------------------- class TransportRouter -------------------------------

		// ����������� ��� ������ �� ����������� ��������
		TransportRouter::TransportRouter(transport_catalogue::TransportCatalogue& tc)
			: _catalogue(tc), _graphs(tc.GetStopsCount() * 2) {
		}

		// ����������� ��� ������ �� ����������� ��������
		TransportRouter::TransportRouter(transport_catalogue::TransportCatalogue& tc, const RouterSettings& settings) 
			: _catalogue(tc), _settings(settings), _graphs(tc.GetStopsCount() * 2) {
		}

		// ����������� ��� ������ �� ����������� ��������
		TransportRouter::TransportRouter(transport_catalogue::TransportCatalogue& tc, RouterSettings&& settings)
			: _catalogue(tc), _settings(std::move(settings)), _graphs(tc.GetStopsCount() * 2) {
		}

		// �������� �������� ��������
		TransportRouter& TransportRouter::SetRouterSettings(const RouterSettings& settings) {
			_settings = settings;
			return *this;
		}

		// �������� �������� ��������
		TransportRouter& TransportRouter::SetRouterSettings(RouterSettings&& settings) {
			_settings = std::move(settings);
			return *this;
		}

		// �������� �������� ��������
		TransportRouter& TransportRouter::SetRouterTransportCatalogue(transport_catalogue::TransportCatalogue& tc) {
			_catalogue = tc;
			return *this;
		}

		transport_catalogue::RouterData TransportRouter::MakeRoute(std::string_view from, std::string_view to) {
			// ��������� ������� ��������������� �������
			if (!_router) {
				ImportRoutingData();
			}

			transport_catalogue::RouterData result;
			// ��������� ������� ��������� ��������� � ���� �������
			if (_wait_points.count(from) && _move_points.count(to)) 
			{
				auto data = _router.get()->BuildRoute(
					_wait_points.at(from), _move_points.at(to));

				if (data.has_value()) // ���� ������� ��������
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

		// ������� ����������, ����� ������� ���� ���������
		TransportRouter& TransportRouter::ImportRoutingData() {
			
			size_t points_counter = 0;
			// ��������� ������ �� ����������
			for (const auto& stop : _catalogue.GetAllStopsData()) {

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

			// ��������� ������ �� ���������
			// ���������� ��������� ��������� ��� �������� �� ���� ��������� ����� ������ �����������
			for (const auto& bus : _catalogue.GetAllBusesData()) {

				// �������� �� �������� �� ������ ���������
				for (size_t from_stop_id = 0; from_stop_id != bus->_stops.size(); ++from_stop_id) {

					int span_count = 0;

					// �� ������ �� ����������� ���������
					for (size_t to_stop_id = from_stop_id + 1; to_stop_id != bus->_stops.size(); ++to_stop_id) {

						double route_distance = 0.0;

						// ������� ���������� �� ��� �� ������
						for (size_t current_point = from_stop_id + 1; current_point <= to_stop_id; ++current_point) {
							route_distance += static_cast<double>(_catalogue
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