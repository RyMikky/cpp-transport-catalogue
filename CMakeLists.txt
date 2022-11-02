cmake_minimum_required(VERSION 3.10)

project(PhoneBook CXX)
set(CMAKE_CXX_STANDARD 17)

find_package(Protobuf REQUIRED)
find_package(Threads REQUIRED)

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS transport_catalogue.proto map_renderer.proto svg.proto transport_router.proto graph.proto)

# альтернативный вариант "все в одном"
# для корректности сборки необходимо подключить transport_catalogue_full.pb.h в serialization.h
#protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS transport_catalogue_full.proto) 

set(MODULE_JSON_FILES json.h json.cpp json_reader.h json_reader.cpp json_builder.h json_builder.cpp)                           # 6 файлов - json_reader
set(MODULE_RENDERER_FILES geo.h geo.cpp svg.h svg.cpp map_renderer.h map_renderer.cpp)                                         # 6 файлов - рендер
set(MODULE_SERIALIZER_FILES serialization.h serialization.cpp)                                                                 # 2 файла - сериалайзер
set(MODULE_ROUTER_FILES ranges.h router.h graph.h transport_router.h transport_router.cpp)                                     # 5 файлов - роутер
set(MODULE_CATALOGUE_FILES transport_catalogue.h transport_catalogue.cpp transport_catalogue.proto)                            # 3 файла - каталог
set(MODULE_HANDLER_FILES request_handler.h request_handler.cpp)                                                                # 2 файла - обработчик
set(MODULE_TEST_FILES test_system.h test_system.cpp log_duration.h)                                                            # 3 файла - тестировщик
set(MODULE_COMMON_FILES domain.h domain.cpp main.cpp)                                                                          # 3 файла - прочие данные


add_executable(transport_catalogue ${PROTO_SRCS} ${PROTO_HDRS} ${MODULE_JSON_FILES} 
			${MODULE_RENDERER_FILES} ${MODULE_SERIALIZER_FILES} ${MODULE_ROUTER_FILES} 
			${MODULE_CATALOGUE_FILES} ${MODULE_HANDLER_FILES} ${MODULE_TEST_FILES} ${MODULE_COMMON_FILES})

target_include_directories(transport_catalogue PUBLIC ${Protobuf_INCLUDE_DIRS})
target_include_directories(transport_catalogue PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

string(REPLACE "protobuf.lib" "protobufd.lib" "Protobuf_LIBRARY_DEBUG" "${Protobuf_LIBRARY_DEBUG}")
string(REPLACE "protobuf.a" "protobufd.a" "Protobuf_LIBRARY_DEBUG" "${Protobuf_LIBRARY_DEBUG}")

target_link_libraries(transport_catalogue "$<IF:$<CONFIG:Debug>,${Protobuf_LIBRARY_DEBUG},${Protobuf_LIBRARY}>" Threads::Threads)