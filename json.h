#pragma once

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <variant>
#include <optional>

namespace json {

    class Node;
    // ��������� ���������� Dict � Array ��� ���������
    using Dict = std::map<std::string, Node>;
    using Array = std::vector<Node>;
    using Value = std::variant<std::nullptr_t, Array, Dict, bool, int, double, std::string>;

    // ��� ������ ������ ������������� ��� ������� �������� JSON
    class ParsingError : public std::runtime_error {
    public:
        using runtime_error::runtime_error;
    };

    class Node final
        : private std::variant<std::nullptr_t, Array, Dict, bool, int, double, std::string>
    {
    public:

        using variant::variant;
        using Value = variant;

        Node(Value value);

        // --------- ��������� ��������--------

        const Value& GetValue() const;
        Value& GetValue();

        int AsInt() const;
        bool AsBool() const;
        double AsDouble() const;
        const std::string& AsString() const;
        const Array& AsArray() const;
        const Dict& AsMap() const;

        // --------- bool �������� ------------

        bool IsInt() const;
        bool IsDouble() const;
        bool IsPureDouble() const;
        bool IsBool() const;
        bool IsString() const;
        bool IsNull() const;
        bool IsArray() const;
        bool IsMap() const;

        bool operator==(const Node& other) const;
        bool operator!=(const Node& other) const;

    private:
        Value _value = nullptr;
    };

    class Document {
    public:
        Document() = default;
        explicit Document(Node root);

        const Node& GetRoot() const;

        bool operator==(const Document& other) const;
        bool operator!=(const Document& other) const;

    private:
        Node root_;
    };

    Document Load(std::istream& input);

    // �������� ������, ������ ������ �� ����� ������ � ������� �����
    struct PrintContext {
        PrintContext(std::ostream& out) : _out(out) {
        }

        PrintContext(std::ostream& out, int indent_step, int indent = 0)
            : _out(out)
            , _indent_step(indent_step)
            , _indent(indent) {
        }

        std::ostream& _out;
        int _indent_step = 4;
        int _indent = 0;

        void PrintIndent() const;

        // ���������� ����� �������� ������ � ����������� ���������
        PrintContext Indented() const {
            return { _out, _indent_step, _indent_step + _indent };
        }

        PrintContext& GetContex() {
            return *this;
        }
    };

    // ��������� ������ �������� � ������ ��� ����������� ������ � std::visit
    struct ValuePrinter {
        std::ostream& out;

        void operator()(std::nullptr_t) const;                   // ������ ������� ��������
        void operator()(int num) const;                          // ������ ������������� ��������
        void operator()(double num) const;                       // ������ ��������������� ��������
        void operator()(bool boolean) const;                     // ������ ��������
        void operator()(const std::string& line) const;          // ������ �����
        void operator()(const Array& array) const;               // ������ ��������
        void operator()(const Dict& dict) const;                 // ������ ��������
    };

    void Print(const Document& doc, std::ostream& output);

}  // namespace json