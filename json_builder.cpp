#include "json_builder.h"

namespace json {

	// ------------------------------ class CommonContext ----------------------------------

	// Основной конструктор класса
	// Принимает ссылку на объект class json::Builder
	CommonContext::CommonContext(json::Builder& builder)
		: builder_(builder) {
	}

	// Задаёт ключ текущего словаря
	// Делегирует выполнение в основной класс json::Builder
	KeyContext CommonContext::Key(std::string key) {
		return builder_.Key(std::move(key));
	}

	// Задает значение поизвольного типа
	// Делегирует выполнение в основной класс json::Builder
	CommonContext CommonContext::Value(json::Node::Value value) {
		return builder_.Value(std::move(value));
	}

	// Открывает новый словарь
	// Делегирует выполнение в основной класс json::Builder
	DictItemContext CommonContext::StartDict() {
		return builder_.StartDict();
	}

	// Открывает новый массив
	// Делегирует выполнение в основной класс json::Builder
	ArrayItemContext CommonContext::StartArray() {
		return builder_.StartArray();
	}

	// Закрыть текущий открытый словарь
	// Делегирует выполнение в основной класс json::Builder
	CommonContext CommonContext::EndDict() {
		return builder_.EndDict();
	}
	
	// Закрыть текущий открытый массив
	// Делегирует выполнение в основной класс json::Builder
	CommonContext CommonContext::EndArray() {
		return builder_.EndArray();
	}

	// Получить результат работы с корневым значением root_
	// Делегирует выполнение в основной класс json::Builder
	json::Node CommonContext::Build() {
		return builder_.Build();
	}

	// ------------------------------ class CommonContext END ------------------------------


	// ------------------------------ class KeyValueContext --------------------------------

	// Основной конструктор класса
	// Принимает ссылку на объект class json::Builder
	KeyValueContext::KeyValueContext(json::Builder& builder)
		: CommonContext(builder) {
	}

	// Дополнительный конструктор класса
	// Конструирует объект через json::CommonContext
	KeyValueContext::KeyValueContext(json::CommonContext common)
		: CommonContext(common) {
	}

	// Задать следующий ключ текущего словаря
	// Делигирует выполнение операции в родительский класс json::CommonContext
	KeyContext KeyValueContext::Key(std::string key) {
		return CommonContext::Key(std::move(key));
	}

	// Закрытие текущего открытого словаря
	// Делигирует выполнение операции в родительский класс json::CommonContext
	CommonContext KeyValueContext::EndDict() {
		return CommonContext::EndDict();
	}

	// ------------------------------ class KeyValueContext END ----------------------------


	// ------------------------------ class KeyContext -------------------------------------

	// Основной конструктор класса
	// Принимает ссылку на объект class json::Builder
	KeyContext::KeyContext(json::Builder& builder)
		: CommonContext(builder) {
	}

	// Задать значение текущего ключа
	// Делигирует выполнение операции в родительский класс json::CommonContext
	KeyValueContext KeyContext::Value(json::Node::Value value) {
		return CommonContext::Value(std::move(value));
	}

	// Открыть новый словарь
	// Делигирует выполнение операции в родительский класс json::CommonContext
	DictItemContext KeyContext::StartDict() {
		return CommonContext::StartDict();
	}

	// Открыть новый массив
	// Делигирует выполнение операции в родительский класс json::CommonContext
	ArrayItemContext KeyContext::StartArray() {
		return CommonContext::StartArray();
	}


	// ------------------------------ class KeyContext END ---------------------------------



	// ------------------------------ class DictItemContext --------------------------------

	// Основной конструктор класса
	// Принимает ссылку на объект class json::Builder
	DictItemContext::DictItemContext(json::Builder& builder) 
		: CommonContext(builder) {
	}

	// Задать ключ текущего словаря
	// Делигирует выполнение операции в родительский класс json::CommonContext
	KeyContext DictItemContext::Key(std::string key) {
		return CommonContext::Key(std::move(key));
	}

	// Закрытие текущего открытого словаря
	// Делигирует выполнение операции в родительский класс json::CommonContext
	CommonContext DictItemContext::EndDict() {
		return CommonContext::EndDict();
	}

	// ------------------------------ class DictItemContext END ----------------------------


	// ------------------------------ class ArrayValueContext ------------------------------

	// Основной конструктор класса
	// Принимает ссылку на объект class json::Builder
	ArrayValueContext::ArrayValueContext(json::Builder& builder)
		: CommonContext(builder) {
	}

	// Дополнительный конструктор класса
	// Конструирует объект через json::CommonContext
	ArrayValueContext::ArrayValueContext(json::CommonContext common) 
		: CommonContext(common) {
	}

	// Добавить значение в массив
	// Делигирует выполнение операции в родительский класс json::CommonContext
	ArrayValueContext ArrayValueContext::Value(json::Node::Value value) {
		return CommonContext::Value(std::move(value));
	}

	// Начать новый словарь
	// Делигирует выполнение операции в родительский класс json::CommonContext
	DictItemContext ArrayValueContext::StartDict() {
		return CommonContext::StartDict();
	}

	// Начать новый массив
	// Делигирует выполнение операции в родительский класс json::CommonContext
	ArrayItemContext ArrayValueContext::StartArray() {
		return CommonContext::StartArray();
	}

	// Закрыть текущего открытый массив
	// Делигирует выполнение операции в родительский класс json::CommonContext
	CommonContext ArrayValueContext::EndArray() {
		return CommonContext::EndArray();
	}

	// ------------------------------ class ArrayValueContext END --------------------------


	// ------------------------------ class ArrayItemContext -------------------------------

	// Основной конструктор класса
	// Принимает ссылку на объект class json::Builder
	ArrayItemContext::ArrayItemContext(json::Builder& builder)
		: CommonContext(builder) {
	}

	// Добавить значение в массив
	// Делигирует выполнение операции в родительский класс json::CommonContext
	ArrayValueContext ArrayItemContext::Value(json::Node::Value value) {
		return CommonContext::Value(std::move(value));
	}

	// Начать новый словарь
	// Делигирует выполнение операции в родительский класс json::CommonContext
	DictItemContext ArrayItemContext::StartDict() {
		return CommonContext::StartDict();
	}

	// Начать новый массив
	// Делигирует выполнение операции в родительский класс json::CommonContext
	ArrayItemContext ArrayItemContext::StartArray() {
		return CommonContext::StartArray();
	}

	// Закрыть текущего открытый массив
	// Делигирует выполнение операции в родительский класс json::CommonContext
	CommonContext ArrayItemContext::EndArray() {
		return CommonContext::EndArray();
	}

	// ------------------------------ class ArrayItemContext END ---------------------------


	// ------------------------------ class Builder ----------------------------------------

	// Задать ключ текущего словаря
	// Выполняется после применения StartDict() - открытия нового словаря
	// Выполняется после записи значения предыдущего ключа (bool key_is_open_ = false)
	KeyContext Builder::Key(std::string key) {

		if (root_ == nullptr) { // если вызывается ключ для пустого рута
			throw std::logic_error("Key() called for empty document.");
		}

		else if (nodes_stack_.empty()) { // нельзя вызывать ключ для НЕ словаря
			throw std::logic_error("Key() called outside of any container element.");
		}

		// получаем родительский корень
		json::Node* parrent = nodes_stack_.back();
		
		if (parrent->IsDict()) {

			if (!key_is_open_) {
				key_ = std::move(key);
				key_is_open_ = true;
				const_cast<json::Dict&>(parrent->AsDict())[key_] = Node();
			}

			else { // если флажок true, то ключ вызван дважды подряд
				throw std::logic_error("Key() double called.");
			}
		}

		else { // если ролительский узел не является словарем, то это логическая ошибка
			throw std::logic_error("Key() called in wrong container.");
		}

		return KeyContext{ *this };
	}

	// Добавить новое/следующее значение произвольного типа
	// Выполняется для задачи отдельного значения типов std::string, int, double, etc...
	// Выполняется для задачи следующего значения в открытом массиве json::Array
	// Выполняется для задачи значения открытого ключа словаря json::Dict
	CommonContext Builder::Value(json::Node::Value value) {

		if (root_ == nullptr) { // когда рут пуст, просто закидываем value в root_ и отдаём в return
			root_.GetValue() = std::move(value);
			return CommonContext{ *this };
		}

		else if (nodes_stack_.empty()) { // не может быть ситуации когда root_ уже не нуль и стек при этом пуст
			throw std::logic_error("Value() called when root_ not nullptr, but nodes stack is empty");
		}

		// получаем родительский корень
		json::Node* parrent = nodes_stack_.back();

		if (parrent->IsDict()) { // если у нас родительский узел является словарем

			if (!key_is_open_) {
				// если ключ не открыт то нарушение логики
				throw std::logic_error("json::Dict()'s key_ must be opened");
			}
			else {
				// при открытом ключе записываем Value в значение словаря
				const_cast<json::Dict&>(parrent->AsDict())[key_] = std::move(value);
				key_.clear(); // стираем ключ
				key_is_open_ = false; // переводим флажок
			}
		}

		else if (parrent->IsArray()) {// если у нас родительский узел является массивом
			// дописываем значение в массив
			const_cast<json::Array&>(parrent->AsArray()).push_back(std::move(value));
		}

		else {
			throw std::logic_error("Value() called in unknow container");
		}

		return CommonContext{ *this };
	}

	// Открытие нового словаря в модуле класса
	// Недопустимо двойной вызов метода - использование при неоткрытом ключе (bool key_is_open_ = false)
	DictItemContext Builder::StartDict() {

		if (root_ == nullptr) { // если рут пустой
			root_ = json::Dict(); // начинаем новый словарь
			nodes_stack_.emplace_back(&root_); //закидываем его в стек
			return DictItemContext{ *this };
		}

		else if (nodes_stack_.empty()) { // не может быть ситуации когда root_ уже не нуль и стек при этом пуст
			throw std::logic_error("StartDict() called when root_ not nullptr, but nodes stack is empty");
		}

		// получаем родительский корень
		json::Node* parrent = nodes_stack_.back();

		if (parrent->IsDict()) { // если родительский ключ это словарь

			if (!key_is_open_) { // нельзя начинать новый словарь при закрытом ключе
				throw std::logic_error("StartDict() called when parrent root is Dict, and key is not opened");
			}

			else {
				// начинаем новый словарь в значении ключа текущего словаря
				const_cast<json::Dict&>(parrent->AsDict())[key_] = json::Dict();
				// получаем созданную ноду назад
				json::Node* node = &const_cast<json::Dict&>(parrent->AsDict()).at(key_);
				// записываем ее в стек
				nodes_stack_.push_back(std::move(node));
				// обнуляем ключ
				key_.clear();
				// закрываем флажок
				key_is_open_ = false;
			}

		}

		else if (parrent->IsArray()) { // если родительский ключ это массив

			// записываем в родительский массив новый словарь
			const_cast<json::Array&>(parrent->AsArray()).push_back(json::Dict());
			// получаем созданную ноду назад
			json::Node* node = &const_cast<json::Array&>(parrent->AsArray()).back();
			// записываем ее в стек
			nodes_stack_.push_back(std::move(node));
		}

		else { // если родительский ключ не определен как массив или словарь
			throw std::logic_error("StartDict() called not for empty document, nor for Array element / Dict Value element.");
		}

		return DictItemContext{ *this };
	}

	// Открытие нового массива в модуле класса
	// При интеграции в словарь недопустимо использование при неоткрытом ключе (bool key_is_open_ = false)
	ArrayItemContext Builder::StartArray() {

		if (root_ == nullptr) { // если рут пустой
			root_ = json::Array(); // начинаем новый словарь
			nodes_stack_.push_back(&root_); //закидываем его в стек
			return ArrayItemContext{ *this };
		}

		else if (nodes_stack_.empty()) { // не может быть ситуации когда root_ уже не нуль и стек при этом пуст
			throw std::logic_error("StartArray() called when root_ not nullptr, but nodes stack is empty");
		}

		// получаем родительский корень
		json::Node* parrent = nodes_stack_.back();

		if (parrent->IsDict()) { // если родительский ключ это словарь

			if (!key_is_open_) { // нельзя начинать новый массив при закрытом ключе
				throw std::logic_error("StartArray() called when parrent root is Dict, and key is not opened");
			}

			else {
				// начинаем новый словарь в значении ключа текущего словаря
				const_cast<json::Dict&>(parrent->AsDict())[key_] = json::Array();
				// получаем созданную ноду назад
				json::Node* node = &const_cast<json::Dict&>(parrent->AsDict()).at(key_);
				// записываем ее в стек
				nodes_stack_.push_back(std::move(node));
				// обнуляем ключ
				key_.clear();
				// закрываем флажок
				key_is_open_ = false;
			}

		}

		else if (parrent->IsArray()) { // если родительский ключ это массив

			// записываем в родительский массив новый словарь
			const_cast<json::Array&>(parrent->AsArray()).push_back(json::Array());
			// получаем созданную ноду назад
			json::Node* node = &const_cast<json::Array&>(parrent->AsArray()).back();
			// записываем ее в стек
			nodes_stack_.push_back(std::move(node));
		}

		else { // если родительский ключ не определен как массив или словарь
			throw std::logic_error("StartArray() called not for empty document, nor for Array element / Dict Value element.");
		}


		return ArrayItemContext{ *this };
	}

	// Закрытие текущего открытого словаря
	// Операция недопустима если последний ключ не закрыт (bool key_is_open_ = true)
	CommonContext Builder::EndDict() {

		if (root_ == nullptr) { // если рут нулевой, нельзя закрыть словарь если нет словаря
			throw std::logic_error("EndDict() called when root_ is nullptr");
		}

		else if (nodes_stack_.empty()) { // вызов при пустом стеке, нельзя закрыть словарь которого нет
			throw std::logic_error("EndDict() called when root_ is not nullptr, but nodes stack is empty");
		}

		// получаем родительский корень
		json::Node* parrent = nodes_stack_.back();

		if (parrent->IsDict()) { // если родиельская нода - словарь

			if (key_is_open_) { // если у словаря не закрыт ключ
				throw std::logic_error("EndDict() cant be called when json::Dict's is not closed");
			}

			// удаляем крайнюю вершину стека
			nodes_stack_.pop_back();
		}

		else { // нельзя вызвать закрытие словаря если в ноде не словарь
			throw std::logic_error("EndDict() called when parrent root is not a json::Dict");
		}

		return CommonContext{ *this };
	}

	// Закрытие текущего открытого массива
	CommonContext Builder::EndArray() {

		if (root_ == nullptr) { // если рут нулевой, нельзя закрыть массив если массива нет
			throw std::logic_error("EndArray() called when root_ is nullptr");
		}

		else if (nodes_stack_.empty()) { // вызов при пустом стеке, нельзя закрыть массив которого нет
			throw std::logic_error("EndArray() called when root_ is not nullptr, but nodes stack is empty");
		}

		// получаем родительский корень
		json::Node* parrent = nodes_stack_.back();

		if (parrent->IsArray()) { // если в родительской ноде массив

			// удаляем крайнюю вершину стека
			nodes_stack_.pop_back();
		}

		else { // нельзя вызвать закрытие массива если в ноде не массив
			throw std::logic_error("EndArray() called when parrent root is not a json::Array");
		}

		return CommonContext{ *this };
	}

	// Возвращает значение сконфигурированного корневого json::Node
	json::Node Builder::Build() {

		if (root_ == nullptr) {
			// постройка вызывается у пустого рута
			throw std::logic_error("Build() called for empty document.");
		}

		else if (!nodes_stack_.empty()) {
			// постройка вызывается в том случае, если какой-то из внутренних контейнеров не закрыт
			throw std::logic_error("Build() called for not finished document.");
		}

		return std::move(root_);
	}

	// ------------------------------ class Builder END ------------------------------------

} // namespace json