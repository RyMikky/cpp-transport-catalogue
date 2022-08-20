#include "svg.h"

namespace svg {

    using namespace std::literals;

    std::ostream& operator<<(std::ostream& out, const svg::Rgb& rgb) {
        out << "rgb"sv;
        out << "("sv << rgb.red << ","sv << rgb.green 
            << ","sv << rgb.blue << ")"sv;
        return out;
    }

    std::ostream& operator<<(std::ostream& out, const svg::Rgba& rgba) {
        out << "rgba"sv;
        out << "("sv << rgba.red << ","sv << rgba.green
            << ","sv << rgba.blue << ","sv << rgba.opacity << ")"sv;
        return out;
    }

    std::ostream& operator<<(std::ostream& out, const svg::Color& color) {
        std::visit(ColorPrinter{ out }, color);
        return out;
    }

    std::ostream& operator<<(std::ostream& out, const svg::StrokeLineCap& line_cap) {
        out << LINE_CAP_MAP.at(line_cap);
        return out;
    }

    std::ostream& operator<<(std::ostream& out, const svg::StrokeLineJoin& line_join) {
        out << LINE_JOIN_MAP.at(line_join);
        return out;
    }

    // ---------- Object ------------------

    void Object::Render(const RenderContext& context) const {
        context.RenderIndent();

        // Делегируем вывод тега своим подклассам
        RenderObject(context);

        context.out << std::endl;
    }

    // ---------- ObjectContainer ---------


    // ---------- Circle ------------------

    Circle& Circle::SetCenter(Point center) {
        center_ = center;
        return *this;
    }

    Circle& Circle::SetRadius(double radius) {
        radius_ = radius;
        return *this;
    }

    void Circle::RenderObject(const RenderContext& context) const {
        auto& out = context.out;
        out << "<circle cx=\""sv << center_.x << "\" cy=\""sv << center_.y << "\" "sv;
        out << "r=\""sv << radius_ << "\""sv;
        RenderAttrs(context.out);
        out << "/>"sv;
    }

    // ---------- Polyline ----------------

    Polyline& Polyline::AddPoint(Point point) {
        _tops_points.push_back(point);
        return *this;
    }

    void Polyline::RenderObject(const RenderContext& context) const {
        auto& out = context.out;
        out << "<polyline points=\""sv;
        bool IsFirst = true;
        for (auto& point : _tops_points) {
            if (IsFirst) {
                out << point.x << ","sv << point.y;
                IsFirst = false;
            }
            else {
                out << " "sv << point.x << ","sv << point.y;
            }
        }
        out << "\""sv;
        RenderAttrs(context.out);
        out << " />"sv;
    }

    // ---------- Text --------------------

    Text& Text::SetPosition(Point pos) {
        _position = pos;
        return *this;
    }

    Text& Text::SetOffset(Point offset) {
        _offset = offset;
        return *this;
    }

    Text& Text::SetFontSize(uint32_t size) {
        _font_size = size;
        return *this;
    }

    Text& Text::SetFontFamily(std::string font_family) {
        _font_family = std::move(font_family);
        return *this;
    }

    Text& Text::SetFontWeight(std::string font_weight) {
        _font_weight = std::move(font_weight);
        return *this;
    }

    Text& Text::SetData(std::string data) {
        _text_data = std::move(data);
        return *this;
    }

    std::string Text::TextEncode(const std::string& text) const {
        std::string result;
        for (char c : text) {
            auto it = _symbols_encode.find(c);
            if (it != _symbols_encode.end()) {
                result += it->second;
            }
            else {
                result += c;
            }
        }

        return result;
    }

    void Text::RenderObject(const RenderContext& context) const {
        auto& out = context.out;
        out << "<text"sv;
        RenderAttrs(context.out);
        out << " x=\""sv << _position.x << "\" y=\""sv << _position.y << "\""sv;
        out << " dx=\""sv << _offset.x << "\" dy=\""sv << _offset.y << "\""sv;
        out << " font-size=\""sv << _font_size << "\""sv;
        if (_font_family.size() > 0) {
            out << " font-family=\""sv << _font_family << "\""sv;
        }
        if (_font_weight.size() > 0) {
            out << " font-weight=\""sv << _font_weight << "\""sv;
        }
        out << ">"sv << TextEncode(_text_data) << "</text>"sv;
    }

    // ---------- Document ----------------

    void Document::AddPtr(std::unique_ptr<Object>&& obj) {
        _objects.push_back(std::move(obj));
    }

    void Document::Render(std::ostream& out) const {
        RenderContext context_ = RenderContext(out, 0, 2);

        out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"sv << std::endl;
        out << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">"sv << std::endl;
        context_.indent += context_.indent_step;
        for (size_t i = 0; i < _objects.size(); ++i)
        {
            _objects[i]->Render(context_);
        }
        context_.indent -= context_.indent_step;
        out << "</svg>"sv << std::endl;
    }

}  // namespace svg