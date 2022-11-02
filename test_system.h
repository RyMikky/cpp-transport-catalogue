/////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                             //
//        class TestExecutor                                                                   //
//        Несамостоятельныый модуль-класс - автоматизированая система тестов.                  //
//        Выявление ошибок в программе методом сравнения результата и верного ответа.          //
//        Все требуемые файлы лежат с папке "test_json" во вложенных папках.                   //
//        Можно добавить еще тест-файлы главное записать их в DOC_LIB.                         //
//                                                                                             //
//        class MakeBaseTest                                                                   //
//        Класс-наследник TestExecutor - создание базы данных каталога и роутера.              //
//        Выполнение "process_request" происходит автоматически для сравнения резульатов.      //
//                                                                                             //
//        class MakeBaseVORTest                                                                //
//        Класс-наследник TestExecutor - создание базы данных каталога БЕЗ роутера.            //
//        Выполнение "process_request" происходит автоматически для сравнения резульатов.      //
//                                                                                             //
//        class SimpleModeTest                                                                 //
//        Класс-наследник TestExecutor - простая обработка полного JSON-блока.                 //
//                                                                                             //
//        Требования и зависимости:                                                            //
//           1. json_reader.h - подключение к основной программе каталога                      //
//           2. log_duration.h - модуль замера времени выполнения                              //
//           3. C++17 (STL)                                                                    //
//                                                                                             //
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "json_reader.h"
#include "log_duration.h"

#include <sstream>
#include <fstream>
#include <cassert>
#include <map>
#include <filesystem>

enum DOC_TYPE              // тип хранимого документа
{
	simple,                // json общего вида включающий base и process
	base,                  // json на создание базы
	process,               // json на получение информации
	json_answer            // json с корректным ответом
};

// блок адресов документов для проверки
const std::map<uint16_t, std::map<DOC_TYPE, std::filesystem::path>> DOC_LIB = {
	{1, 
		{{simple, std::filesystem::path("test_json/part_1_lessen_tests/1_test/1_test_input_simple.json")}
		,{base, std::filesystem::path("test_json/part_1_lessen_tests/1_test/1_test_input_make_base.json")}
		,{process, std::filesystem::path("test_json/part_1_lessen_tests/1_test/1_test_input_process_requests.json")}
		,{json_answer, std::filesystem::path("test_json/part_1_lessen_tests/1_test/1_test_output_correct.json")}}
	},
	{2,
		{{simple, std::filesystem::path("test_json/part_1_lessen_tests/2_test/2_test_input_simple.json")}
		,{base, std::filesystem::path("test_json/part_1_lessen_tests/2_test/2_test_input_make_base.json")}
		,{process, std::filesystem::path("test_json/part_1_lessen_tests/2_test/2_test_input_process_requests.json")}
		,{json_answer, std::filesystem::path("test_json/part_1_lessen_tests/2_test/2_test_output_correct.json")}}
	},
	{3,
		{{simple, std::filesystem::path("test_json/part_1_lessen_tests/3_test/3_test_input_simple.json")}
		,{base, std::filesystem::path("test_json/part_1_lessen_tests/3_test/3_test_input_make_base.json")}
		,{process, std::filesystem::path("test_json/part_1_lessen_tests/3_test/3_test_input_process_requests.json")}
		,{json_answer, std::filesystem::path("test_json/part_1_lessen_tests/3_test/3_test_output_correct.json")}}
	},
	{4,
		{{simple, std::filesystem::path("test_json/part_3_open_tests/1_test/s14_3_opentest_1_simple.json")}
		,{base, std::filesystem::path("test_json/part_3_open_tests/1_test/s14_3_opentest_1_make_base.json")}
		,{process, std::filesystem::path("test_json/part_3_open_tests/1_test/s14_3_opentest_1_process_requests.json")}
		,{json_answer, std::filesystem::path("test_json/part_3_open_tests/1_test/s14_3_opentest_1_answer.json")}}
	},
	{5,
		{{simple, std::filesystem::path("test_json/part_3_open_tests/2_test/s14_3_opentest_2_simple.json")}
		,{base, std::filesystem::path("test_json/part_3_open_tests/2_test/s14_3_opentest_2_make_base.json")}
		,{process, std::filesystem::path("test_json/part_3_open_tests/2_test/s14_3_opentest_2_process_requests.json")}
		,{json_answer, std::filesystem::path("test_json/part_3_open_tests/2_test/s14_3_opentest_2_answer.json")}}
	},
	{6,
		{{simple, std::filesystem::path("test_json/part_3_open_tests/3_test/s14_3_opentest_3_simple.json")}
		,{base, std::filesystem::path("test_json/part_3_open_tests/3_test/s14_3_opentest_3_make_base.json")}
		,{process, std::filesystem::path("test_json/part_3_open_tests/3_test/s14_3_opentest_3_process_requests.json")}
		,{json_answer, std::filesystem::path("test_json/part_3_open_tests/3_test/s14_3_opentest_3_answer.json")}}
	}
};

namespace transport_catalogue {

	namespace test_system {

		class TestExecutor {
		public:
			TestExecutor() = delete;
			TestExecutor(std::istream&, std::ostream&, transport_catalogue::json_reader::ProgramMode);

			virtual void RunAllTests() = 0;                             // запустить все тесты блока
			void ResetBaseSystem();                                     // сбросить текущее состояние ситемы

		protected:
			std::ifstream _input_test_file;                             // поток входного файла с запросами
			std::ifstream _correct_answer_file;                         // поток входного файла с верными овтетами
			std::ifstream _result_db_binary;                            // поток полученного файла библиотеки
			std::stringstream _result_stream;                           // поток информации от каталога
			transport_catalogue::json_reader::JsonReader _reader;       // программа запуска каталога

			bool ResultCompare(std::filesystem::path);                  // проверка результата работы
		};

		class MakeBaseTest : public TestExecutor {
		public:
			MakeBaseTest();
			MakeBaseTest(std::istream&, std::ostream&);

			void RunAllTests() override;                                 // запустить все тесты блока
		private:
			void Execute_Block_Test(size_t);                             // выполнить тест с блоком файлов
		};

		class MakeBaseVORTest : public TestExecutor {
		public:
			MakeBaseVORTest();
			MakeBaseVORTest(std::istream&, std::ostream&);

			void RunAllTests() override;                                 // запустить все тесты блока
		private:
			void Execute_Block_Test(size_t);                             // выполнить тест с блоком файлов
		};

		class SimpleModeTest : protected TestExecutor {
		public:
			SimpleModeTest();
			SimpleModeTest(std::istream&, std::ostream&);

			void RunAllTests() override;                                 // запустить все тесты блока
		private:
			void Execute_Block_Test(size_t);                             // выполнить тест с блоком файлов
		};

	} //namespace test_system

} // namespace transport_catalogue