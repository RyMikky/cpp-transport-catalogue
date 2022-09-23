#pragma once

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <variant>
#include <optional>

namespace json {

    class Node;
 
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

        const Value& GetValue() const;                                // ������� �������� �����
        Value& GetValue();                                            // ������� �������� �����

        int AsInt() const;                                            // �������� �������� int
        bool AsBool() const;                                          // �������� �������� bool
        double AsDouble() const;                                      // �������� �������� double
        const std::string& AsString() const;                          // �������� �������� std::string
        const Array& AsArray() const;                                 // �������� �������� std::vector<Node>
        const Dict& AsDict() const;                                   // �������� �������� std::map<std::string, Node>

        // --------- bool �������� ------------

        bool IsInt() const;                                           // ��������� ����� �� � Node ��� int
        bool IsDouble() const;                                        // ��������� ����� �� � Node ��� double ��� int
        bool IsPureDouble() const;                                    // ��������� ����� �� � Node ��� double
        bool IsBool() const;                                          // ��������� ����� �� � Node ��� bool
        bool IsString() const;                                        // ��������� ����� �� � Node ��� std::string
        bool IsNull() const;                                          // ��������� ����� �� � Node ��� nullptr
        bool IsArray() const;                                         // ��������� ����� �� � Node ��� std::vector<Node>
        bool IsDict() const;                                          // ��������� ����� �� � Node ��� std::map<std::string, Node>

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

    void Print(const Document& doc, std::ostream& output);

}  // namespace json