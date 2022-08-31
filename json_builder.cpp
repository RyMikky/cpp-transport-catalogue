#include "json_builder.h"

namespace json {

	// ------------------------------ class CommonContext ----------------------------------

	// �������� ����������� ������
	// ��������� ������ �� ������ class json::Builder
	CommonContext::CommonContext(json::Builder& builder)
		: builder_(builder) {
	}

	// ����� ���� �������� �������
	// ���������� ���������� � �������� ����� json::Builder
	KeyContext CommonContext::Key(std::string key) {
		return builder_.Key(std::move(key));
	}

	// ������ �������� ������������ ����
	// ���������� ���������� � �������� ����� json::Builder
	CommonContext CommonContext::Value(json::Node::Value value) {
		return builder_.Value(std::move(value));
	}

	// ��������� ����� �������
	// ���������� ���������� � �������� ����� json::Builder
	DictItemContext CommonContext::StartDict() {
		return builder_.StartDict();
	}

	// ��������� ����� ������
	// ���������� ���������� � �������� ����� json::Builder
	ArrayItemContext CommonContext::StartArray() {
		return builder_.StartArray();
	}

	// ������� ������� �������� �������
	// ���������� ���������� � �������� ����� json::Builder
	CommonContext CommonContext::EndDict() {
		return builder_.EndDict();
	}
	
	// ������� ������� �������� ������
	// ���������� ���������� � �������� ����� json::Builder
	CommonContext CommonContext::EndArray() {
		return builder_.EndArray();
	}

	// �������� ��������� ������ � �������� ��������� root_
	// ���������� ���������� � �������� ����� json::Builder
	json::Node CommonContext::Build() {
		return builder_.Build();
	}

	// ------------------------------ class CommonContext END ------------------------------


	// ------------------------------ class KeyValueContext --------------------------------

	// �������� ����������� ������
	// ��������� ������ �� ������ class json::Builder
	KeyValueContext::KeyValueContext(json::Builder& builder)
		: CommonContext(builder) {
	}

	// �������������� ����������� ������
	// ������������ ������ ����� json::CommonContext
	KeyValueContext::KeyValueContext(json::CommonContext common)
		: CommonContext(common) {
	}

	// ������ ��������� ���� �������� �������
	// ���������� ���������� �������� � ������������ ����� json::CommonContext
	KeyContext KeyValueContext::Key(std::string key) {
		return CommonContext::Key(std::move(key));
	}

	// �������� �������� ��������� �������
	// ���������� ���������� �������� � ������������ ����� json::CommonContext
	CommonContext KeyValueContext::EndDict() {
		return CommonContext::EndDict();
	}

	// ------------------------------ class KeyValueContext END ----------------------------


	// ------------------------------ class KeyContext -------------------------------------

	// �������� ����������� ������
	// ��������� ������ �� ������ class json::Builder
	KeyContext::KeyContext(json::Builder& builder)
		: CommonContext(builder) {
	}

	// ������ �������� �������� �����
	// ���������� ���������� �������� � ������������ ����� json::CommonContext
	KeyValueContext KeyContext::Value(json::Node::Value value) {
		return CommonContext::Value(std::move(value));
	}

	// ������� ����� �������
	// ���������� ���������� �������� � ������������ ����� json::CommonContext
	DictItemContext KeyContext::StartDict() {
		return CommonContext::StartDict();
	}

	// ������� ����� ������
	// ���������� ���������� �������� � ������������ ����� json::CommonContext
	ArrayItemContext KeyContext::StartArray() {
		return CommonContext::StartArray();
	}


	// ------------------------------ class KeyContext END ---------------------------------



	// ------------------------------ class DictItemContext --------------------------------

	// �������� ����������� ������
	// ��������� ������ �� ������ class json::Builder
	DictItemContext::DictItemContext(json::Builder& builder) 
		: CommonContext(builder) {
	}

	// ������ ���� �������� �������
	// ���������� ���������� �������� � ������������ ����� json::CommonContext
	KeyContext DictItemContext::Key(std::string key) {
		return CommonContext::Key(std::move(key));
	}

	// �������� �������� ��������� �������
	// ���������� ���������� �������� � ������������ ����� json::CommonContext
	CommonContext DictItemContext::EndDict() {
		return CommonContext::EndDict();
	}

	// ------------------------------ class DictItemContext END ----------------------------


	// ------------------------------ class ArrayValueContext ------------------------------

	// �������� ����������� ������
	// ��������� ������ �� ������ class json::Builder
	ArrayValueContext::ArrayValueContext(json::Builder& builder)
		: CommonContext(builder) {
	}

	// �������������� ����������� ������
	// ������������ ������ ����� json::CommonContext
	ArrayValueContext::ArrayValueContext(json::CommonContext common) 
		: CommonContext(common) {
	}

	// �������� �������� � ������
	// ���������� ���������� �������� � ������������ ����� json::CommonContext
	ArrayValueContext ArrayValueContext::Value(json::Node::Value value) {
		return CommonContext::Value(std::move(value));
	}

	// ������ ����� �������
	// ���������� ���������� �������� � ������������ ����� json::CommonContext
	DictItemContext ArrayValueContext::StartDict() {
		return CommonContext::StartDict();
	}

	// ������ ����� ������
	// ���������� ���������� �������� � ������������ ����� json::CommonContext
	ArrayItemContext ArrayValueContext::StartArray() {
		return CommonContext::StartArray();
	}

	// ������� �������� �������� ������
	// ���������� ���������� �������� � ������������ ����� json::CommonContext
	CommonContext ArrayValueContext::EndArray() {
		return CommonContext::EndArray();
	}

	// ------------------------------ class ArrayValueContext END --------------------------


	// ------------------------------ class ArrayItemContext -------------------------------

	// �������� ����������� ������
	// ��������� ������ �� ������ class json::Builder
	ArrayItemContext::ArrayItemContext(json::Builder& builder)
		: CommonContext(builder) {
	}

	// �������� �������� � ������
	// ���������� ���������� �������� � ������������ ����� json::CommonContext
	ArrayValueContext ArrayItemContext::Value(json::Node::Value value) {
		return CommonContext::Value(std::move(value));
	}

	// ������ ����� �������
	// ���������� ���������� �������� � ������������ ����� json::CommonContext
	DictItemContext ArrayItemContext::StartDict() {
		return CommonContext::StartDict();
	}

	// ������ ����� ������
	// ���������� ���������� �������� � ������������ ����� json::CommonContext
	ArrayItemContext ArrayItemContext::StartArray() {
		return CommonContext::StartArray();
	}

	// ������� �������� �������� ������
	// ���������� ���������� �������� � ������������ ����� json::CommonContext
	CommonContext ArrayItemContext::EndArray() {
		return CommonContext::EndArray();
	}

	// ------------------------------ class ArrayItemContext END ---------------------------


	// ------------------------------ class Builder ----------------------------------------

	// ������ ���� �������� �������
	// ����������� ����� ���������� StartDict() - �������� ������ �������
	// ����������� ����� ������ �������� ����������� ����� (bool key_is_open_ = false)
	KeyContext Builder::Key(std::string key) {

		if (root_ == nullptr) { // ���� ���������� ���� ��� ������� ����
			throw std::logic_error("Key() called for empty document.");
		}

		else if (nodes_stack_.empty()) { // ������ �������� ���� ��� �� �������
			throw std::logic_error("Key() called outside of any container element.");
		}

		// �������� ������������ ������
		json::Node* parrent = nodes_stack_.back();
		
		if (parrent->IsDict()) {

			if (!key_is_open_) {
				key_ = std::move(key);
				key_is_open_ = true;
				const_cast<json::Dict&>(parrent->AsDict())[key_] = Node();
			}

			else { // ���� ������ true, �� ���� ������ ������ ������
				throw std::logic_error("Key() double called.");
			}
		}

		else { // ���� ������������ ���� �� �������� ��������, �� ��� ���������� ������
			throw std::logic_error("Key() called in wrong container.");
		}

		return KeyContext{ *this };
	}

	// �������� �����/��������� �������� ������������� ����
	// ����������� ��� ������ ���������� �������� ����� std::string, int, double, etc...
	// ����������� ��� ������ ���������� �������� � �������� ������� json::Array
	// ����������� ��� ������ �������� ��������� ����� ������� json::Dict
	CommonContext Builder::Value(json::Node::Value value) {

		if (root_ == nullptr) { // ����� ��� ����, ������ ���������� value � root_ � ����� � return
			root_.GetValue() = std::move(value);
			return CommonContext{ *this };
		}

		else if (nodes_stack_.empty()) { // �� ����� ���� �������� ����� root_ ��� �� ���� � ���� ��� ���� ����
			throw std::logic_error("Value() called when root_ not nullptr, but nodes stack is empty");
		}

		// �������� ������������ ������
		json::Node* parrent = nodes_stack_.back();

		if (parrent->IsDict()) { // ���� � ��� ������������ ���� �������� ��������

			if (!key_is_open_) {
				// ���� ���� �� ������ �� ��������� ������
				throw std::logic_error("json::Dict()'s key_ must be opened");
			}
			else {
				// ��� �������� ����� ���������� Value � �������� �������
				const_cast<json::Dict&>(parrent->AsDict())[key_] = std::move(value);
				key_.clear(); // ������� ����
				key_is_open_ = false; // ��������� ������
			}
		}

		else if (parrent->IsArray()) {// ���� � ��� ������������ ���� �������� ��������
			// ���������� �������� � ������
			const_cast<json::Array&>(parrent->AsArray()).push_back(std::move(value));
		}

		else {
			throw std::logic_error("Value() called in unknow container");
		}

		return CommonContext{ *this };
	}

	// �������� ������ ������� � ������ ������
	// ����������� ������� ����� ������ - ������������� ��� ���������� ����� (bool key_is_open_ = false)
	DictItemContext Builder::StartDict() {

		if (root_ == nullptr) { // ���� ��� ������
			root_ = json::Dict(); // �������� ����� �������
			nodes_stack_.emplace_back(&root_); //���������� ��� � ����
			return DictItemContext{ *this };
		}

		else if (nodes_stack_.empty()) { // �� ����� ���� �������� ����� root_ ��� �� ���� � ���� ��� ���� ����
			throw std::logic_error("StartDict() called when root_ not nullptr, but nodes stack is empty");
		}

		// �������� ������������ ������
		json::Node* parrent = nodes_stack_.back();

		if (parrent->IsDict()) { // ���� ������������ ���� ��� �������

			if (!key_is_open_) { // ������ �������� ����� ������� ��� �������� �����
				throw std::logic_error("StartDict() called when parrent root is Dict, and key is not opened");
			}

			else {
				// �������� ����� ������� � �������� ����� �������� �������
				const_cast<json::Dict&>(parrent->AsDict())[key_] = json::Dict();
				// �������� ��������� ���� �����
				json::Node* node = &const_cast<json::Dict&>(parrent->AsDict()).at(key_);
				// ���������� �� � ����
				nodes_stack_.push_back(std::move(node));
				// �������� ����
				key_.clear();
				// ��������� ������
				key_is_open_ = false;
			}

		}

		else if (parrent->IsArray()) { // ���� ������������ ���� ��� ������

			// ���������� � ������������ ������ ����� �������
			const_cast<json::Array&>(parrent->AsArray()).push_back(json::Dict());
			// �������� ��������� ���� �����
			json::Node* node = &const_cast<json::Array&>(parrent->AsArray()).back();
			// ���������� �� � ����
			nodes_stack_.push_back(std::move(node));
		}

		else { // ���� ������������ ���� �� ��������� ��� ������ ��� �������
			throw std::logic_error("StartDict() called not for empty document, nor for Array element / Dict Value element.");
		}

		return DictItemContext{ *this };
	}

	// �������� ������ ������� � ������ ������
	// ��� ���������� � ������� ����������� ������������� ��� ���������� ����� (bool key_is_open_ = false)
	ArrayItemContext Builder::StartArray() {

		if (root_ == nullptr) { // ���� ��� ������
			root_ = json::Array(); // �������� ����� �������
			nodes_stack_.push_back(&root_); //���������� ��� � ����
			return ArrayItemContext{ *this };
		}

		else if (nodes_stack_.empty()) { // �� ����� ���� �������� ����� root_ ��� �� ���� � ���� ��� ���� ����
			throw std::logic_error("StartArray() called when root_ not nullptr, but nodes stack is empty");
		}

		// �������� ������������ ������
		json::Node* parrent = nodes_stack_.back();

		if (parrent->IsDict()) { // ���� ������������ ���� ��� �������

			if (!key_is_open_) { // ������ �������� ����� ������ ��� �������� �����
				throw std::logic_error("StartArray() called when parrent root is Dict, and key is not opened");
			}

			else {
				// �������� ����� ������� � �������� ����� �������� �������
				const_cast<json::Dict&>(parrent->AsDict())[key_] = json::Array();
				// �������� ��������� ���� �����
				json::Node* node = &const_cast<json::Dict&>(parrent->AsDict()).at(key_);
				// ���������� �� � ����
				nodes_stack_.push_back(std::move(node));
				// �������� ����
				key_.clear();
				// ��������� ������
				key_is_open_ = false;
			}

		}

		else if (parrent->IsArray()) { // ���� ������������ ���� ��� ������

			// ���������� � ������������ ������ ����� �������
			const_cast<json::Array&>(parrent->AsArray()).push_back(json::Array());
			// �������� ��������� ���� �����
			json::Node* node = &const_cast<json::Array&>(parrent->AsArray()).back();
			// ���������� �� � ����
			nodes_stack_.push_back(std::move(node));
		}

		else { // ���� ������������ ���� �� ��������� ��� ������ ��� �������
			throw std::logic_error("StartArray() called not for empty document, nor for Array element / Dict Value element.");
		}


		return ArrayItemContext{ *this };
	}

	// �������� �������� ��������� �������
	// �������� ����������� ���� ��������� ���� �� ������ (bool key_is_open_ = true)
	CommonContext Builder::EndDict() {

		if (root_ == nullptr) { // ���� ��� �������, ������ ������� ������� ���� ��� �������
			throw std::logic_error("EndDict() called when root_ is nullptr");
		}

		else if (nodes_stack_.empty()) { // ����� ��� ������ �����, ������ ������� ������� �������� ���
			throw std::logic_error("EndDict() called when root_ is not nullptr, but nodes stack is empty");
		}

		// �������� ������������ ������
		json::Node* parrent = nodes_stack_.back();

		if (parrent->IsDict()) { // ���� ����������� ���� - �������

			if (key_is_open_) { // ���� � ������� �� ������ ����
				throw std::logic_error("EndDict() cant be called when json::Dict's is not closed");
			}

			// ������� ������� ������� �����
			nodes_stack_.pop_back();
		}

		else { // ������ ������� �������� ������� ���� � ���� �� �������
			throw std::logic_error("EndDict() called when parrent root is not a json::Dict");
		}

		return CommonContext{ *this };
	}

	// �������� �������� ��������� �������
	CommonContext Builder::EndArray() {

		if (root_ == nullptr) { // ���� ��� �������, ������ ������� ������ ���� ������� ���
			throw std::logic_error("EndArray() called when root_ is nullptr");
		}

		else if (nodes_stack_.empty()) { // ����� ��� ������ �����, ������ ������� ������ �������� ���
			throw std::logic_error("EndArray() called when root_ is not nullptr, but nodes stack is empty");
		}

		// �������� ������������ ������
		json::Node* parrent = nodes_stack_.back();

		if (parrent->IsArray()) { // ���� � ������������ ���� ������

			// ������� ������� ������� �����
			nodes_stack_.pop_back();
		}

		else { // ������ ������� �������� ������� ���� � ���� �� ������
			throw std::logic_error("EndArray() called when parrent root is not a json::Array");
		}

		return CommonContext{ *this };
	}

	// ���������� �������� ������������������� ��������� json::Node
	json::Node Builder::Build() {

		if (root_ == nullptr) {
			// ��������� ���������� � ������� ����
			throw std::logic_error("Build() called for empty document.");
		}

		else if (!nodes_stack_.empty()) {
			// ��������� ���������� � ��� ������, ���� �����-�� �� ���������� ����������� �� ������
			throw std::logic_error("Build() called for not finished document.");
		}

		return std::move(root_);
	}

	// ------------------------------ class Builder END ------------------------------------

} // namespace json