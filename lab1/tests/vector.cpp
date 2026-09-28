#include "vector.hpp"

#include <gtest/gtest.h>

#include <string>
#include <utility>

namespace {

Vector<std::string> makeVector(int count) {
    Vector<std::string> vector;
    for (int i = 0; i < count; ++i) {
        vector.pushBack(std::to_string(i));
    }
    return vector;
}

TEST(VectorTest, DefaultIsEmpty) {
    const Vector<int> vector;
    EXPECT_TRUE(vector.empty());
    EXPECT_EQ(vector.size(), 0U);
    EXPECT_EQ(vector.capacity(), 0U);
    EXPECT_EQ(vector.begin(), vector.end());
}

TEST(VectorTest, SizedConstructorAllocates) {
    const Vector<int> vector(5);
    EXPECT_EQ(vector.size(), 5U);
    EXPECT_EQ(vector.capacity(), 5U);
    EXPECT_NE(vector.data(), nullptr);
}

TEST(VectorTest, PushBackGrowsGeometrically) {
    Vector<std::string> vector = makeVector(5);
    EXPECT_EQ(vector.size(), 5U);
    EXPECT_EQ(vector.capacity(), 8U);
    for (int i = 0; i < 5; ++i) {
        EXPECT_EQ(vector[static_cast<std::size_t>(i)], std::to_string(i));
    }
    vector[0] = "changed";
    EXPECT_EQ(vector[0], "changed");
}

TEST(VectorTest, CopyIsDeep) {
    const Vector<std::string> original = makeVector(3);
    Vector<std::string> copy(original);
    copy[0] = "x";
    EXPECT_EQ(original[0], "0");
    EXPECT_EQ(copy.size(), 3U);

    Vector<std::string> assigned;
    assigned = original;
    EXPECT_EQ(assigned.size(), 3U);
    EXPECT_EQ(assigned[2], "2");

    const Vector<std::string>& alias = assigned;
    assigned = alias;
    EXPECT_EQ(assigned.size(), 3U);
}

TEST(VectorTest, MoveTransfersOwnership) {
    Vector<std::string> source = makeVector(4);
    Vector<std::string> moved(std::move(source));
    EXPECT_EQ(moved.size(), 4U);

    Vector<std::string> target = makeVector(1);
    target = std::move(moved);
    EXPECT_EQ(target.size(), 4U);
    EXPECT_EQ(target[3], "3");

    Vector<std::string>& alias = target;
    target = std::move(alias);
    EXPECT_EQ(target.size(), 4U);
}

TEST(VectorTest, SwapExchangesContents) {
    Vector<std::string> first = makeVector(1);
    Vector<std::string> second = makeVector(3);
    first.swap(second);
    EXPECT_EQ(first.size(), 3U);
    EXPECT_EQ(second.size(), 1U);
}

TEST(VectorTest, IteratesOverElements) {
    const Vector<std::string> vector = makeVector(3);
    std::string joined;
    for (const std::string& value : vector) {
        joined += value;
    }
    EXPECT_EQ(joined, "012");
}

}
