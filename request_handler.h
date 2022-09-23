/////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                             //
//        class Request Handler                                                                //
//        ���������� ������-����� - ��������� ����� �������, ������ � �������������            //
//        �������� ���������� - ���������� �������� � ��������� ��������� ��������.            //
//        ���������� � �����������:                                                            //
//           1. domain.h - ����� ���������� �������� ��� ������ ���������                      //
//           2. transport_catalogue.h - ������ ������������� ��������                          //
//           3. transport_router.h - ������ ������� ���������                                  //
//           4. map_renderer.h - ������ ������������ ����                                      //
//           5. C++17 (STL)                                                                    //
//                                                                                             //
/////////////////////////////////////////////////////////////////////////////////////////////////


#pragma once
#include <iostream>

#include "transport_catalogue.h"                       // ������ ������������� ��������
#include "map_renderer.h"                              // ������ ������������
#include "transport_router.h"                          // ������ �������� ���������
#include "domain.h"                                    // ������ �������� ��������

using namespace std::literals;

namespace transport_catalogue {

	namespace request_handler {

		class RequestHandler {
		public:
			RequestHandler() = default;

			// ----------------------- ������� �������� �������, ������� � ����������� ---------------------------------------------

			RequestHandler& SetTransportCatalogue(transport_catalogue::TransportCatalogue&&);        // ��������� ��������� �������
			RequestHandler& SetRedererSettings(map_renderer::RendererSettings&&);                    // ��������� ��������� �������
			RequestHandler& SetRouterSettings(router::RouterSettings&&);                             // ��������� ��������� �������

			// ----------------------- ��������� ���������� ������� � ������� ------------------------------------------------------

			RequestHandler& EnableMapRenderer();                                     // ������������� � ��������� ��������� �������
			RequestHandler& EnableTransportRouter();                                 // ������������� � ��������� ��������� �������

			// ----------------------- ��������� �������� �� ���������� ���������� -------------------------------------------------

			RequestHandler& AddStop(JsonRequestPtr);                                 // �������� ��������� � �������
			RequestHandler& AddRoute(JsonRequestPtr);                                // �������� ������� � �������
			RequestHandler& AddDistance(JsonRequestPtr);                             // �������� ���������� � �������

			// ----------------------- �������� ��������� �������� �� ���������� ���������� ----------------------------------------

			RequestHandler& AddStopsProcess(const std::deque<JsonRequestPtr>&);      // �������� ���������� ��������� � �������
			RequestHandler& AddRoutesProcess(const std::deque<JsonRequestPtr>&);     // �������� ���������� ��������� � �������

			// ----------------------- ��������� ���������� �� ���� �� ������� -----------------------------------------------------
			
			RouteStatPtr GetRouteInfo(JsonRequestPtr);                               // �������� ���������� �� ��������
			StopStatPtr GetStopInfo(JsonRequestPtr);                                 // �������� ���������� �� ���������
			RouterData GetRouterData(JsonRequestPtr);                                // �������� ���������� ��������������
			RendererData GetRendererData(JsonRequestPtr);                            // �������� ����� ���������

		private:
			transport_catalogue::TransportCatalogue _catalogue;                      // ���������� �������
			std::shared_ptr<router::TransportRouter> _transport_router = nullptr;    // ���������� ������
			std::shared_ptr<map_renderer::MapRenderer> _map_renderer = nullptr;      // ���������� ������

			router::RouterSettings _router_settings;                                 // ���� �������� �������
			map_renderer::RendererSettings _renderer_settings;                       // ���� �������� �������

			// ��������� ��������� ��� sub-�������� �� ���������� ���������
			std::map<std::string, std::map<std::string, int64_t>*> _current_distance_json_requests = {};

			Route MakeRoute(JsonRequestPtr);                                         // ������ ������� �� ������ �������
			Stop MakeStop(JsonRequestPtr);                                           // ������ ��������� �� ������ �������

			RequestHandler& AddDistancesProcess();                                   // ���������� ��������� � �������
		};

	} // namespace request_handler

} // namespace transport_catalogue