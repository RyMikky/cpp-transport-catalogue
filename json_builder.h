#pragma once

#include "json.h"

namespace json {

	class Builder;
	class CommonContext;
	class KeyContext;
	class DictItemContext;
	class ArrayItemContext;
	class KeyValueContext;
	class ArrayValueContext;

	// Блок вспомогательных классов-контекстов

	// Вспомогательный родительский класс-контекст
	class CommonContext {
	public:
		Builder& builder_;

		CommonContext(json::Builder&);                           // конструктор класса

		KeyContext Key(std::string);                             // задать ключ текущего словаря
		CommonContext Value(json::Node::Value);                  // задать новое/следующее значение поизвольного типа
		DictItemContext StartDict();                             // открыть новый словарь
		ArrayItemContext StartArray();                           // открыть новый массив		
		CommonContext EndDict();                                 // закрыть текущий открытый словарь
		CommonContext EndArray();                                // закрыть текущий открытый массив

		json::Node Build();
	};

	// Вспомогательный класс-контекст - значения ключа
	class KeyValueContext : public CommonContext {
	public:

		// методы дублируют родительский класс

		KeyValueContext(json::Builder&);
		KeyValueContext(CommonContext);
		KeyContext Key(std::string);
		KeyValueContext Value(json::Node::Value) = delete;
		DictItemContext StartDict() = delete;
		ArrayItemContext StartArray() = delete;
		CommonContext EndDict();
		CommonContext EndArray() = delete;
		json::Node Build() = delete;
	private:
	};

	// Вспомогательный класс-контекст - ключ
	class KeyContext : public CommonContext {
	public:

		// методы дублируют родительский класс

		KeyContext(json::Builder&);
		KeyContext Key(std::string) = delete;
		KeyValueContext Value(json::Node::Value);
		DictItemContext StartDict();
		ArrayItemContext StartArray();
		CommonContext EndDict() = delete;
		CommonContext& EndArray() = delete;
		json::Node Build() = delete;
	};


	// Вспомогательный класс-контекст - словарь
	class DictItemContext : public CommonContext {
	public:
		
		// методы дублируют родительский класс

		DictItemContext(json::Builder&);
		KeyContext Key(std::string);
		CommonContext Value(json::Node::Value) = delete;
		DictItemContext StartDict() = delete;
		ArrayItemContext StartArray() = delete;
		CommonContext EndDict();
		CommonContext EndArray() = delete;
		json::Node Build() = delete;
	private:
	};

	// Вспомогательный класс-контекст - значения массива
	class ArrayValueContext : public CommonContext {
	public:

		// методы дублируют родительский класс

		ArrayValueContext(json::Builder&);
		ArrayValueContext(CommonContext);
		KeyContext Key(std::string) = delete;
		ArrayValueContext Value(json::Node::Value);
		DictItemContext StartDict();
		ArrayItemContext StartArray();
		CommonContext EndDict() = delete;
		CommonContext EndArray();
		json::Node Build() = delete;
	private:
	};

	// Вспомогательный класс-контекст - массив
	class ArrayItemContext : public CommonContext {
	public:

		// методы дублируют родительский класс

		ArrayItemContext(json::Builder&);
		KeyContext Key(std::string) = delete;
		ArrayValueContext Value(json::Node::Value);
		DictItemContext StartDict();
		ArrayItemContext StartArray();
		CommonContext EndDict() = delete;
		CommonContext EndArray();
		json::Node Build() = delete;
	private:
	};


	
	// Основной класс-конструктор json-объектов
	class Builder { 	
	public:

		Builder() = default;
		~Builder() = default;

		KeyContext Key(std::string);                             // задать ключ текущего словаря
		CommonContext Value(json::Node::Value);                  // задать новое/следующее значение поизвольного типа
		DictItemContext StartDict();                             // открыть новый словарь
		ArrayItemContext StartArray();                           // открыть новый массив		
		CommonContext EndDict();                                 // закрыть текущий открытый словарь
		CommonContext EndArray();                                // закрыть текущий открытый массив

		json::Node Build();                                      // возвращает сконфигурированный корневой рут

	private: 
		json::Node root_ = nullptr;                              // основной рут
		std::vector<Node*> nodes_stack_;                         // стак рутов при многоуровневой системе докумета
		std::string key_;                                        // текущий ключ массива
		bool key_is_open_ = false;                               // флажок открытия словаря. Есть ли ключ
	};

} // namespace json