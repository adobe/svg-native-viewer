#include "gtest/gtest.h"

#include <svgnative/SVGRenderer.h>

using namespace SVGNative;

TEST(interval_tests, interval_empty_test)
{
  Interval interval;
  EXPECT_EQ(interval.isEmpty(), true);
}

TEST(interval_tests, interval_single_number_test)
{
  Interval interval(3.5);
  EXPECT_EQ(interval.isEmpty(), true);
  EXPECT_EQ(interval.Min(), 3.5);
  EXPECT_EQ(interval.Max(), 3.5);
}

TEST(interval_tests, interval_two_numbers_test)
{
  Interval interval(3.5, 4.5);
  EXPECT_EQ(interval.isEmpty(), false);
  EXPECT_EQ(interval.Min(), 3.5);
  EXPECT_EQ(interval.Max(), 4.5);
}

TEST(intervals_test, interval_two_opp_order_test)
{
  Interval interval(5.5, 2.5);
  EXPECT_EQ(interval.isEmpty(), false);
  EXPECT_EQ(interval.Min(), 2.5);
  EXPECT_EQ(interval.Max(), 5.5);
}

TEST(intervals_test, interval_intersection)
{
  {
    Interval a(1.5, 2.5);
    Interval b(2, 3.5);

    Interval intersection = a & b;
    EXPECT_EQ(intersection.isEmpty(), false);
    EXPECT_EQ(intersection.Min(), 2);
    EXPECT_EQ(intersection.Max(), 2.5);
  }
  {
    Interval a(1.5, 2.5);
    Interval b(3, 3.5);

    Interval intersection = a & b;
    EXPECT_EQ(intersection.isEmpty(), true);
  }
  {
    Interval a(1.5, 2.5);
    Interval b(2.5, 3.5);

    Interval intersection = a & b;
    EXPECT_EQ(intersection.isEmpty(), true);
  }
  {
    Interval a(1.5, 2.5);
    Interval b;

    Interval intersection = a & b;
    EXPECT_EQ(intersection.isEmpty(), true);
  }
  {
    Interval a;
    Interval b(1, 2);

    Interval intersection = a & b;
    EXPECT_EQ(intersection.isEmpty(), true);
  }
  {
    Interval a;
    Interval b;

    Interval intersection = a & b;
    EXPECT_EQ(intersection.isEmpty(), true);
  }
  {
    Interval a(1, 1);
    Interval b(1, 1);

    Interval intersection = a & b;
    EXPECT_EQ(intersection.isEmpty(), true);
  }
}

TEST(intervals_test, interval_join)
{
  {
    Interval a(1.5, 2.5);
    Interval b(2, 3);

    Interval join = a | b;
    EXPECT_EQ(join.isEmpty(), false);
    EXPECT_EQ(join.Min(), 1.5);
    EXPECT_EQ(join.Max(), 3);
  }
  {
    Interval a(5.5, 2.5);
    Interval b(2, 3);

    Interval join = a | b;
    EXPECT_EQ(join.isEmpty(), false);
    EXPECT_EQ(join.Min(), 2);
    EXPECT_EQ(join.Max(), 5.5);
  }
  {
    Interval a(5.5, 5.5);
    Interval b(2, 3);

    Interval join = a | b;
    EXPECT_EQ(join.isEmpty(), false);
    EXPECT_EQ(join.Min(), 2);
    EXPECT_EQ(join.Max(), 3);
  }
  {
    Interval a(2, 3);
    Interval b(5.5, 5.5);

    Interval join = a | b;
    EXPECT_EQ(join.isEmpty(), false);
    EXPECT_EQ(join.Min(), 2);
    EXPECT_EQ(join.Max(), 3);
  }
  {
    Interval a(2, 2);
    Interval b(5.5, 5.5);

    Interval join = a | b;
    EXPECT_EQ(join.isEmpty(), true);
  }
  {
    Interval a;
    Interval b;

    Interval join = a | b;
    EXPECT_EQ(join.isEmpty(), true);
  }
}

TEST(intervals_test, interval_contain_test)
{
  {
    Interval a(1, 2);
    Interval b(1.5, 2.5);

    EXPECT_EQ(a.contains(b), false);
  }
  {
    Interval a(1, 2);
    Interval b(0.5, 2.5);

    EXPECT_EQ(a.contains(b), false);
    EXPECT_EQ(b.contains(a), true);
  }
  {
    Interval a(0.5, 2.5);
    Interval b(0.5, 2.5);

    EXPECT_EQ(a.contains(b), true);
    EXPECT_EQ(b.contains(a), true);
  }
  {
    Interval a(0.5, 0.5);
    Interval b(0.5, 2.5);

    EXPECT_EQ(a.contains(b), false);
    EXPECT_EQ(b.contains(a), true);
  }
  {
    Interval a(0, 0);
    Interval b(0.5, 2.5);

    EXPECT_EQ(a.contains(b), false);
    EXPECT_EQ(b.contains(a), true);
  }
  {
    Interval a(1, 1);
    Interval b(0.5, 2.5);

    EXPECT_EQ(a.contains(b), false);
    EXPECT_EQ(b.contains(a), true);
  }
  {
    Interval a(1, 1);
    Interval b(0.5, 0.5);

    EXPECT_EQ(a.contains(b), true);
    EXPECT_EQ(b.contains(a), true);
  }
}
