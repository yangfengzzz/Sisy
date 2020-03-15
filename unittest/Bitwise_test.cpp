//
//  main.cpp
//  GUnitTest
//
//  Created by 杨丰 on 2019/12/1.
//  Copyright © 2019 杨丰. All rights reserved.
//

#include "../src.gfx/OgreBitwise.h"
#include "../external/gtest/include/gtest/gtest.h"

using namespace Ogre;
TEST(BitwiseTests, testFixedPointConversion){
    EXPECT_EQ(Bitwise::fixedToFixed(0x0,  1,8), (unsigned int)0x00);
    EXPECT_EQ(Bitwise::fixedToFixed(0x1,  1,8), (unsigned int)0xFF);
    EXPECT_EQ(Bitwise::fixedToFixed(0x2,  2,8), (unsigned int)0xAA);     // 10101010
    EXPECT_EQ(Bitwise::fixedToFixed(0x1,  2,8), (unsigned int)0x55);     // 01010101
    EXPECT_EQ(Bitwise::fixedToFixed(0x2,  2,9), (unsigned int)0x155);    // 1 01010101
    EXPECT_EQ(Bitwise::fixedToFixed(0x1,  2,9), (unsigned int)0x0AA);    // 0 10101010
    EXPECT_EQ(Bitwise::fixedToFixed(0xFE, 8,3), (unsigned int)0x7);      // 111
    EXPECT_EQ(Bitwise::fixedToFixed(0xFE, 8,9), (unsigned int)0x1FD);    // 111111101
    
    EXPECT_EQ(Bitwise::fixedToFloat(0xFF, 8), 1.0f);
    EXPECT_EQ(Bitwise::fixedToFloat(0x00, 8), 0.0f);
    
    EXPECT_EQ(Bitwise::floatToFixed(1.0f, 8), (unsigned int)0xFF);
    EXPECT_EQ(Bitwise::floatToFixed(0.0f, 8), (unsigned int)0x00);
    
    // Test clamping
    EXPECT_EQ(Bitwise::floatToFixed(-1.0f,8), (unsigned int)0x00);
    EXPECT_EQ(Bitwise::floatToFixed(2.0f, 8), (unsigned int)0xFF);
    
    // Test circular conversion
    bool failed = false;
    for(unsigned int x = 0; x < 0x0010; x++)
        if(Bitwise::floatToFixed(Bitwise::fixedToFloat(x, 4), 4) != x)
            failed = true;
    ASSERT_EQ(failed, false) << "circular floatToFixed/fixedToFloat for 4 bit failed";
    
    failed = false;
    for(unsigned int x = 0; x < 0x0100; x++)
        if(Bitwise::floatToFixed(Bitwise::fixedToFloat(x, 8), 8) != x)
            failed = true;
    ASSERT_EQ(failed, false) << "circular floatToFixed/fixedToFloat for 8 bit failed";
    
    failed = false;
    for(unsigned int x = 0; x < 0xFFE; x++) // originally loop ran till 0x1000, but precision issues sometimes prevent that
        if(Bitwise::floatToFixed(Bitwise::fixedToFloat(x, 12), 12) != x)
            failed = true;
    ASSERT_EQ(failed, false) << "circular floatToFixed/fixedToFloat for 12 bit failed";
}

TEST(BitwiseTests, testIntReadWrite){
    // Test reading and writing integers
    uint32 testje = 0x12345678;
    EXPECT_EQ(Bitwise::intRead(&testje, 4), 0x12345678);
    uint16 testje2 = 0x1234;
    EXPECT_EQ(Bitwise::intRead(&testje2, 2), 0x1234);
    uint8 testje3 = 0xD3;
    EXPECT_EQ(Bitwise::intRead(&testje3, 1), 0xD3);
    
#if OGRE_ENDIAN == OGRE_ENDIAN_BIG
    uint8 testje4[] = {0x12, 0x34, 0x56};
#else
    uint8 testje4[] = {0x56, 0x34, 0x12};
#endif
    EXPECT_EQ(Bitwise::intRead(&testje4, 3), 0x123456);
    
    Bitwise::intWrite(&testje, 4, 0x87654321);
    EXPECT_EQ(testje, 0x87654321);
    
    Bitwise::intWrite(&testje2, 2, 0x4321);
    EXPECT_EQ(testje2, 0x4321);
    
    Bitwise::intWrite(&testje3, 1, 0x12);
    EXPECT_EQ(testje3, 0x12);
}

TEST(BitwiseTests, testHalf){
    for(int x=0; x<0x100; x++)
    {
        float f = (float)x/256.0f;
//        uint32 fi = *reinterpret_cast<uint32*>(&f);
        uint16 g = Bitwise::floatToHalf(f);
        float h = Bitwise::halfToFloat(g);
//        uint32 hi = *reinterpret_cast<uint32*>(&h);
        int i = h*256.0f;
        
//        std::cerr << x << " " << fi << " "
//        << std::hex << std::setw(4) << g << " " << hi << " " << i << std::endl;
        EXPECT_EQ(x, i);
    }
}
