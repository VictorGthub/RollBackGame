#pragma once

#include <cmath>
#include <random>

namespace core
{
    inline float Abs(float v)
{
    return std::abs(v);
}
inline bool Equal(float a, float b, float epsilon = 0.0000001f)
{
    return Abs(a - b) < epsilon;
}

inline float Lerp(float start, float end, float t)
{
    return start + (end - start) * t;
}
template<typename T>
float Clamp(T value, T lower, T upper)
{
    return value < lower ? lower : (value > upper ? upper : value);
}

template<typename T>
typename std::enable_if<std::is_integral<T>::value, T>::type RandomRange(T start, T end)
{
    static std::random_device rd;  //Will be used to obtain a seed for the random number engine
    static std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<T> dis(start, end);
    return dis(gen);
}

template<typename T>
typename std::enable_if<std::is_floating_point<T>::value, T>::type RandomRange(T start, T end)
{
    static std::random_device rd;  //Will be used to obtain a seed for the random number engine
    static std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_real_distribution<T> dis(start, end);
    return dis(gen);
}
template<typename T>
T constexpr SqrtNewtonRaphson(T x, T curr, T prev)
{
    return curr == prev
           ? curr
           : SqrtNewtonRaphson<T>(x, (curr + x / curr) * 0.5, curr);
}


/*
* Constexpr version of the square root
* Return value:
*   - For a finite and non-negative value of "x", returns an approximation for the square root of "x"
*   - Otherwise, returns NaN
*/
template<typename T>
T constexpr Sqrt(T x)
{
    return x >= 0 && x < std::numeric_limits<T>::infinity()
           ? SqrtNewtonRaphson<T>(x, x, 0)
           : std::numeric_limits<T>::quiet_NaN();
}
}