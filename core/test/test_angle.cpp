//
// Created by efarhan on 7/26/21.
//
#include <cmath>
#include "maths/angle.h"
#include <gtest/gtest.h>

TEST(Angle, RadianToDegree)
{
    core::radian_t angle{M_PI};
    core::degree_t angle2 = angle;

    EXPECT_FLOAT_EQ(180.0f, angle2.value());
}

TEST(Angle, DegreeToRadian)
{
    core::degree_t angle{180.0f};
    core::radian_t angle2 = angle;

    EXPECT_FLOAT_EQ(M_PI, angle2.value());
}

TEST(Angle, Cosinus)
{
    core::degree_t angle{180.0f};
    core::radian_t angle2 = angle;
    EXPECT_FLOAT_EQ(core::Cos(angle), core::Cos(angle2));
}

TEST(Angle, Sinus)
{
    core::degree_t angle{180.0f};
    core::radian_t angle2 = angle;
    EXPECT_FLOAT_EQ(core::Sin(angle), core::Sin(angle2));
}

TEST(Angle, Tan)
{
    core::degree_t angle{45.0f};
    core::radian_t angle2 = angle;
    EXPECT_FLOAT_EQ(core::Tan(angle), core::Tan(angle2));
}

TEST(Angle, ArcCosinus)
{
    core::degree_t angle{180.0f};
    const auto result = core::Cos(angle);
    const core::degree_t angleResult = core::Acos(result);
    EXPECT_FLOAT_EQ(angleResult.value(), angle.value());
}

TEST(Angle, ArcSinus)
{
    core::degree_t angle{90.0f};
    const auto result = core::Sin(angle);
    const core::degree_t angleResult = core::Asin(result);
    EXPECT_FLOAT_EQ(angleResult.value(), angle.value());
}

TEST(Angle, ArcTan)
{
    core::degree_t angle{45.0f};
    const auto result = core::Tan(angle);
    const core::degree_t angleResult = core::Atan(result);
    EXPECT_FLOAT_EQ(angleResult.value(), angle.value());
}