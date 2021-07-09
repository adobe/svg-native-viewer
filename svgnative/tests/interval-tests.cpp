#include "gtest/gtest.h"

#include <svgnative/SVGRenderer.h>

using namespace SVGNative;

TEST(interval_tests, interval_empty_test)
{
  Interval interval;
  EXPECT_EQ(interval.Empty(), true);
}

TEST(interval_tests, interval_single_number_test)
{
  Interval interval(3.5);
  EXPECT_EQ(interval.Empty(), false);
  EXPECT_EQ(interval.Min(), 3.5);
  EXPECT_EQ(interval.Max(), 3.5);
}

TEST(interval_tests, interval_two_numbers_test)
{
  Interval interval(3.5, 4.5);
  EXPECT_EQ(interval.Empty(), false);
  EXPECT_EQ(interval.Min(), 3.5);
  EXPECT_EQ(interval.Max(), 4.5);
}

TEST(intervals_test, interval_two_opp_order_test)
{
  Interval interval(5.5, 2.5);
  EXPECT_EQ(interval.Empty(), false);
  EXPECT_EQ(interval.Min(), 2.5);
  EXPECT_EQ(interval.Max(), 5.5);
}


