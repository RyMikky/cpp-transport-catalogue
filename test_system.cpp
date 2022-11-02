#pragma once

#include "test_system.h"

namespace transport_catalogue {

	namespace test_system {

	// --------------------------- class TestExecutor ------------------------------------------

	TestExecutor::TestExecutor(std::istream& input, std::ostream& output
		, transport_catalogue::json_reader::ProgramMode mode)
			: _reader(input, output, mode) {
	}

	void TestExecutor::ResetBaseSystem() {

		_input_test_file.close();              // закрываем файлы входного потока
		_correct_answer_file.close();          // закрываем файлы готового ответа
		_result_stream.str("");         // очищаем поток ответа

		_reader.reset_all_modules();           // очищаем состояние модулей каталога
	}

	bool TestExecutor::ResultCompare(std::filesystem::path answer) {

		// открываем файл с верным ответом
		_correct_answer_file.open(answer);

		if (!_correct_answer_file.is_open()) {
			// если файл не открыт
			return false;
		}
		
		return json::Load(_result_stream) == json::Load(_correct_answer_file);
	}

	// --------------------------- class TestExecutor END --------------------------------------


	// --------------------------- class MakeBaseTest ------------------------------------------
	
	MakeBaseTest::MakeBaseTest()
		: TestExecutor(_input_test_file, _result_stream, transport_catalogue::json_reader::test_mode) {
	}

	MakeBaseTest::MakeBaseTest(std::istream& input, std::ostream& output)
		: TestExecutor(input, output, transport_catalogue::json_reader::make_base_mode) {
	}

	void MakeBaseTest::RunAllTests() {

		std::cerr << "\nmake_base, all tests, begin\n";

		for (size_t i = 1; i <= DOC_LIB.size(); ++i) {
			Execute_Block_Test(i);
		}

		std::cerr << "\nmake_base, all tests, complete\n\n\n";
	}

	void MakeBaseTest::Execute_Block_Test(size_t num) {

		std::cerr << std::endl << "make_base, test "sv << num << ", begin"sv << std::endl;

		// проверяем что файлы изначально не открыты
		if (_input_test_file.is_open()) {
			assert(false);
		}

		// открываем файл блока загрузки базы
		_input_test_file.open(DOC_LIB.at((uint16_t)num).at(base));

		// если файл открыт
		if (_input_test_file.is_open()) {
			{
				// замеряем время работы блока
				LogDuration process("\tmake_base, test " + std::to_string(num) + ", execution");
				// запускаем выполнение запроса
				_reader.make_base_mode();
			}

			// закрываем файл первого блока на формирование базы
			_input_test_file.close();
		}

		// открываем файл первого блока на формирование результата
		_input_test_file.open(DOC_LIB.at((uint16_t)num).at(process));

		// если файл открыт
		if (_input_test_file.is_open()) {
			{
				// замеряем время работы блока
				LogDuration process("\tprocess_requests_from_serial_db, test " + std::to_string(num) + ", execution");
				// запускаем выполнение запроса
				_reader.process_requests_mode();
			}
		}

		{
			// замеряем время работы блока
			LogDuration process("\tmake_base, test " + std::to_string(num) + ", compare");
			// сверяем результат
			assert(ResultCompare(DOC_LIB.at((uint16_t)num).at(json_answer)));
		}

		ResetBaseSystem();          // обнуляем состояние системы перед следующим тестом

		std::cerr << "\tmake_base, test " << num << ", complete"sv << std::endl;

	}

	// --------------------------- class MakeBaseTest END --------------------------------------


	// --------------------------- class MakeBaseVORTest ---------------------------------------

	MakeBaseVORTest::MakeBaseVORTest() : TestExecutor(_input_test_file, _result_stream
		, transport_catalogue::json_reader::test_mode) {
	}

	MakeBaseVORTest::MakeBaseVORTest(std::istream& input, std::ostream& output)
		: TestExecutor(input, output, transport_catalogue::json_reader::make_base_mode_vo_router) {
	}

	void MakeBaseVORTest::RunAllTests() {

		std::cerr << "\nmake_base_whithout_router, all tests, begin\n";

		for (size_t i = 1; i <= DOC_LIB.size(); ++i) {
			Execute_Block_Test(i);
		}

		std::cerr << "\nmake_base_whithout_router, all tests, complete\n\n\n";
	}

	void MakeBaseVORTest::Execute_Block_Test(size_t num) {

		std::cerr << std::endl << "make_base_whithout_router, test "sv << num << ", begin"sv << std::endl;

		// проверяем что файлы изначально не открыты
		if (_input_test_file.is_open() && _result_db_binary.is_open()) {
			assert(false);
		}

		// открываем файл блока загрузки базы
		_input_test_file.open(DOC_LIB.at((uint16_t)num).at(base));

		// если файл открыт
		if (_input_test_file.is_open()) {
			{
				// замеряем время работы блока
				LogDuration process("\tmake_base_whithout_router, test " + std::to_string(num) + ", execution");
				// запускаем выполнение запроса
				_reader.make_base_mode_vo_router();
			}

			// закрываем файл первого блока на формирование базы
			_input_test_file.close();
		}

		// открываем файл первого блока на формирование результата
		_input_test_file.open(DOC_LIB.at((uint16_t)num).at(process));

		// если файл открыт
		if (_input_test_file.is_open()) {
			{
				// замеряем время работы блока
				LogDuration process("\tprocess_requests_from_serial_db, test " + std::to_string(num) + ", execution");
				// запускаем выполнение запроса
				_reader.process_requests_mode();
			}
		}

		{
			// замеряем время работы блока
			LogDuration process("\tmake_base_whithout_router, test " + std::to_string(num) + ", compare");
			// сверяем результат
			assert(ResultCompare(DOC_LIB.at((uint16_t)num).at(json_answer)));
		}

		ResetBaseSystem();          // обнуляем состояние системы перед следующим тестом

		std::cerr << "\tmake_base_whithout_router, test " << num << ", complete"sv << std::endl;

	}

	// --------------------------- class MakeBaseVORTest END -----------------------------------
	
	
	// --------------------------- class SimpleModeTest ----------------------------------------

	SimpleModeTest::SimpleModeTest() : TestExecutor(_input_test_file, _result_stream
		, transport_catalogue::json_reader::test_mode) {
	}

	SimpleModeTest::SimpleModeTest(std::istream& input, std::ostream& output)
		: TestExecutor(input, output, transport_catalogue::json_reader::test_mode) {
	}

	void SimpleModeTest::RunAllTests() {

		std::cerr << "\nsingle_mode, all tests, begin\n";

		for (size_t i = 1; i <= DOC_LIB.size(); ++i) {
			Execute_Block_Test(i);
		}

		std::cerr << "\nsingle_mode, all tests, complete\n\n\n";
	}

	void SimpleModeTest::Execute_Block_Test(size_t num) {

		std::cerr << std::endl << "single_mode, test "sv << num << ", begin"sv << std::endl;

		if (_input_test_file.is_open()) {
			assert(false);
		}

		// открываем файл первого блока
		_input_test_file.open(DOC_LIB.at((uint16_t)num).at(simple));

		// если файл открыт
		if (_input_test_file.is_open()) {
			{
				// замеряем время работы блока
				LogDuration process("\tsingle_mode, test " + std::to_string(num) + ", execution");
				// запускаем выполнение запроса
				_reader.single_block_mode();
			}
		}

		{
			// замеряем время работы блока
			LogDuration process("\tsingle_mode, test " + std::to_string(num) + ", compare");
			// сверяем результат
			assert(ResultCompare(DOC_LIB.at((uint16_t)num).at(json_answer)));
		}

		ResetBaseSystem();          // обнуляем состояние системы перед следующим тестом

		std::cerr << "\tsingle_mode, test " << num << ", complete"sv << std::endl;

	}

	// --------------------------- class SimpleModeTest END ------------------------------------

	} //namespace test_system

} // namespace transport_catalogue