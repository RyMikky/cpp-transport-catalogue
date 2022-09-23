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

    // Эта ошибка должна выбрасываться при ошибках парсинга JSON
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

        // --------- получение знаечний--------

        const Value& GetValue() const;                                // вернуть значение корня
        Value& GetValue();                                            // вернуть значение корня

        int AsInt() const;                                            // получить значение int
        bool AsBool() const;                                          // получить значение bool
        double AsDouble() const;                                      // получить значение double
        const std::string& AsString() const;                          // получить значение std::string
        const Array& AsArray() const;                                 // получить значение std::vector<Node>
        const Dict& AsDict() const;                                   // получить значение std::map<std::string, Node>

        // --------- bool операции ------------

        bool IsInt() const;                                           // проверить лежит ли в Node тип int
        bool IsDouble() const;                                        // проверить лежит ли в Node тип double или int
        bool IsPureDouble() const;                                    // проверить лежит ли в Node тип double
        bool IsBool() const;                                          // проверить лежит ли в Node тип bool
        bool IsString() const;                                        // проверить лежит ли в Node тип std::string
        bool IsNull() const;                                          // проверить лежит ли в Node тип nullptr
        bool IsArray() const;                                         // проверить лежит ли в Node тип std::vector<Node>
        bool IsDict() const;                                          // проверить лежит ли в Node тип std::map<std::string, Node>

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

    // Контекст вывода, хранит ссылку на поток вывода и текущий отсуп
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

        // Возвращает новый контекст вывода с увеличенным смещением
        PrintContext Indented() const {
            return { _out, _indent_step, _indent_step + _indent };
        }

        PrintContext& GetContex() {
            return *this;
        }
    };

    void Print(const Document& doc, std::ostream& output);

}  // namespace json