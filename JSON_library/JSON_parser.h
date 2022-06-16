#pragma once
#include <exception>
#include <variant>
#include <istream>
#include <ostream>
#include <string>
#include <vector>
#include <map>
#include <functional>

class Node;

using Dict = std::map<std::string, Node>;
using Array = std::vector<Node>;
using namespace std::string_literals;

class ParsingError : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

class Node final : private std::variant<std::nullptr_t, Array, Dict, bool, int, double, std::string> {
public:
    using variant::variant;
    using Value = variant;

    Node(Node::Value& value) {
        this->swap(value);
    }

    bool IsInt() const {
        return std::holds_alternative<int>(*this);
    }

    int AsInt() const {
        if (!IsInt()) {
            throw std::logic_error("not as int");
        }
        return std::get<int>(*this);
    }

    bool IsPureDouble() const {
        return std::holds_alternative<double>(*this);
    }

    bool IsDouble() const {
        return IsPureDouble() || IsInt();
    }

    double AsDouble() const {
        using namespace std::literals;
        if (!IsDouble())
            throw std::logic_error("not as double");
        if (IsPureDouble())
            return std::get<double>(*this);
        else
            return AsInt();
    }

    bool IsBool() const {
        return std::holds_alternative<bool>(*this);
    }

    bool AsBool() const {
        if (!IsBool())
            throw std::logic_error("not as bool");
        else
            return std::get<bool>(*this);
    }

    bool IsNull() const {
        return std::holds_alternative<nullptr_t>(*this);
    }

    bool IsArray() const {
        return std::holds_alternative<Array>(*this);
    }

    const Array& AsArray() const {
        using namespace std::literals;
        if (!IsArray())
            throw std::logic_error("not as array");
        else
            return std::get<Array>(*this);
    }

    bool IsString() const {
        return std::holds_alternative<std::string>(*this);
    }

    const std::string& AsString() const {
        using namespace std::literals;
        if (!IsString())
            throw std::logic_error("not as string");
        else
            return std::get<std::string>(*this);
    }

    bool IsDict() const{
        return std::holds_alternative<Dict>(*this);
    }

    const Dict& AsDict() const {
        using namespace std::literals;
        if (!IsDict())
            throw std::logic_error("not as dict");
        else
            return std::get<Dict>(*this);
    }

    const Value &GetValue() const {
        return *this;
    }

    bool operator==(const Node& rhs) const{
        if(this->IsNull())
            return true;
        if(this->IsArray())
            return this->AsArray() == rhs.AsArray();
        if(this->IsDict())
            return this->AsDict() == rhs.AsDict();
        if(this->IsBool())
            return this->AsBool() == rhs.AsBool();
        if(this->IsInt())
            return this->AsInt() == rhs.AsInt();
        if(this->IsDouble())
            return this->AsDouble() == rhs.AsDouble();
        if(this->IsString())
            return this->AsString() == rhs.AsString();
        else
            std::logic_error("== OPERATOR trow from Node class");
    }

};

inline bool operator!=(const Node& lhs, const Node& rhs) {
    if(lhs.IsNull())
        return false;
    if(lhs.IsArray())
        return lhs.AsArray() != rhs.AsArray();
    if(lhs.IsDict())
        return lhs.AsDict() != rhs.AsDict();
    if(lhs.IsBool())
        return lhs.AsBool() != rhs.AsBool();
    if(lhs.IsInt())
        return lhs.AsInt() != rhs.AsInt();
    if(lhs.IsDouble())
        return lhs.AsDouble() != rhs.AsDouble();
    if(lhs.IsString())
        return lhs.AsString() != rhs.AsString();
    else
        throw std::logic_error("!= OPERATOR trow for Node class");
}

class Document
{
public:
    explicit Document(Node root) : root_(std::move(root)) {}

    const Node& GetRoot() const {
        return root_;
    }

private:
    Node root_;
};

inline bool operator==(const Document& lhs, const Document& rhs) {
    return lhs.GetRoot() == rhs.GetRoot();
}

inline bool operator!=(const Document& lhs, const Document& rhs) {
    return lhs.GetRoot() != rhs.GetRoot();
}

Node Load_Node(std::istream &input);

Node Load_string(std::istream &input) {
    auto iterator = std::istreambuf_iterator<char>(input);
    auto end = std::istreambuf_iterator<char>();
    std::string string_contain;
    while (true) {
        if (iterator == end)
            throw ParsingError("problem with string loading process!");
        const char current_symbol = *iterator;
        if (current_symbol == '"') {
            ++iterator;
            break;
        } else if (current_symbol == '\\') {
            ++iterator;
            if (iterator == end)
                throw ParsingError("problem with string loading process!");
            const char escape_symbol = *iterator;
            switch (escape_symbol) {
                case 'n':
                    string_contain.push_back('\n');
                    break;
                case 't':
                    string_contain.push_back('\t');
                    break;
                case 'r':
                    string_contain.push_back('\r');
                    break;
                case '"':
                    string_contain.push_back('"');
                    break;
                case '\\':
                    string_contain.push_back('\\');
                    break;
                default:
                    throw ParsingError("not recognized escape symbol!");
            }
        } else if (current_symbol == '\n' || current_symbol == '\r')
            throw ParsingError("unexpected endl!");
        else
            string_contain.push_back(current_symbol);
        ++iterator;
    }
    return Node(std::move(string_contain));
}

std::string Load_literal(std::istream &input) {
    std::string string_literal_contain;
    while (std::isalpha(input.peek()))
        string_literal_contain.push_back(static_cast<char>(input.get()));
    return string_literal_contain;
}

Node Load_bool(std::istream &input) {
    const auto logic_literal = Load_literal(input);
    if (logic_literal == "true"s)
        return Node{true};
    else if (logic_literal == "false"s)
        return Node{false};
    else
        throw ParsingError("problem with loading logic literal!");
}

Node Load_null(std::istream &input) {
    auto null_literal = Load_literal(input);
    if (null_literal == "null"s)
        return Node{nullptr};
    else
        throw ParsingError("problem with loading null literal!");
}

Node Load_number(std::istream &input) {
    std::string parsed_num;
    auto read_char = [&parsed_num, &input] {
        parsed_num += (static_cast<char>(input.get()));
        if (!input)
            throw ParsingError("problem with loading number!");
    };
    auto read_number = [&input, read_char] {
        if (!std::isdigit(static_cast<unsigned char>(input.peek())))
            throw ParsingError("problem with loading number!");
        while (std::isdigit(input.peek())) {
            read_char();
        }
    };
    if(input.peek() == '-')
        read_char();
    if(input.peek() == '0')
        read_char();
    else
        read_number();
    bool is_int = true;
    if (input.peek() == '.') {
        read_char();
        read_number();
        is_int = false;
    }
    if (int ch = input.peek(); ch == 'e' || ch == 'E') {
        read_char();
        if (int ch = input.peek(); ch == '+' || ch == '-') {
            read_char();
        }
        read_number();
        is_int = false;
    }
    if (is_int)
        return std::stoi(parsed_num);
    else
        return std::stod(parsed_num);
}

Node Load_array(std::istream &input) {
    std::vector<Node> contain;
    for (char ch; input >> ch && ch != ']';) {
        if (ch != ',')
            input.putback(ch);
        contain.push_back(Load_Node(input));
    }
    if (!input)
        throw ParsingError("problem with loading Array!");
    return Node(std::move(contain));
}

Node Load_dict(std::istream &input) {
    Dict contain;
    for (char ch; input >> ch && ch != '}';) {
        if (ch == '"') {
            std::string key = Load_string(input).AsString();
            if (contain.find(key) == contain.end()) {
                if (input >> ch && ch == ':') {
                    Node value = Load_Node(input);
                    contain.emplace(std::move(key), value);
                }
                else
                    throw ParsingError("':' not found!_(problem with loading Dict)");
            } else
                throw ParsingError("key declaration error_(problem with loading Dict)!");
        } else if (ch != ',')
            throw ParsingError("',' not found!_(problem with loading Dict)");
    }
    if (!input)
        throw ParsingError("problem with loading Dict!");
    return Node(std::move(contain));
}

Node Load_Node(std::istream &input) {
    char buff;
    if (!input)
        throw ParsingError("stream error!");
    input >> buff;
    switch (buff) {
        case '"':
            return Load_string(input);
        case 't':
            input.putback(buff);
            return Load_bool(input);
        case 'f':
            input.putback(buff);
            return Load_bool(input);
        case 'n':
            input.putback(buff);
            return Load_null(input);
        case '[':
            return Load_array(input);
        case '{':
            return Load_dict(input);
        default:
            input.putback(buff);
            return Load_number(input);
    }
}

Document Load(std::istream& input) {
    return Document(Load_Node(input));
}

struct context_for_lamda {
    std::ostream &out;
    int indent_step = 4;
    int indent = 1;
    void PrintIdent() const {
        for (int i = 0; i < indent; ++i) {
            out.put('\t');
        }
    }
    context_for_lamda Indented() const {
        return {out, indent_step, indent};
    }
};

template<typename Value>
void PrintValue(const Value &value, const context_for_lamda &ctx) {
    ctx.out << value;
}

void PrintString(const std::string &value, std::ostream &out) {
    out.put('"');
    for (const char ch: value) {
        switch (ch) {
            case '\r' :
                out << "\\r"s;
                break;
            case '\n' :
                out << "\\n"s;
                break;
            case '"' :
                [[fallthrough]];
            case '\\':
                out.put('\\');
                [[fallthrough]];
            default:
                out << ch;
                break;
        }
    }
    out.put('"');
}

void PrintNode(const Node &node, const context_for_lamda &ctx);


void PrintValue(const std::string &value, const context_for_lamda &ctx) {
    PrintString(value, ctx.out);
}

void PrintValue(const std::nullptr_t &value, const context_for_lamda &ctx) {
    ctx.out << "null"s;
}

void PrintValue(const bool &value, const context_for_lamda &ctx) {
    ctx.out << (value ? "true"s : "false"s);
}

void PrintValue(const Array &nodes, const context_for_lamda &ctx) {
    std::ostream &out = ctx.out;
    out << "[\n"s;
    bool first = true;
    auto inner_ctx = ctx.Indented();
    for (const Node &node: nodes) {
        if (first) {
            first = false;
        } else {
            out << ",\n"s;
        }
        inner_ctx.PrintIdent();
        PrintNode(node, inner_ctx);
    }
    out.put('\n');
    out.put(']');
}

void PrintValue(const Dict &nodes, const context_for_lamda &ctx) {
    std::ostream &out = ctx.out;
    out << "{\n"s;
    bool first = true;
    auto inner_ctx = ctx.Indented();
    for (const auto &[key, node]: nodes) {
        if (first) {
            first = false;
        } else out << ",\n"s;
        inner_ctx.PrintIdent();
        PrintString(key, ctx.out);
        out << ": "s;
        PrintNode(node, inner_ctx);
    }
    ctx.PrintIdent();
    out << "\n}"s;
}

void PrintNode(const Node &node, const context_for_lamda &ctx) {
    std::visit([&ctx](const auto &value) {PrintValue(value, ctx);}, node.GetValue());
}

void Print(const Document& doc, std::ostream& output){
    PrintNode(doc.GetRoot(), context_for_lamda{output});
}