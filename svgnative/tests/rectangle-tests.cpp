/*
Copyright 2022 Adobe. All rights reserved.
This file is licensed to you under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License. You may obtain a copy
of the License at http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under
the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR REPRESENTATIONS
OF ANY KIND, either express or implied. See the License for the specific language
governing permissions and limitations under the License.
*/

#include "gtest/gtest.h"
#include "gtest/gtest-spi.h"

#include <svgnative/SVGRenderer.h>

using namespace SVGNative;

TEST(rectangle_tests, rectangle_empty_test)
{
    Rect rect(0, 0, 0, 0);
    EXPECT_EQ(rect.IsEmpty(), true);
}

TEST(rectangle_tests, rectangle_invalid_dimensions_test)
{
    try {
        Rect rect(1, 2, 4, -6);
        FAIL();
    } catch(std::invalid_argument &excepted) {}
}

TEST(rectangle_tests, rectangle_regular_test)
{
    Rect rect(1, 2, 4, 6);
    EXPECT_EQ(rect.IsEmpty(), false);
    EXPECT_EQ(rect.x, 1);
    EXPECT_EQ(rect.y, 2);
    EXPECT_EQ(rect.width, 4);
    EXPECT_EQ(rect.height, 6);
}

TEST(rectangle_tests, rectangle_equality_test)
{
    Rect rect1(1, 2, 4, 6);
    Rect rect2(1, 2, 4, 6);
    Rect rect3(2, 2, 4, 6);
    EXPECT_EQ(rect1 == rect2, true);
    EXPECT_EQ(rect2 == rect3, false);
    EXPECT_EQ(rect2 == rect1, true);
    EXPECT_EQ(rect1 == rect3, false);
}

TEST(rectangle_tests, rectangle_Contains_test)
{
    Rect rect1(1, 2, 4, 6);
    Rect rect2(2, 3, 2, 2);
    EXPECT_EQ(rect1.Contains(rect2), true);
    EXPECT_EQ(rect2.Contains(rect1), false);
}

TEST(rectangle_tests, rectangle_Contains_self_test)
{
    Rect rect1(1, 2, 4, 6);
    EXPECT_EQ(rect1.Contains(rect1), true);
}

TEST(rectangle_tests, rectangle_intersection_test)
{
    Rect rect1(1, 2, 4, 6);
    Rect rect2(2, 3, 2, 2);
    Rect rect3(0, 0, 10, 10);
    Rect rect4(-10, -10, 5, 5);
    Rect rect5(-8, -8, 5, 5);
    EXPECT_EQ((Rect{0, 0, 0, 0} & Rect{0, 0, 0, 0}).IsEmpty(), true);
    EXPECT_EQ((Rect(1, 1, 10, 10) & Rect{0, 0, 0, 0}).IsEmpty(), true);
    EXPECT_EQ((Rect{0, 0, 0, 0} & Rect(1, 1, 10, 10)).IsEmpty(), true);
    EXPECT_EQ((Rect(1, 1, 10, 10) & Rect(1, 1, 10, 10)).IsEmpty(), false);
    EXPECT_EQ((Rect(1, 1, 10, 10) & Rect(1, 1, 10, 10)) == Rect(1, 1, 10, 10), true);
    EXPECT_EQ((rect1 & rect2).IsEmpty(), false);
    EXPECT_EQ((rect1 & rect2) == rect2, true);
    EXPECT_EQ((rect1 & rect3).IsEmpty(), false);
    EXPECT_EQ((rect1 & rect3) == rect1, true);
    EXPECT_EQ((rect3 & rect4).IsEmpty(), true);
    Rect result = rect4 & rect5;
    EXPECT_EQ(result.IsEmpty(), false);
    EXPECT_EQ(result.x, -8);
    EXPECT_EQ(result.y, -8);
    EXPECT_EQ(result.width, 3);
    EXPECT_EQ(result.height, 3);
    EXPECT_EQ((result & result) == result, true);
}

TEST(rectangle_tests, rectangle_join_test)
{
    Rect rect1(1, 1, 4, 4);
    Rect rect2(5, 5, 4, 4);
    Rect result = rect1 | rect2;
    EXPECT_EQ(result.IsEmpty(), false);
    EXPECT_EQ(result.x, 1);
    EXPECT_EQ(result.y, 1);
    EXPECT_EQ(result.width, 8);
    EXPECT_EQ(result.height, 8);
    EXPECT_EQ((Rect{0, 0, 0, 0} | Rect{0, 0, 0, 0}).IsEmpty(), true);
    EXPECT_EQ((rect1 | rect1) == rect1, true);
    EXPECT_EQ((rect1 | Rect{0, 0, 0, 0}) == rect1, true);
    EXPECT_EQ((rect2 | Rect{0, 0, 0, 0}) == rect2, true);
}

TEST(rectangle_tests, rectangle_side_tests)
{
    Rect rect(1, 2, 4, 6);
    EXPECT_EQ(rect.Area(), 24.0);
    EXPECT_EQ(rect.Left(), 1);
    EXPECT_EQ(rect.Top(), 2);
    EXPECT_EQ(rect.Right(), 5);
    EXPECT_EQ(rect.Bottom(), 8);
}
