#include "animation/animation_strategy.hh"
#include "color.hh"

#include <algorithm> // std::max, std::min
#include <cmath> // std::abs, std::fmod, std::sqrt
#include <stdexcept> // std::logic_error, std::invalid_argument

namespace Sian {

Color Color::rgb(double r, double g, double b)
{
    return Color(RGBColor(r, g, b));
}

Color Color::rgb(int r, int g, int b)
{
    return rgb((double) r, (double) g, (double) b);
}

Color Color::rgba(double r, double g, double b, double a)
{
    return Color(RGBColor(r, g, b), a);
}

Color Color::rgba(int r, int g, int b, int a)
{
    return rgba((double) r, (double) g, (double) b, (double) a);
}

Color Color::rgb(int code)
{
    return rgb(
            (code & 0xFF0000) >> 16,
            (code & 0x00FF00) >> 8,
            code & 0x0000FF);
}

Color Color::hsl(double h, double s, double l)
{
    return Color(HSLColor(h, s, l));
}

Color Color::hsla(double h, double s, double l, double a)
{
    return Color(HSLColor(h, s, l), a);
}

double Color::red() const
{
    return rgb_color.red;
}

double Color::green() const
{
    return rgb_color.green;
}

double Color::blue() const
{
    return rgb_color.blue;
}

double Color::hue() const
{
    return hsl_color.hue;
}

double Color::saturation() const
{
    return hsl_color.saturation;
}

double Color::lightness() const
{
    return hsl_color.lightness;
}

double Color::alpha() const
{
    return alpha_channel;
}

std::ostream& operator<<(std::ostream& stream, const Color& color)
{
    return stream << "[red=" << color.red()
                  << ", green=" << color.green()
                  << ", blue=" << color.blue()
                  << ", alpha=" << color.alpha()
                  << "]";
}

Color::Color(const RGBColor& rgb, double alpha_channel)
    : rgb_color(rgb),
      hsl_color(rgb_to_hsl(rgb)),
      alpha_channel(alpha_channel)
{ }

Color::Color(const HSLColor& hsl, double alpha_channel)
    : rgb_color(hsl_to_rgb(hsl)),
      hsl_color(hsl),
      alpha_channel(alpha_channel)
{ }

const Color Color::black = Color::rgb(0, 0, 0);
const Color Color::white = Color::rgb(255, 255, 255);

// Neither std::fmod nor std::remainder have the required behaviour when x < 0.
double simple_mod(double x, double y)
{
    int n = (int) (x / y);
    if (x < 0)
        --n;
    x -= n * 360;
    return x;
}

template<>
Color interpolate<Color>(const Color& origin, const Color& target, double t)
{
    const double H_o = origin.hue();
    const double H_t = target.hue();
    // Hue represents an angle in degrees and can therefore reach the target by
    // either increasing or decreasing. We choose whichever distance is smaller.
    const double H =
        simple_mod(H_t - H_o, 360) < simple_mod(H_o - H_t, 360)
        ? interpolate(H_o, H_t >= H_o ? H_t : H_t + 360, t)
        : interpolate(H_o, H_t <= H_o ? H_t : H_t - 360, t);

    return Color::hsla(
            simple_mod(H, 360),
            interpolate(origin.saturation(), target.saturation(), t),
            interpolate(origin.lightness(), target.lightness(), t),
            interpolate(origin.alpha(), target.alpha(), t));
}

template<>
double difference<Color>(const Color& a, const Color& b)
{
    const double dH = std::min(
            simple_mod(a.hue() - b.hue(), 360),
            simple_mod(b.hue() - a.hue(), 360));
    const double dS = std::abs(a.saturation() - b.saturation());
    const double dL = std::abs(a.lightness() - b.lightness());
    const double dH_norm = dH / 360;
    return std::sqrt(
            dH_norm * dH_norm +
            dS * dS +
            dL * dL);
}

HSLColor rgb_to_hsl(const RGBColor& rgb)
{
    const double R = rgb.red / 255;
    const double G = rgb.green / 255;
    const double B = rgb.blue / 255;

    // see https://en.wikipedia.org/wiki/HSL_and_HSV
    const double max = std::max({R, G, B});
    const double min = std::min({R, G, B});

    const double V = max;
    const double C = V - min;
    const double L = V - C / 2;

    double H =
        60 * (
            max == min ? 0 :
            max == R   ? 0 + (G - B) / C :
            max == G   ? 2 + (B - R) / C :
            max == B   ? 4 + (R - G) / C :
            throw std::logic_error("impossible"));
    H = simple_mod(H, 360);

    const double S =
        (max == 0 || min == 1)
        ? 0
        : (V - L) / std::min(L, 1 - L);
    return {H, S, L};
}

RGBColor hsl_to_rgb(const HSLColor& hsl)
{
    const double H = hsl.hue;
    const double S = hsl.saturation;
    const double L = hsl.lightness;

    // see https://en.wikipedia.org/wiki/HSL_and_HSV
    const auto f = [&](double n) {
        const double k = std::fmod(n + H / 30, 12);
        const double a = S * std::min(L, 1 - L);
        return L - a * std::max(std::min({k - 3, 9 - k, 1.0}), -1.0);
    };

    return {f(0) * 255, f(8) * 255, f(4) * 255};
}

double check_range(double val, double from, double to)
{
    if (val < from || val > to)
    {
        throw std::invalid_argument(
                "one of color components was outside the allowed range");
    }
    return val;
}

RGBColor::RGBColor(double red, double green, double blue)
    : red(check_range(red, 0, 255)),
      green(check_range(green, 0, 255)),
      blue(check_range(blue, 0, 255))
{ }

HSLColor::HSLColor(double hue, double saturation, double lightness)
    : hue(check_range(hue, 0, 360)),
      saturation(check_range(saturation, 0, 1)),
      lightness(check_range(lightness, 0, 1))
{ }


} // namespace Sian
