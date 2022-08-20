#include "json.h"
#include <sstream>

using namespace std;

namespace json {

    using namespace std::literals;

    namespace {

        using Number = std::variant<int, double>;

        Node LoadNode(istream& input);    // ��������������� ����������

        // ������ ����� � ������
        Number NumberParser(std::istream& input) {
            //using namespace std::literals;

            std::string parsed_num;

            // ��������� � parsed_num ��������� ������ �� input
            auto read_char = [&parsed_num, &input] {
                parsed_num += static_cast<char>(input.get());
                if (!input) {
                    throw ParsingError("Failed to read number from stream"s);
                }
            };

            // ��������� ���� ��� ����� ���� � parsed_num �� input
            auto read_digits = [&input, read_char] {
                if (!std::isdigit(input.peek())) {
                    throw ParsingError("A digit is expected"s);
                }
                while (std::isdigit(input.peek())) {
                    read_char();
                }
            };

            if (input.peek() == '-') {
                read_char();
            }
            // ������ ����� ����� �����
            if (input.peek() == '0') {
                read_char();
                // ����� 0 � JSON �� ����� ���� ������ �����
            }
            else {
                read_digits();
            }

            bool is_int = true;
            // ������ ������� ����� �����
            if (input.peek() == '.') {
                read_char();
                read_digits();
                is_int = false;
            }

            // ������ ���������������� ����� �����
            if (int ch = input.peek(); ch == 'e' || ch == 'E') {
                read_char();
                if (ch = input.peek(); ch == '+' || ch == '-') {
                    read_char();
                }
                read_digits();
                is_int = false;
            }

            try {
                if (is_int) {
                    // ������� ������� ������������� ������ � int
                    try {
                        return std::stoi(parsed_num);
                    }
                    catch (...) {
                        // � ������ �������, ��������, ��� ������������,
                        // ��� ���� ��������� ������������� ������ � double
                    }
                }
                return std::stod(parsed_num);
            }
            catch (...) {
                throw ParsingError("Failed to convert "s + parsed_num + " to number"s);
            }
        }
        // ����������� ������ ��� ���������� ��������� �������� � ����
        std::string LiteralsParser(std::istream& input) {
            std::string result;
            while (std::isalpha(input.peek()))
            {
                result.push_back(static_cast<char>(input.get()));
            }
            return result;
        }
        // ����������� ������ - ������ �����
        std::string StringParser(std::istream& input) {
            using namespace std::literals;

            auto it = std::istreambuf_iterator<char>(input);
            auto end = std::istreambuf_iterator<char>();
            std::string s;
            while (true) {
                if (it == end) {
                    // ����� ���������� �� ����, ��� ��������� ����������� �������?
                    throw ParsingError("String parsing error");
                }
                const char ch = *it;
                if (ch == '"') {
                    // ��������� ����������� �������
                    ++it;
                    break;
                }
                else if (ch == '\\') {
                    // ��������� ������ escape-������������������
                    ++it;
                    if (it == end) {
                        // ����� ���������� ����� ����� ������� �������� ����� �����
                        throw ParsingError("String parsing error");
                    }
                    const char escaped_char = *(it);
                    // ������������ ���� �� �������������������: \\, \n, \t, \r, \"
                    switch (escaped_char) {
                    case 'n':
                        s.push_back('\n');
                        break;
                    case 't':
                        s.push_back('\t');
                        break;
                    case 'r':
                        s.push_back('\r');
                        break;
                    case '"':
                        s.push_back('"');
                        break;
                    case '\\':
                        s.push_back('\\');
                        break;
                    default:
                        // ��������� ����������� escape-������������������
                        throw ParsingError("Unrecognized escape sequence \\"s + escaped_char);
                    }
                }
                else if (ch == '\n' || ch == '\r') {
                    // ��������� ������� ������- JSON �� ����� ����������� ��������� \r ��� \n
                    throw ParsingError("Unexpected end of line"s);
                }
                else {
                    // ������ ��������� ��������� ������ � �������� ��� � �������������� ������
                    s.push_back(ch);
                }
                ++it;
            }

            return s;
        }

        // �������� Node �������� ���� ������������� � ������� ������� LoadNode

        Node LoadNull(istream& input) {
            std::string check = LiteralsParser(input);
            if (check == "null"sv) {
                return Node(nullptr);
            }
            else {
                throw ParsingError("Null parsing error");
            }

        }

        Node LoadBoolean(std::istream& input) {
            std::string check = LiteralsParser(input);
            if (check == "true"sv) {
                return Node(true);
            }
            else if (check == "false"sv) {
                return Node(false);
            }
            else {
                throw ParsingError("Boolean parsing error");
            }
        }

        Node LoadNumber(istream& input) {
            Number result = NumberParser(input);
            if (std::holds_alternative<int>(result)) {
                return Node(std::get<int>(result));
            }
            else if (std::holds_alternative<double>(result)) {
                return Node(std::get<double>(result));
            }
            else {
                throw ParsingError("Number parsing error");
            }
        }

        Node LoadString(istream& input) {
            string line = StringParser(input);
            return Node(move(line));
        }

        Node LoadArray(istream& input) {
            Array result;

            auto it = std::istreambuf_iterator<char>(input);
            auto end = std::istreambuf_iterator<char>();

            if (it == end) {
                throw ParsingError("Array parsing error");
            }

            for (char c; input >> c && c != ']';) {
                if (c != ',') {
                    input.putback(c);
                }
                result.push_back(LoadNode(input));
            }

            return Node(move(result));
        }

        Node LoadDict(istream& input) {
            Dict result;

            auto it = std::istreambuf_iterator<char>(input);
            auto end = std::istreambuf_iterator<char>();

            if (it == end) {
                throw ParsingError("Dict parsing error");
            }

            for (char c; input >> c && c != '}';) {

                if (c == ',') {
                    input >> c;
                }

                string key = LoadString(input).AsString();
                input >> c;
                result.insert({ move(key), LoadNode(input) });
            }

            return Node(move(result));
        }

        Node LoadNode(istream& input) {
            char c;
            if (!(input >> c)) {
                throw ParsingError("Zero input_stream");
            }

            switch (c)
            {
            case '[':
                return LoadArray(input);

            case '{':
                return LoadDict(input);

            case '"':
                return LoadString(input);

            case 'n':
                input.putback(c);
                return LoadNull(input);

            case 't':
                input.putback(c);
                return LoadBoolean(input);

            case 'f':
                input.putback(c);
                return LoadBoolean(input);

            default:
                input.putback(c);
                return LoadNumber(input);
            }
        }

    }  // namespace


    // --------- class Node ---------------

    Node::Node(Value value)
        : variant(std::move(value)) {
    }

    // --------- ��������� �������� -------

    const Value& Node::GetValue() const {
        return *this;
    }

    Value& Node::GetValue() {
        return *this;
    }

    int Node::AsInt() const {
        if (!IsInt()) {
            throw std::logic_error("This not an Int");
        }
        return std::get<int>(*this);
    }

    bool Node::AsBool() const {
        if (!IsBool()) {
            throw std::logic_error("This not a Bool");
        }
        return std::get<bool>(*this);
    }

    double Node::AsDouble() const {
        if (!IsInt() && !IsPureDouble()) {
            throw std::logic_error("This not a Double");
        }
        else {
            double result_;
            if (IsInt()) {
                int int_result_ = std::get<int>(*this);
                result_ = int_result_;
            }
            else {
                result_ = std::get<double>(*this);
            }
            return result_;
        }
    }

    const std::string& Node::AsString() const {
        if (!IsString()) {
            throw std::logic_error("This not a String");
        }
        return std::get<std::string>(*this);
    }

    const Array& Node::AsArray() const {
        if (!IsArray()) {
            throw std::logic_error("This not a Array");
        }
        return std::get<Array>(*this);
    }

    const Dict& Node::AsMap() const {
        if (!IsMap()) {
            throw std::logic_error("This not a Dict");
        }
        return std::get<Dict>(*this);
    }

    // --------- bool �������� ------------

    bool Node::IsInt() const {
        return std::holds_alternative<int>(*this);
    }

    bool Node::IsDouble() const {
        return IsInt() || IsPureDouble();
    }

    bool Node::IsPureDouble() const {
        return std::holds_alternative<double>(*this);
    }

    bool Node::IsBool() const {
        return std::holds_alternative<bool>(*this);
    }

    bool Node::IsString() const {
        return std::holds_alternative<std::string>(*this);
    }

    bool Node::IsNull() const {
        return std::holds_alternative<std::nullptr_t>(*this);
    }

    bool Node::IsArray() const {
        return std::holds_alternative<Array>(*this);
    }

    bool Node::IsMap() const {
        return std::holds_alternative<Dict>(*this);
    }

    bool Node::operator==(const Node& other) const {
        return GetValue() == other.GetValue();
    }

    bool Node::operator!=(const Node& other) const {
        return GetValue() != other.GetValue();
    }

    // --------- class Document -----------

    Document::Document(Node root)
        : root_(move(root)) {
    }

    const Node& Document::GetRoot() const {
        return root_;
    }

    bool Document::operator==(const Document& other) const {
        return root_ == other.GetRoot();
    }

    bool Document::operator!=(const Document& other) const {
        return root_ != other.GetRoot();
    }

    Document Load(istream& input) {
        return Document{ LoadNode(input) };
    }

    // --------- struct PrintContext ------

    void PrintContext::PrintIndent() const {
        for (int i = 0; i < _indent; ++i) {
            _out.put(' ');
        }
    }

    template <typename Value>
    void PrintValue(const Value& value, const PrintContext& ctx) {
        //ctx._out << value;
        PrintValue(value, ctx);
    }

    void PrintValue(std::nullptr_t, const PrintContext& ctx) {
        ctx._out << "null"sv;
    }

    void PrintValue(const std::string& line, const PrintContext& ctx) {
        ctx._out.put('"');
        for (const char c : line) {
            switch (c)
            {
            case '\r':
                ctx._out << "\\r"sv;
                break;
            case '\n':
                ctx._out << "\\n"sv;
                break;
            case '"':
                // ������� " � \ ��������� ��� \" ��� \\, ��������������
                [[fallthrough]];
            case '\\':
                ctx._out.put('\\');
                [[fallthrough]];
            default:
                ctx._out.put(c);
                break;
            }
        }
        ctx._out.put('"');
    }

    void PrintValue(int num, const PrintContext& ctx) {
        ctx._out << num;
    }

    void PrintValue(double num, const PrintContext& ctx) {
        ctx._out << num;
    }

    void PrintValue(bool boolean, const PrintContext& ctx) {
        boolean ? ctx._out << "true"sv : ctx._out << "false"sv;
    }

    void PrintValue(const Array& array, const PrintContext& ctx) {
        auto& out = ctx._out;
        bool IsFirst = true;
        out.put('[');
        out.put(' ');
        for (auto& item : array) {
            if (!IsFirst) {
                out.put(',');
                out.put(' ');
            }
            else {
                IsFirst = false;
            }
            std::visit(
                [&out, &item](const auto& value) {
                    PrintValue(value, PrintContext(out, 0, 0)); },
                item.GetValue());
        }
        out.put(' ');
        out.put(']');
    }

    void PrintValue(const Dict& dict, const PrintContext& ctx) {
        auto& out = ctx._out;
        bool IsFirst = true;
        out.put('{');
        out.put(' ');
        for (auto& item : dict) {
            if (!IsFirst) {
                out.put(',');
                out.put(' ');
            }
            else {
                IsFirst = false;
            }
            PrintValue(item.first, PrintContext(out, 0, 0));
            out << " : "sv;
            std::visit(
                [&out, &item](const auto& value) {
                    PrintValue(value, PrintContext(out, 0, 0)); },
                item.second.GetValue());
        }
        out.put(' ');
        out.put('}');
    }

    // --------- struct ValuePrinter ------

    void ValuePrinter::operator()(std::nullptr_t) const {
        out << "null"sv;
    }

    void ValuePrinter::operator()(int num) const {
        out << num;
    }

    void ValuePrinter::operator()(double num) const {
        out << num;
    }

    void ValuePrinter::operator()(bool boolean) const {
        boolean ? out << "true"sv : out << "false"sv;
    }

    void ValuePrinter::operator()(const std::string& line) const {
        out.put('"');
        for (const char c : line) {
            switch (c)
            {
            case '\r':
                out << "\\r"sv;
                break;
            case '\n':
                out << "\\n"sv;
                break;
            case '"':
                // ������� " � \ ��������� ��� \" ��� \\, ��������������
                [[fallthrough]];
            case '\\':
                out.put('\\');
                [[fallthrough]];
            default:
                out.put(c);
                break;
            }
        }
        out.put('"');
    }

    void ValuePrinter::operator()(const Array& array) const {
        bool IsFirst = true;
        out.put('[');
        out.put(' ');
        for (auto& item : array) {
            if (!IsFirst) {
                out.put(',');
                out.put(' ');
            }
            else {
                IsFirst = false;
            }
            auto val = item.GetValue();
            std::visit(ValuePrinter{ out }, val);
        }
        out.put(' ');
        out.put(']');
    }

    void ValuePrinter::operator()(const Dict& dict) const {
        bool IsFirst = true;
        out.put('{');
        out.put(' ');
        for (auto& item : dict) {
            if (!IsFirst) {
                out.put(',');
                out.put(' ');
            }
            else {
                IsFirst = false;
            }
            PrintValue(item.first, PrintContext(out, 0, 0));
            out << " : "sv;
            auto val = item.second.GetValue();
            std::visit(ValuePrinter{ out }, val);
        }
        out.put(' ');
        out.put('}');
    }

    void Print(const Document& doc, std::ostream& output) {
        // ������� ����� ValuePrinter, ������-�� �������� ��������� � ������� �� 15%
        // ���������������� ���������� �� ���� ����, ��� ������ ��� �������� ���������
        // �� �������� ��������� �������� ������� � �����
        //std::visit(ValuePrinter{ PrintContext(output, 4, 0).GetContex()._out}, doc.GetRoot().GetValue());

        // ������� ����� ���������� ������� PrintValue, �������� � ������� �� 15% �������
        std::visit(
            [&output](const auto& value) {
                PrintValue(value, PrintContext(output, 4, 0)); }, doc.GetRoot().GetValue());
    }

}  // namespace json