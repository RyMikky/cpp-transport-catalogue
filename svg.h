#pragma once

#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <optional>
#include <variant>

using namespace std::literals;

namespace svg {

    struct Rgb {
        Rgb() = default;

        Rgb(uint16_t r, uint16_t g, uint16_t b) : red(r), green(g), blue(b) {
        }

        uint16_t red = 0;
        uint16_t green = 0;
        uint16_t blue = 0;
    };

    std::ostream& operator<<(std::ostream&, const svg::Rgb&);

    struct Rgba
    {
        Rgba() = default;

        Rgba(uint16_t r, uint16_t g, uint16_t b, double o)
            : red(r), green(g), blue(b), opacity(o) {
        }

        uint16_t red = 0;
        uint16_t green = 0;
        uint16_t blue = 0;
        double opacity = 1.0;
    };

    std::ostream& operator<<(std::ostream&, const svg::Rgba&);

    using Color = std::variant<std::monostate, std::string, svg::Rgb, svg::Rgba>;
    inline const Color NoneColor{ "none" };

    struct ColorPrinter
    {
        std::ostream& out;

        void operator()(std::monostate) const {
            out << "none"sv;
        }

        void operator()(std::string color) const {
            out << color;
        }

        void operator()(svg::Rgb color) const {
            out << color;
        }

        void operator()(svg::Rgba color) const {
            out << color;
        }
    };

    std::ostream& operator<<(std::ostream&, const svg::Color&);

    enum class StrokeLineCap {
        BUTT,
        ROUND,
        SQUARE,
    };

    const std::unordered_map<svg::StrokeLineCap, std::string> LINE_CAP_MAP = {
        {StrokeLineCap::BUTT, "butt"}, {StrokeLineCap::ROUND, "round"}, {StrokeLineCap::SQUARE, "square"}
    };

    enum class StrokeLineJoin {
        ARCS,
        BEVEL,
        MITER,
        MITER_CLIP,
        ROUND,
    };

    const std::unordered_map<svg::StrokeLineJoin, std::string> LINE_JOIN_MAP = {
        {StrokeLineJoin::ARCS, "arcs"}, {StrokeLineJoin::BEVEL, "bevel"}, 
        {StrokeLineJoin::MITER, "miter"}, {StrokeLineJoin::MITER_CLIP, "miter-clip"}, {StrokeLineJoin::ROUND, "round"}
    };

    std::ostream& operator<<(std::ostream&, const svg::StrokeLineCap&);

    std::ostream& operator<<(std::ostream&, const svg::StrokeLineJoin&);

    struct Point {
        Point() = default;
        Point(double x, double y)
            : x(x)
            , y(y) {
        }
        double x = 0;
        double y = 0;
    };

    /*
     * ��������������� ���������, �������� �������� ��� ������ SVG-��������� � ���������.
     * ������ ������ �� ����� ������, ������� �������� � ��� ������� ��� ������ ��������
     */
    struct RenderContext {
        RenderContext(std::ostream& out)
            : out(out) {
        }

        RenderContext(std::ostream& out, int indent_step, int indent = 0)
            : out(out)
            , indent_step(indent_step)
            , indent(indent) {
        }

        RenderContext Indented() const {
            return { out, indent_step, indent + indent_step };
        }

        void RenderIndent() const {
            for (int i = 0; i < indent; ++i) {
                out.put(' ');
            }
        }

        std::ostream& out;
        int indent_step = 0;
        int indent = 0;
    };

    /*
     * ����������� ������� ����� Object ������ ��� ���������������� ��������
     * ���������� ����� SVG-���������
     * ��������� ������� "��������� �����" ��� ������ ����������� ����
     */

    class Object {
    public:
        void Render(const RenderContext& context) const;

        virtual ~Object() = default;

    private:
        virtual void RenderObject(const RenderContext& context) const = 0;
    };

    class ObjectContainer {
    protected:
        ObjectContainer() = default;
    public:

        template <typename Obj>
        void Add(Obj obj) {
            AddPtr(std::make_unique<Obj>(obj));
        }

        virtual void AddPtr(std::unique_ptr<Object>&& obj) = 0;
    private:
    };

    class Drawable {
    public:
        virtual ~Drawable() = default;
        virtual void Draw(ObjectContainer& container) const = 0;
    };

    template <typename Owner>
    class PathProps {
    public:
        Owner& SetFillColor(Color color) {
            _fill_color = std::move(color);
            return AsOwner();
        }

        Owner& SetStrokeColor(Color color) {
            _stroke_color = std::move(color);
            return AsOwner();
        }

        Owner& SetStrokeWidth(double width) {
            _width = std::move(width);
            return AsOwner();
        }

        Owner& SetStrokeLineCap(StrokeLineCap line_cap) {
            _line_cap = line_cap;
            return AsOwner();
        }

        Owner& SetStrokeLineJoin(StrokeLineJoin line_join) {
            _line_join = line_join;
            return AsOwner();
        }

    protected:
        ~PathProps() = default;

        void RenderAttrs(std::ostream& out) const {
            using namespace std::literals;

            if (_fill_color) {
                out << " fill=\""sv << *_fill_color << "\""sv;
            }

            if (_stroke_color) {
                out << " stroke=\""sv << *_stroke_color << "\""sv;
            }

            if (_width) {
                out << " stroke-width=\""sv << *_width << "\""sv;
            }

            if (_line_cap) {
                out << " stroke-linecap=\""sv << *_line_cap << "\""sv;
            }

            if (_line_join) {
                out << " stroke-linejoin=\""sv << *_line_join << "\""sv;
            }
        }

    private:
        Owner& AsOwner() {
            // static_cast ��������� ����������� *this � Owner&,
            // ���� ����� Owner � ��������� PathProps
            return static_cast<Owner&>(*this);
        }

        std::optional<Color> _fill_color;
        std::optional<Color> _stroke_color;
        std::optional<double> _width;
        std::optional<StrokeLineCap> _line_cap;
        std::optional<StrokeLineJoin> _line_join;
    };

    /*
     * ����� Circle ���������� ������� <circle> ��� ����������� �����
     * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/circle
     */
    class Circle final : public Object, public PathProps<Circle> {
    public:
        Circle& SetCenter(Point center);
        Circle& SetRadius(double radius);

    private:
        void RenderObject(const RenderContext& context) const override;

        Point center_;
        double radius_ = 1.0;
    };

    /*
     * ����� Polyline ���������� ������� <polyline> ��� ����������� ������� �����
     * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/polyline
     */
    class Polyline final : public Object, public PathProps<Polyline> {
    public:
        // ��������� ��������� ������� � ������� �����
        Polyline& AddPoint(Point point);

    private:
        void RenderObject(const RenderContext& context) const override;

        std::vector<Point> _tops_points = {};
    };

    /*
     * ����� Text ���������� ������� <text> ��� ����������� ������
     * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/text
     */
    class Text final : public Object, public PathProps<Text> {
    public:
        // ����� ���������� ������� ����� (�������� x � y)
        Text& SetPosition(Point pos);

        // ����� �������� ������������ ������� ����� (�������� dx, dy)
        Text& SetOffset(Point offset);

        // ����� ������� ������ (������� font-size)
        Text& SetFontSize(uint32_t size);

        // ����� �������� ������ (������� font-family)
        Text& SetFontFamily(std::string font_family);

        // ����� ������� ������ (������� font-weight)
        Text& SetFontWeight(std::string font_weight);

        // ����� ��������� ���������� ������� (������������ ������ ���� text)
        Text& SetData(std::string data);

    private:
        std::string TextEncode(const std::string& text) const;

        void RenderObject(const RenderContext& context) const override;

        Point _position;
        Point _offset;
        uint32_t _font_size = 1;
        std::string _font_family = "";
        std::string _font_weight = "";
        std::string _text_data = "";

        const std::unordered_map<char, std::string> _symbols_encode = {
            {'&', std::string("&amp;")}, {'\"', std::string("&quot;")},
            {'\'', std::string("&apos;")}, {'<', std::string("&lt;")}, {'>', std::string("&gt;")}
        };
    };

    class Document final : public ObjectContainer {
    public:
        // ��������� � svg-�������� ������-��������� svg::Object
        void AddPtr(std::unique_ptr<Object>&& obj) override;

        // ������� � ostream svg-������������� ���������
        void Render(std::ostream& out) const;

        // ������ ������ � ������, ����������� ��� ���������� ������ Document
    private:
        std::vector<std::unique_ptr<Object>> _objects;
    };

}  // namespace svg