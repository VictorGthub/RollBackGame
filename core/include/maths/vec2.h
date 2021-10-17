#pragma once

#include <SFML/System/Vector2.hpp>
#include <maths/angle.h>

namespace core
{

struct Vec2f
{
    float x = 0.0f, y = 0.0f;

    constexpr Vec2f() = default;
    constexpr Vec2f(float x, float y) : x(x), y(y)
    {

    }
    Vec2f(sf::Vector2f v);


    [[nodiscard]] float GetMagnitude() const;
    void Normalize();
    [[nodiscard]] Vec2f GetNormalized() const;
    [[nodiscard]] float GetSqrMagnitude() const;
    [[nodiscard]] Vec2f Rotate(degree_t rotation) const;
    static float Dot(Vec2f a, Vec2f b);
    static Vec2f Lerp(Vec2f a, Vec2f b, float t);
    [[nodiscard]] sf::Vector2f toSf() const;

    Vec2f operator+(Vec2f v) const;
    Vec2f& operator+=(Vec2f v);
    Vec2f operator-(Vec2f v) const;
    Vec2f& operator-=(Vec2f v);
    Vec2f operator*(float f) const;
    Vec2f operator/(float f) const;

    static constexpr Vec2f zero() { return Vec2f(); }
    static constexpr Vec2f one() { return Vec2f(1,1); }
    static constexpr Vec2f up() { return Vec2f(0,1); }
    static constexpr Vec2f down() { return Vec2f(0,-1); }
    static constexpr Vec2f left() { return Vec2f(-1,0); }
    static constexpr Vec2f right() { return Vec2f(1,0); }
};

Vec2f operator*(float f, Vec2f v);

}