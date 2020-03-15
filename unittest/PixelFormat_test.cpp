//
//  PixelFormat_test.cpp
//  GUnitTest
//
//  Created by 杨丰 on 2019/12/1.
//  Copyright © 2019 杨丰. All rights reserved.
//

#include "../src.gfx/OgrePixelBox.h"
#include "../src.gfx/OgrePixelFormat.h"
#include "../src.gfx/OgrePixelUtil.h"
#include "../external/gtest/include/gtest/gtest.h"

using namespace Ogre;

class PixelFormatTests : public testing::Test {
public:
    void SetUp() override{
        mSize = 4096;
        mRandomData = new uint8[mSize];
        mTemp = new uint8[mSize];
        mTemp2 = new uint8[mSize];
        
        // Generate reproducible random data
        srand(0);
        for(unsigned int x=0; x<(unsigned int)mSize; x++)
            mRandomData[x] = (uint8)rand();
    }
    
    void TearDown() override{
        delete [] mRandomData;
        delete [] mTemp;
        delete [] mTemp2;
    }
    
    int mSize;
    uint8 *mRandomData;
    uint8 *mTemp, *mTemp2;
    PixelBox mSrc, mDst1, mDst2;
    
public:
    void setupBoxes(PixelFormat srcFormat, PixelFormat dstFormat)
    {
        unsigned int width = (mSize-4) / PixelUtil::getNumElemBytes(srcFormat);
        unsigned int width2 = (mSize-4) / PixelUtil::getNumElemBytes(dstFormat);
        if(width > width2)
            width = width2;
        
        mSrc = PixelBox(width, 1, 1, srcFormat, mRandomData);
        mDst1 = PixelBox(width, 1, 1, dstFormat, mTemp);
        mDst2 = PixelBox(width, 1, 1, dstFormat, mTemp2);
    }
    
    // Pure 32 bit float precision brute force pixel conversion; for comparison
    void naiveBulkPixelConversion(const PixelBox &src, const PixelBox &dst)
    {
        uint8 *srcptr = static_cast<uint8*>(src.data);
        uint8 *dstptr = static_cast<uint8*>(dst.data);
        size_t srcPixelSize = PixelUtil::getNumElemBytes(src.format);
        size_t dstPixelSize = PixelUtil::getNumElemBytes(dst.format);

        // Calculate pitches+skips in bytes
        unsigned long srcRowSkipBytes = src.getRowSkip()*srcPixelSize;
        unsigned long srcSliceSkipBytes = src.getSliceSkip()*srcPixelSize;

        unsigned long dstRowSkipBytes = dst.getRowSkip()*dstPixelSize;
        unsigned long dstSliceSkipBytes = dst.getSliceSkip()*dstPixelSize;

        // The brute force fallback
        float r,g,b,a;
        for(size_t z=src.front; z<src.back; z++)
        {
            for(size_t y=src.top; y<src.bottom; y++)
            {
                for(size_t x=src.left; x<src.right; x++)
                {
                    PixelUtil::unpackColour(&r, &g, &b, &a, src.format, srcptr);
                    PixelUtil::packColour(r, g, b, a, dst.format, dstptr);
                    srcptr += srcPixelSize;
                    dstptr += dstPixelSize;
                }
                srcptr += srcRowSkipBytes;
                dstptr += dstRowSkipBytes;
            }
            srcptr += srcSliceSkipBytes;
            dstptr += dstSliceSkipBytes;
        }
    }
    
public:
    void testCase(PixelFormat srcFormat, PixelFormat dstFormat)
    {
        setupBoxes(srcFormat, dstFormat);
        // Check end of buffer
        unsigned long eob = mDst1.getWidth()*PixelUtil::getNumElemBytes(dstFormat);
        mTemp[eob] = (unsigned char)0x56;
        mTemp[eob+1] = (unsigned char)0x23;

        //std::cerr << "["+PixelUtil::getFormatName(srcFormat)+"->"+PixelUtil::getFormatName(dstFormat)+"]" << " " << eob << std::endl;

        // Do pack/unpacking with both naive and optimized version
        PixelUtil::bulkPixelConversion(mSrc, mDst1);
        naiveBulkPixelConversion(mSrc, mDst2);

        EXPECT_EQ(mTemp[eob], (unsigned char)0x56);
        EXPECT_EQ(mTemp[eob+1], (unsigned char)0x23);

        std::stringstream s;
        int x;
        s << "src=";
        for(x=0; x<16; x++)
            s << std::hex << std::setw(2) << std::setfill('0') << (unsigned int) mRandomData[x];
        s << " dst=";
        for(x=0; x<16; x++)
            s << std::hex << std::setw(2) << std::setfill('0') << (unsigned int) mTemp[x];
        s << " dstRef=";
        for(x=0; x<16; x++)
            s << std::hex << std::setw(2) << std::setfill('0') << (unsigned int) mTemp2[x];
        s << " ";

        // Compare result
        StringStream msg;
        msg << "Conversion mismatch [" << PixelUtil::getFormatName(srcFormat) <<
            "->" << PixelUtil::getFormatName(dstFormat) << "] " << s.str();
        EXPECT_EQ(memcmp(mDst1.data, mDst2.data, eob), 0)<<msg.str();
    }
};

TEST_F(PixelFormatTests, testFloatPackUnpack){
    // Float32
    float data[4] = {1.0f, 2.0f, 3.0f, 4.0f};
    float r,g,b,a;
    PixelUtil::unpackColour(&r, &g, &b, &a, PF_FLOAT32_RGBA, data);
    EXPECT_EQ(r, 1.0f);
    EXPECT_EQ(g, 2.0f);
    EXPECT_EQ(b, 3.0f);
    EXPECT_EQ(a, 4.0f);
    
    // Float16
    setupBoxes(PF_A8B8G8R8, PF_FLOAT16_RGBA);
    mDst2.format = PF_A8B8G8R8;
    unsigned int eob = mSrc.getWidth()*4;
    
    PixelUtil::bulkPixelConversion(mSrc, mDst1);
    PixelUtil::bulkPixelConversion(mDst1, mDst2);
    
    // Locate errors
    std::stringstream s;
    unsigned int x;
    for(x=0; x<eob; x++) {
        if(mTemp2[x] != mRandomData[x])
            s << std::hex << std::setw(2) << std::setfill('0') << (unsigned int) mRandomData[x]
            << "!= " << std::hex << std::setw(2) << std::setfill('0') << (unsigned int) mTemp2[x] << " ";
    }
    
    // src and dst2 should match
    EXPECT_EQ(memcmp(mSrc.data, mDst2.data, eob), 0)
    <<"PF_FLOAT16_RGBA<->PF_A8B8G8R8 conversion was not lossless "+s.str();
}

TEST_F(PixelFormatTests, testBulkConversion){
    // Self match
    testCase(PF_A8R8G8B8, PF_A8R8G8B8);

    // Optimized
    testCase(PF_A8R8G8B8,PF_A8B8G8R8);
    testCase(PF_A8R8G8B8,PF_B8G8R8A8);
    testCase(PF_A8R8G8B8,PF_R8G8B8A8);
    testCase(PF_A8B8G8R8,PF_A8R8G8B8);
    testCase(PF_A8B8G8R8,PF_B8G8R8A8);
    testCase(PF_A8B8G8R8,PF_R8G8B8A8);
    testCase(PF_B8G8R8A8,PF_A8R8G8B8);
    testCase(PF_B8G8R8A8,PF_A8B8G8R8);
    testCase(PF_B8G8R8A8,PF_R8G8B8A8);
    testCase(PF_R8G8B8A8,PF_A8R8G8B8);
    testCase(PF_R8G8B8A8,PF_A8B8G8R8);
    testCase(PF_R8G8B8A8,PF_B8G8R8A8);

    testCase(PF_A8B8G8R8, PF_L8);
    testCase(PF_L8, PF_A8B8G8R8);
    testCase(PF_A8R8G8B8, PF_L8);
    testCase(PF_L8, PF_A8R8G8B8);
    testCase(PF_B8G8R8A8, PF_L8);
    testCase(PF_L8, PF_B8G8R8A8);
    testCase(PF_L8, PF_L16);
    testCase(PF_L16, PF_L8);
    testCase(PF_R8G8B8, PF_B8G8R8);
    testCase(PF_B8G8R8, PF_R8G8B8);
    testCase(PF_B8G8R8, PF_R8G8B8);
    testCase(PF_R8G8B8, PF_B8G8R8);
    testCase(PF_R8G8B8, PF_A8R8G8B8);
    testCase(PF_B8G8R8, PF_A8R8G8B8);
    testCase(PF_R8G8B8, PF_A8B8G8R8);
    testCase(PF_B8G8R8, PF_A8B8G8R8);
    testCase(PF_R8G8B8, PF_B8G8R8A8);
    testCase(PF_B8G8R8, PF_B8G8R8A8);
    testCase(PF_A8R8G8B8, PF_R8G8B8);
    testCase(PF_A8R8G8B8, PF_B8G8R8);
    testCase(PF_X8R8G8B8, PF_A8R8G8B8);
    testCase(PF_X8R8G8B8, PF_A8B8G8R8);
    testCase(PF_X8R8G8B8, PF_B8G8R8A8);
    testCase(PF_X8R8G8B8, PF_R8G8B8A8);
    testCase(PF_X8B8G8R8, PF_A8R8G8B8);
    testCase(PF_X8B8G8R8, PF_A8B8G8R8);
    testCase(PF_X8B8G8R8, PF_B8G8R8A8);
    testCase(PF_X8B8G8R8, PF_R8G8B8A8);
}
