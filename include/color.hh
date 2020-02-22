#ifndef COLOR_HH
#define COLOR_HH

#include "payload_type.hh"

#include <ostream>

namespace Sian {

struct RGBColor
{
    RGBColor(double red, double green, double blue);

    const double red;
    const double green;
    const double blue;
};

struct HSLColor
{
    HSLColor(double hue, double saturation, double lightness);

    const double hue;
    const double saturation;
    const double lightness;
};

class Color
{
public:
    static Color rgb(double r, double g, double b);

    static Color rgb(int r, int g, int b);

    static Color rgba(double r, double g, double b, double a);

    static Color rgba(int r, int g, int b, int a);

    static Color hsl(double h, double s, double l);

    static Color hsla(double h, double s, double l, double a);

    static Color rgb(int code);

    friend std::ostream& operator<<(std::ostream& stream, const Color& color);

    double red() const;

    double green() const;

    double blue() const;

    double hue() const;

    double saturation() const;

    double lightness() const;

    double alpha() const;

    static const Color black;
    static const Color white;

private:
    Color(const RGBColor& rgb, double alpha_channel = 255);
    Color(const HSLColor& hsl, double alpha_channel = 255);

    const RGBColor rgb_color;
    const HSLColor hsl_color;
    const double alpha_channel;
};

HSLColor rgb_to_hsl(const RGBColor& rgb);

RGBColor hsl_to_rgb(const HSLColor& hsl);

template<>
Color interpolate<Color>(const Color& origin, const Color& target, double t);

template<>
double difference<Color>(const Color& a, const Color& b);

} // namespace Sian

#endif
