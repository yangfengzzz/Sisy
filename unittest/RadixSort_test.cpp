//
//  RadixSort_test.cpp
//  GUnitTest
//
//  Created by 杨丰 on 2019/12/1.
//  Copyright © 2019 杨丰. All rights reserved.
//

#include "../src.gfx/OgreMath.h"
#include "../src.gfx/OgreRadixSort.h"
#include "../external/gtest/include/gtest/gtest.h"

using namespace Ogre;

class RadixSortTests : public testing::Test {
public:
    void SetUp() override{
        srand(0);
    }
    
    class FloatSortFunctor
    {
    public:
        float operator()(const float& p) const
        {
            return p;
        }
    };
    class IntSortFunctor
    {
    public:
        int operator()(const int& p) const
        {
            return p;
        }
    };
    class UnsignedIntSortFunctor
    {
    public:
        unsigned int operator()(const unsigned int& p) const
        {
            return p;
        }
    };
};

TEST_F(RadixSortTests, testFloatVector){
    std::vector<float> container;
    FloatSortFunctor func;
    RadixSort<std::vector<float>, float, float> sorter;
    
    for (int i = 0; i < 1000; ++i)
    {
        container.push_back((float)Math::RangeRandom(-1e10, 1e10));
    }
    
    sorter.sort(container, func);
    
    std::vector<float>::iterator v = container.begin();
    float lastValue = *v++;
    for (;v != container.end(); ++v)
    {
        ASSERT_GE(*v, lastValue);
        lastValue = *v;
    }
}

TEST_F(RadixSortTests, testFloatList){
    std::list<float> container;
    FloatSortFunctor func;
    RadixSort<std::list<float>, float, float> sorter;

    for (int i = 0; i < 1000; ++i)
    {
        container.push_back((float)Math::RangeRandom(-1e10, 1e10));
    }

    sorter.sort(container, func);

    std::list<float>::iterator v = container.begin();
    float lastValue = *v++;
    for (;v != container.end(); ++v)
    {
        ASSERT_GE(*v, lastValue);
        lastValue = *v;
    }
}

TEST_F(RadixSortTests, testUnsignedIntList){
    std::list<unsigned int> container;
    UnsignedIntSortFunctor func;
    RadixSort<std::list<unsigned int>, unsigned int, unsigned int> sorter;

    for (int i = 0; i < 1000; ++i)
    {
        container.push_back((unsigned int)Math::RangeRandom(0, 1e10));
    }

    sorter.sort(container, func);

    std::list<unsigned int>::iterator v = container.begin();
    unsigned int lastValue = *v++;
    for (;v != container.end(); ++v)
    {
        ASSERT_GE(*v, lastValue);
        lastValue = *v;
    }
}

TEST_F(RadixSortTests, testIntList){
    std::list<int> container;
    IntSortFunctor func;
    RadixSort<std::list<int>, int, int> sorter;

    for (int i = 0; i < 1000; ++i)
    {
        container.push_back((int)Math::RangeRandom(-1e10, 1e10));
    }

    sorter.sort(container, func);

    std::list<int>::iterator v = container.begin();
    int lastValue = *v++;
    for (;v != container.end(); ++v)
    {
        ASSERT_GE(*v, lastValue);
        lastValue = *v;
    }
}

TEST_F(RadixSortTests, testUnsignedIntVector){
    std::vector<unsigned int> container;
    UnsignedIntSortFunctor func;
    RadixSort<std::vector<unsigned int>, unsigned int, unsigned int> sorter;

    for (int i = 0; i < 1000; ++i)
    {
        container.push_back((unsigned int)Math::RangeRandom(0, 1e10));
    }

    sorter.sort(container, func);

    std::vector<unsigned int>::iterator v = container.begin();
    unsigned int lastValue = *v++;
    for (;v != container.end(); ++v)
    {
        ASSERT_GE(*v, lastValue);
        lastValue = *v;
    }
}

TEST_F(RadixSortTests, testIntVector){
    std::vector<int> container;
    IntSortFunctor func;
    RadixSort<std::vector<int>, int, int> sorter;

    for (int i = 0; i < 1000; ++i)
    {
        container.push_back((int)Math::RangeRandom(-1e10, 1e10));
    }

    sorter.sort(container, func);

    std::vector<int>::iterator v = container.begin();
    int lastValue = *v++;
    for (;v != container.end(); ++v)
    {
        ASSERT_GE(*v, lastValue);
        lastValue = *v;
    }
}
