/////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                             //
//        class Transport Router                                                               //
//        ������������������ ������-����� - ������ ���������.                                  //
//        �������� ���������� - ����� � ��������� �������� ����� ��������� �����������.        //
//        ���������� � �����������:                                                            //
//           1. transport_catalogue.h - ���� ���������� � ��������� ��� ��������               //
//           2. router.h - ��������� ���������� ��������                                       //
//           3. C++17 (STL)                                                                    //
//                                                                                             //
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "router.h"                                                       // �������������� ��������� ����������
#include "transport_catalogue.h"                                          // ������� ����������� � ������������� ��������

#include <vector>
#include <memory>
#include <unordered_map>
#include <string_view>

inline const double METER_IN_KILO = 1000.0;                               // ���������� ������ � ����� ���������
inline const double MIN_IN_HOUR = 60.0;                                   // ���������� ����� � ����� ����

namespace transport_catalogue {

	namespace router {

		// ��������� ������� �������
		struct RouterSettings {

			RouterSettings() = default;
			RouterSettings(size_t, double);

			RouterSettings& SetBusWaitTime(size_t);                       // ������ ����� �������� ��������
			RouterSettings& SetBusVelocity(double);                       // ������ �������� �������� ��������
			
			size_t GetBusWaitTime() const;                                // �������� ����� �������� ��������
			double GetBusVelocity() const;                                // �������� �������� �������� ��������

			size_t _bus_wait_time = {};                                   // ����� �������� ������� ��������
			double _bus_velocity = {};                                    // �������� �������� �������� � ��/�
		};

		class TransportRouter {
		private:
			transport_catalogue::TransportCatalogue& _catalogue;
		public:
			TransportRouter() = default;
			TransportRouter(transport_catalogue::TransportCatalogue&);                                  // ����������� ��� ������ �� ����������� ��������
			TransportRouter(transport_catalogue::TransportCatalogue&, const RouterSettings&);           // ����������� ��� ������ �� ����������� ��������
			TransportRouter(transport_catalogue::TransportCatalogue&, RouterSettings&&);                // ����������� ��� ������ �� ����������� ��������

			TransportRouter& SetRouterSettings(const RouterSettings&);                                  // �������� �������� ��������
			TransportRouter& SetRouterSettings(RouterSettings&&);                                       // �������� �������� ��������
			TransportRouter& SetRouterTransportCatalogue(transport_catalogue::TransportCatalogue&);     // �������������� ������������� ��������

			TransportRouter& ImportRoutingData();                                                       // �������� ������ ��� �������� �� ��������

			transport_catalogue::RouterData MakeRoute(std::string_view, std::string_view);              // ��������� ������� �� ����� � �� ����� �

		private:
			RouterSettings _settings;

			graph::DirectedWeightedGraph<double> _graphs;                                               // ���� �� ����� ����������� � ����������
			std::unique_ptr<graph::Router<double>> _router = nullptr;                                   // ������� ������ �� ����� ���������� ����������
			std::unordered_map<std::string_view, size_t> _wait_points;                                  // ������ id ��������� �� �������� ��� ��������
			std::unordered_map<std::string_view, size_t> _move_points;                                  // ������ id ��������� �� �������� ��� ��������
		};

	} // namespace router

} // namespace transport_catalogue