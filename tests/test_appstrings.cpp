#include <gtest/gtest.h>
#include "../core/utils/AppStrings.h"

TEST(AppStringsTest, DefaultNameUsesKindAndId)
{
    EXPECT_EQ(AppStrings::makeDefaultObjectName("Box", 7), QString("Box_7"));
}

TEST(AppStringsTest, DefaultNameWorksForDifferentPrimitive)
{
    EXPECT_EQ(AppStrings::makeDefaultObjectName("Cylinder", 12), QString("Cylinder_12"));
}

TEST(AppStringsTest, DefaultNameSupportsZeroId)
{
    EXPECT_EQ(AppStrings::makeDefaultObjectName("Sphere", 0), QString("Sphere_0"));
}

TEST(AppStringsTest, DuplicateNameAddsSuffix)
{
    EXPECT_EQ(AppStrings::makeDuplicateName("Sphere_3"), QString("Sphere_3_copy"));
}

TEST(AppStringsTest, DuplicateNameWorksForImportedObject)
{
    EXPECT_EQ(AppStrings::makeDuplicateName("ImportedPart"), QString("ImportedPart_copy"));
}

TEST(AppStringsTest, VectorFormattingUsesTwoDigits)
{
    EXPECT_EQ(AppStrings::formatVector3(10.0, -5.5, 0.125), QString("X=10.00, Y=-5.50, Z=0.13"));
}

TEST(AppStringsTest, VectorFormattingHandlesZeros)
{
    EXPECT_EQ(AppStrings::formatVector3(0.0, 0.0, 0.0), QString("X=0.00, Y=0.00, Z=0.00"));
}

TEST(AppStringsTest, VectorFormattingHandlesNegativeValues)
{
    EXPECT_EQ(AppStrings::formatVector3(-12.345, -1.0, -99.999),
              QString("X=-12.35, Y=-1.00, Z=-100.00"));
}
