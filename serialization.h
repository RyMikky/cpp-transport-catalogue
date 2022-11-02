/////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                             //
//        class Serial Handler                                                                 //
//        Несамостоятельныый модуль-класс - обрабочик сериализации.                            //
//        Основной функционал - взаимодействие с proto-сгенерированными файлами.               //
//        Требования и зависимости:                                                            //
//           1. transport_catalogue.h - база маршрутов и остановок для роутинга                //
//           2. transport_catalogue.proto - описание сериализированной базы                    //
//           3. transport_catalogue.pb.h - сгенерированная proto-библиотека                    //
//           4. transport_router.h - для работы с блоком настрек роутера                       //
//           5. map_renderer.h - для работы с блоком настрек рендера                           //
//           6. C++17 (STL)                                                                    //
//                                                                                             //
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "map_renderer.h"
#include "transport_router.h"
#include "transport_catalogue.h"

// Перед работой и сборкой необходимо выбрать тип подключения прото-файлов

#include <transport_catalogue.pb.h>                  // для сборки через CMake с разделением прото-файлов
//#include <transport_catalogue_full.pb.h>           // для сборки через CMake с одним прото-файлом

//#include "transport_catalogue.pb.h"                // для работы в Visual Studio с разделением прото-файлов
//#include "transport_catalogue_full.pb.h"           // для работы в Visual Studio с одним прото-файлом

#include <iostream>
#include <fstream>
#include <cassert>

namespace transport_catalogue {

	namespace serial_handler {

		class SerialHandler {
		private:
			transport_catalogue::TransportCatalogue& _transport_catalogue;                         // ссылка на базовый каталог
			transport_catalogue::router::RouterSettings& _router_settings;                         // ссылка на настройки роутера
			transport_catalogue::map_renderer::RendererSettings& _renderer_settings;               // ссылка на настройки рендера
			std::shared_ptr<router::TransportRouter> _transport_router = nullptr;                  // указатель на роутер для забора его данных

		public:

			SerialHandler() = delete;                                                              // без каталога нет смысла создавать сериалайзер
			SerialHandler(transport_catalogue::TransportCatalogue&
				, router::RouterSettings&, map_renderer::RendererSettings&);                       // конструктор для вызова из обработчика запросов

			// ------------------------------------------ блок сеттеров класса SerialHandler --------------------------------------------------------

			SerialHandler& SetTransporCatalogue(transport_catalogue::TransportCatalogue&);         // назначить транспортный каталог
			SerialHandler& SetRendererSettings(map_renderer::RendererSettings&);                   // назначить настройки рендера
			SerialHandler& SetRouterSettings(router::RouterSettings&);                             // назначить настройки роутера
			SerialHandler& SetTransportRouter(std::shared_ptr<router::TransportRouter>);           // назначить готовый роутер

			// ------------------------------------------ блок процессинга класса SerialHandler -----------------------------------------------------

			SerialHandler& GetDataFromCatalogue();                                                 // подготовить данные каталога для сериализации
			SerialHandler& GetDataFromRouter();                                                    // подготовить данные роутра для сериализации
			SerialHandler& ApplyDataToCatalogue();                                                 // загрузить данные в каталог
			SerialHandler& ApplyDataToRouter();                                                    // загрузить данные в роутер

			bool UploadDataToOstream(std::ostream&);                                               // выгрузить подготовленные данные в поток
			bool DownloadDataFromIstream(std::istream&);                                           // загрузить данные из потока

			// ------------------------------------------ возврат булевых флагов класса SerialHandler -----------------------------------------------

			bool IsCatalogueDataPrepearedToSerialize() const;                                      // возврашает флаг готовности данных каталога к выгрузке
			bool IsCatalogueDataIncomeFromOutside() const;                                         // возврашает флаг получения данных каталога и готовности к загрузке
			bool IsRouterDataPrepearedToSerialize() const;                                         // возврашает флаг готовности данных роутера к выгрузке
			bool IsRouterDataIncomeFromOutside() const;                                            // возврашает флаг получения данных роутера и готовности к загрузке

		private:
			bool _is_catalogue_data_prepeared_to_serialize = false;                                // данные получены из каталога и готовы к выгрзуке
			bool _is_catalogue_data_income_from_outside = false;                                   // данные получены извне и готовы к загрузке в каталог
			bool _is_router_data_prepeared_to_serialize = false;                                   // данные получены из роутера и готовы к выгрзуке
			bool _is_router_data_income_from_outside = false;                                      // данные получены извне и готовы к загрузке в роутер
			transport_catalogue_serialize::TransportCatalogue _serial_data;                        // базовая структура для сериализованных данных

			// ------------------------------------------ блок декодирования в Protobuf формат ------------------------------------------------------	

			void ToOutputColorDecode(
				const svg::Color&, transport_catalogue_serialize::Color*);                         // декодирование цвета по типу
			bool ToOutputGraphsDecode(transport_catalogue_serialize::RouterData*);                 // подготовка данных по графам роутера

			bool ToOutputStopsDataDecode();                                                        // подготовить данные по остановкам
			bool ToOutputBusesDataDecode();                                                        // подготовить данные по маршрутам
			bool ToOutputDistancesDataDecode();                                                    // подготовить данные по дистанциям
			bool ToOutputRendererSettingsDecode();                                                 // подготовить данные по настройкам рендера
			bool ToOutputRouterSettingsDecode();                                                   // подготовить данные по настройкам роутера
			bool ToOutputRouterDataDecode();                                                       // подготовить данные по базе роутера

			// ------------------------------------------ блок декодирования из Protobuf формата ----------------------------------------------------

			svg::Color ToCatalogueColorDecode(
				const transport_catalogue_serialize::Color&);                                      // декодирование цвета по типу
			bool ToCatalogueGraphsDecode(transport_catalogue_serialize::RouterData*);              // подготовка данных по графам роутера

			bool ToCatalogueStopsDataDecode();                                                     // загрузить данные по остановкам
			bool ToCatalogueBusesDataDecode();                                                     // загрузить данные по маршрутам
			bool ToCatalogueDistancesDataDecode();                                                 // загрузить данные по дистанциям
			bool ToCatalogueRendererSettingsDecode();                                              // загрузить данные по настройкам рендера
			bool ToCatalogueRouterSettingsDecode();                                                // загрузить данные по настройкам роутера
			bool ToCatalogueRouterDataDecode();                                                    // загрузить данные по базе роутера

		};

	} // namespace serial_handler

} // namespace transport_catalogue