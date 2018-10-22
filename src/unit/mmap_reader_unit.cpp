#include <iostream>

#include "gtest/gtest.h"
#include "mmap_reader.h"

TEST(MmapReader, Init) {

    std::string file = "./unit/data/mmap_reader_unit.data";
    MmapReader mreader(file.c_str());

    size_t size = mreader.Size();
    size_t readsize = mreader.ReadSize();
    ASSERT_TRUE(0 == readsize);
}

TEST(MmapReader, NextLine) {
    std::string file = "./unit/data/mmap_reader_unit.data";
    MmapReader mreader(file.c_str());

    std::string line;
    size_t readlen = 0;
    size_t readalllen = 0;

    while((readlen = mreader.NextLine(&line)) && readlen != 0) {
        readalllen += readlen;

        size_t readsize = mreader.ReadSize();

        ASSERT_TRUE(readalllen == readsize);

    }
}

TEST(MmapReader, Reset) {

    std::string file = "./unit/data/mmap_reader_unit.data";
    MmapReader mreader(file.c_str());

    std::string line1;
    size_t l = mreader.NextLine(&line1);
    ASSERT_TRUE(mreader.ReadSize() == line1.size());

    mreader.Reset();
    ASSERT_TRUE(mreader.ReadSize() == 0);
    std::string line2;
    l = mreader.NextLine(&line2);

    ASSERT_EQ(line1, line2);

}

int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

