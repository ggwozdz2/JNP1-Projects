#ifndef IMAGES_JNP_H
#define IMAGES_JNP_H

#include "functional.h"
#include "color.h"
#include "coordinate.h"
#include <functional>
#include <utility>
#include <cmath>
#include <iomanip>

// type used in that task to represents numbers
// in [0,1]
using Fraction = double;

template <typename T>
using Base_image = std::function<T(const Point)>;

using Image = Base_image<Color>;

using Region = Base_image<bool>;

using Blend = Base_image<Fraction>;

namespace Detail
{
    // helper pair to hangle point rotation
    using point_angle = std::pair<Point, double>;

    inline Point make_cartesian(Point p)
    {
        return p.is_polar ? from_polar(p) : p;
    }

    inline Point make_polar(Point p)
    {
        return p.is_polar ? p : to_polar(p);
    }

    // formulas used to rotate a point by a given angle
    inline const Point rotated(point_angle PA)
    {
        Point p = make_cartesian(PA.first);
        double phi = PA.second;
        Coordinate px = std::cos(-phi) * p.first - std::sin(-phi) * p.second;
        Coordinate py = (std::sin(-phi) * p.first + std::cos(-phi) * p.second);
        Point p1 = Point(px, py, p.is_polar);
        return p1;
    }
}

template <typename T>
Base_image<T> constant(T t)
{
    return [=]([[maybe_unused]] Point p)
    {
        return t;
    };
}

template <typename T>
Base_image<T> rotate(Base_image<T> image, double phi)
{
    return [=](Point p)
    {
        Detail::point_angle PA = std::make_pair(p, phi);
        return compose(Detail::rotated, image)(PA);
    };
}

template <typename T>
Base_image<T> translate(Base_image<T> image, Vector v)
{
    return compose(
        Detail::make_cartesian, [=](Point p)
        { return Point(p.first - v.first, p.second - v.second, p.is_polar); },
        image);
}

template <typename T>
Base_image<T> scale(Base_image<T> image, double s)
{
    return compose(
        Detail::make_cartesian, [=](Point p)
        { return Point(p.first / s, p.second / s, p.is_polar); },
        image);
}

template <typename T>
Base_image<T> circle(Point q, double r, T inner, T outer)
{
    return [=](Point p)
    {
        Point p1 = Detail::make_cartesian(p);
        Point q1 = Detail::make_cartesian(q);
        return distance(p1, q1) <= r ? inner : outer;
    };
}

template <typename T>
Base_image<T> checker(double d, T this_way, T that_way)
{
    return compose(Detail::make_cartesian, [=](Point p)
                   {
        // getting coordinates to check in which 
        // square inside checkerboard we are
        Coordinate x = p.first;
        Coordinate y = p.second;
        int num_x = x >= 0 ? std::floor(x / d) + 1 : std::floor(x / d) - 1;
        int num_y = y >= 0 ? std::floor(y / d) + 1 : std::floor(y / d) - 1;
        return (num_x + num_y) % 2 == 0 ? this_way : that_way; });
}

template <typename T>
Base_image<T> polar_checker(double d, int n, T this_way, T that_way)
{
    return compose([=](Point p)
                   {
        // formulas to get proper color used from provided materials
        Point p1 = Detail::make_polar(p);
        double n_2 = n;
        n_2 /= 2;
        auto py = p1.second * n_2 * d / M_PI;
        return (Point(p1.first, py, false)); },
                   checker(d, this_way, that_way));
}

template <typename T>
Base_image<T> rings(Point q, double d, T this_way, T that_way)
{
    return [=](Point p)
    {
        // checking parity of the ring inside which the current
        // point is gives us its color
        Point p1 = Detail::make_cartesian(p);
        Point q1 = Detail::make_cartesian(q);
        int dist_p_q = distance(p1, q1) / d;
        return dist_p_q % 2 == 0 ? this_way : that_way;
    };
}

template <typename T>
Base_image<T> vertical_stripe(double d, T this_way, T that_way)
{
    return compose(Detail::make_cartesian, [=](Point p)
                   { return std::abs(p.first) <= d / 2 ? this_way : that_way; });
}

inline Image cond(Region region, Image this_way, Image that_way)
{
    return lift([=](bool is_inside, Color this_way, Color that_way)
                { return is_inside ? this_way : that_way; },
                region, this_way, that_way);
}

inline Image lerp(Blend blend, Image this_way, Image that_way)
{
    return lift([=](Fraction w, Color this_way, Color that_way)
                { return this_way.weighted_mean(that_way, w); },
                blend, this_way, that_way);
}

// darken and lighten are simply mixing current color with black/white

inline Image darken(Image image, Blend blend)
{
    return lerp(blend, image, constant(Colors::black));
}

inline Image lighten(Image image, Blend blend)
{
    return lerp(blend, image, constant(Colors::white));
}

#endif
