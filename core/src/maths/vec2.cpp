#include <maths/vec2.h>
#include <cmath>

namespace core
{
    Vec2f::Vec2f(sf::Vector2f v) : x(v.x), y(v.y)
    {
    }

    sf::Vector2f Vec2f::toSf() const
    {
        return sf::Vector2f(x, y);
    }

    Vec2f Vec2f::operator+(Vec2f v) const
    {
        return {x + v.x, y + v.y};
    }

    Vec2f& Vec2f::operator+=(Vec2f v)
    {
        x += v.x;
        y += v.y;
        return *this;
    }

    Vec2f Vec2f::operator-(Vec2f v) const
    {
        return {x - v.x, y - v.y};
    }

    Vec2f& Vec2f::operator-=(Vec2f v)
    {
        x -= v.x;
        y -= v.y;
        return *this;
    }

    Vec2f Vec2f::operator*(float f) const
    {
        return {x * f, y * f};
    }

    Vec2f Vec2f::operator/(float f) const
    {
        return {x / f, y / f};
    }

    Vec2f operator*(float f, Vec2f v)
    {
        return v*f;
    }

    float Vec2f::GetMagnitude() const
{
    return std::sqrt(GetSqrMagnitude());
}

void Vec2f::Normalize()
{
    const auto magnitude = GetMagnitude();
    x /= magnitude;
    y /= magnitude;
}

Vec2f Vec2f::GetNormalized() const
{
    const auto magnitude = GetMagnitude();
    return (*this) / magnitude;
}

float Vec2f::GetSqrMagnitude() const
{
    return x * x + y * y;
}

Vec2f Vec2f::Rotate(degree_t rotation) const
{

    const auto cs = Cos(rotation);
    const auto sn = Sin(rotation);

    Vec2f v;
    v.x = x * cs - y * sn;
    v.y = x * sn + y * cs;
    return v;
}

float Vec2f::Dot(Vec2f a, Vec2f b)
{
    return a.x * b.x + a.y * b.y;
}

Vec2f Vec2f::Lerp(Vec2f a, Vec2f b, float t)
{
    return a + (b - a) * t;
}
}
