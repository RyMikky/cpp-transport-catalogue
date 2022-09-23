/////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                             //
//        class Transport �atalogue                                                            //
//        ���������������� ������-����� - �������� ���������� ��������� � ���������.           //
//        �������� ���������� - ����������� ���������� �� ���������, ����� �� ����,            //
//        ������ ��������� ������ � ������� �������-��������. API �������������� ����.         //
//        ���������� � �����������:                                                            //
//           1. domain.h - ����� ���������� �������� ��� ������ ���������                      //
//           2. C++17 (STL)                                                                    //
//                                                                                             //
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once    
#include "domain.h"              // ������ � ������ ����������� ������

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
		
		// ---------------------------------------------- ���� ������� �������� ���������� ------------------------------------------------------
		
		void RangeCalculate(Route&);                                                      // ������ ����� ��������
		void AddStopsProcess(Stop&&);                                                             // �������� ���������
		void AddRoute(Route&&);                                                           // �������� �������
		void AddDistance(StopPtr, StopPtr, size_t);                                       // �������� ���������� ����� ����� �����������
		
		// ---------------------------------------------- ���� ������� ��������� ���������� -----------------------------------------------------
		
		size_t GetStopsCount() const;                                                     // �������� ���������� ��������� � ����
		size_t GetDistance(StopPtr, StopPtr);                                             // �������� ��������� � ������ ��� �������� �����������
		size_t GetDistanceSimple(StopPtr, StopPtr);                                       // �������� ��������� � ������ �����������
		StopPtr GetStopByName(const std::string_view) const;                              // ���������� ��������� �� ��������� �� �� �����
		RoutePtr GetRouteByName(const std::string_view) const;                            // ���������� ��������� �� ������� �� ��� �����
		RouteStatPtr GetRouteInfo(const std::string_view) const;                          // ���������� ��������� �� ��������� ������� � ��������
		StopStatPtr GetBusesForStopInfo(const std::string_view) const;                    // ���������� ��������� �� ��������� ������� � ���������
		std::map<std::string, RendererRequest> GetDataToRenderer();                       // ���������� ������ ��� ���������� ���������
		const std::deque<StopPtr>& GetAllStopsData() const;                               // ���������� ��� ���������� �� ��������� ��� �������
		const std::deque<RoutePtr>& GetAllBusesData() const;                              // ���������� ��� ���������� �� �������� ��� �������

	private:
		
		std::deque<Stop> _all_stops_data;                                                 // �������� ������ ������ �� ����������
		std::deque<StopPtr> _all_stops_to_router;                                         // ������ ���������� �� ��������� ��� �������
		std::unordered_map<std::string_view, StopPtr> _all_stops_map;                     // ��� ���������� ��������� ��� �������� ������ �� ����
		
		std::deque<Route> _all_buses_data;                                                // �������� ������ ���������
		std::deque<RoutePtr> _all_buses_to_router;                                        // ������ ���������� �� �������� ��� �������
		std::unordered_map<std::string_view, RoutePtr> _all_buses_map;                    // ��� ���������� ��������� ��� �������� ������ �� ����

		std::unordered_map<std::pair<StopPtr, StopPtr>, size_t, PairPointersHasher> _distances_map;         // ���������� ����� �����������
	
		std::string_view GetStopName(StopPtr);                                            // ���������� string_view � ������ ��������� �� ��������� �� ��������� ��������� Stop	
		std::string_view GetStopName(const Stop);                                         // ���������� string_view � ������ ��������� ��� ���������� ��������� Stop
		
		std::string_view GetBusName(RoutePtr);                                            // ���������� string_view � ������� �������� �� ��������� �� ��������� ��������� Route
		std::string_view GetBusName(const Route);                                         // ���������� string_view � ������� �������� ��� ���������� ��������� Route
	};

} // namespace transport_catalogue