# С++ Transport Catalogue
Финальный проект: транспортный справочник
Программная реализация транспортного каталога - визуализатора маршрутов. Работает с JSON запросами.

# Общая информация о работе системы

1. Ключи запуска приложения
	1.1 "make_base" - создание базы данных каталога и запись в файл
	1.2 "make_base_vor" - создание базы данных без инициализации роутера
	1.3 "process_requests" - загрузка данных из внешней базы и получение результатов
	1.4 "simple_mode" - стандартный режим работы с единым JSON-блоком
	1.5 "test_mode" - запуск тестирования всех четырех режимов
		"process_requests" тестируется автоматически в режимах создания базы

# Системные требования
- C++ 17 (STL)

# Технологии
- Protobuf
- JSON
- SVG

# Дополнительная информация по сборке

1. Ручная сборка и подключение proto-файлов в проект Visual Studio 
	(адреса до предварительно собранной и заведомо рабочей библиотеки ProtoBuf вставить по факту)

	1.1 Генерация pb.h и pb.cc
	- открыть командную строку в папке проекта
	- выполнить команду "../Libraries/Protobuf/bin/protoc.exe --cpp_out=. proto_files/transport_catalogue_full.proto"
	ИЛИ
	- выполнить команду "../Libraries/Protobuf/bin/protoc.exe --cpp_out=. proto_files/transport_catalogue.proto 
		proto_files/map_renderer.proto proto_files/svg.proto proto_files/transport_router.proto proto_files/graph.proto"

	1.2 Настройки компилятора
	- подключить папку "../Libraries/Protobuf/include"
	- в соответствии с режимом Debug или Release выбрать библитеку времени выполнения "/MTd" или "/MT" соответственно
	
	1.3 Настройка компоновщика
	- подключить папку "../Libraries/Protobuf/lib"
	- в соответствии с режимом Debug или Release прописать зависимость "libprotobufd.lib" или "libprotobuf.lib" соответственно

	1.4 Подключить синтезированные pb.h и pb.cc файлы к проекту

	1.5 В хеддере serialization.h - подключить "transport_catalogue.pb.h" или "transport_catalogue_full.pb.h" через кавычки

	1.5 Пользоваться
