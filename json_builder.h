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

	// ���� ��������������� �������-����������

	// ��������������� ������������ �����-��������
	class CommonContext {
	public:
		Builder& builder_;

		CommonContext(json::Builder&);                           // ����������� ������

		KeyContext Key(std::string);                             // ������ ���� �������� �������
		CommonContext Value(json::Node::Value);                  // ������ �����/��������� �������� ������������ ����
		DictItemContext StartDict();                             // ������� ����� �������
		ArrayItemContext StartArray();                           // ������� ����� ������		
		CommonContext EndDict();                                 // ������� ������� �������� �������
		CommonContext EndArray();                                // ������� ������� �������� ������

		json::Node Build();
	};

	// ��������������� �����-�������� - �������� �����
	class KeyValueContext : public CommonContext {
	public:

		// ������ ��������� ������������ �����

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

	// ��������������� �����-�������� - ����
	class KeyContext : public CommonContext {
	public:

		// ������ ��������� ������������ �����

		KeyContext(json::Builder&);
		KeyContext Key(std::string) = delete;
		KeyValueContext Value(json::Node::Value);
		DictItemContext StartDict();
		ArrayItemContext StartArray();
		CommonContext EndDict() = delete;
		CommonContext& EndArray() = delete;
		json::Node Build() = delete;
	};


	// ��������������� �����-�������� - �������
	class DictItemContext : public CommonContext {
	public:
		
		// ������ ��������� ������������ �����

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

	// ��������������� �����-�������� - �������� �������
	class ArrayValueContext : public CommonContext {
	public:

		// ������ ��������� ������������ �����

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

	// ��������������� �����-�������� - ������
	class ArrayItemContext : public CommonContext {
	public:

		// ������ ��������� ������������ �����

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


	
	// �������� �����-����������� json-��������
	class Builder { 	
	public:

		Builder() = default;
		~Builder() = default;

		KeyContext Key(std::string);                             // ������ ���� �������� �������
		CommonContext Value(json::Node::Value);                  // ������ �����/��������� �������� ������������ ����
		DictItemContext StartDict();                             // ������� ����� �������
		ArrayItemContext StartArray();                           // ������� ����� ������		
		CommonContext EndDict();                                 // ������� ������� �������� �������
		CommonContext EndArray();                                // ������� ������� �������� ������

		json::Node Build();                                      // ���������� ������������������ �������� ���

	private: 
		json::Node root_ = nullptr;                              // �������� ���
		std::vector<Node*> nodes_stack_;                         // ���� ����� ��� �������������� ������� ��������
		std::string key_;                                        // ������� ���� �������
		bool key_is_open_ = false;                               // ������ �������� �������. ���� �� ����
	};

} // namespace json