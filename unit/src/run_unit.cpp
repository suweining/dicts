#include "gtest/gtest.h"
#include "run_unit_list.hpp"

TEST(Test, End) {
    ASSERT_TRUE(true);
}

int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

