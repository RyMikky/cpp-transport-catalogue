#pragma once

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <variant>
#include <optional>

namespace json {

    class Node;
    // Сохраните объявления Dict и Array без изменения
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

        const Value& GetValue() const;
        Value& GetValue();

        int AsInt() const;
        bool AsBool() const;
        double AsDouble() const;
        const std::string& AsString() const;
        const Array& AsArray() const;
        const Dict& AsMap() const;

        // --------- bool операции ------------

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

    // структура вывода значений в печать для последующей работы с std::visit
    struct ValuePrinter {
        std::ostream& out;

        void operator()(std::nullptr_t) const;                   // печать нулевых значений
        void operator()(int num) const;                          // печать целочисленных значений
        void operator()(double num) const;                       // печать нецелочисленных значений
        void operator()(bool boolean) const;                     // печать булеанов
        void operator()(const std::string& line) const;          // печать строк
        void operator()(const Array& array) const;               // печать массивов
        void operator()(const Dict& dict) const;                 // печать словарей
    };

    void Print(const Document& doc, std::ostream& output);

}  // namespace json