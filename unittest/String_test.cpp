//
//  String_test.cpp
//  GUnitTest
//
//  Created by 杨丰 on 2019/12/1.
//  Copyright © 2019 杨丰. All rights reserved.
//

#include "../src.gfx/OgreStringConverter.h"
#include "../external/gtest/include/gtest/gtest.h"

using namespace Ogre;

class StringTests : public testing::Test {
public:
    void SetUp() override{
        testFileNoPath = "testfile.txt";
        testFileRelativePathUnix = "this/is/relative/testfile.txt";
        testFileRelativePathWindows = "this\\is\\relative\\testfile.txt";
        testFileAbsolutePathUnix = "/this/is/absolute/testfile.txt";
        testFileAbsolutePathWindows = "c:\\this\\is\\absolute\\testfile.txt";
    }
    
    String testFileNoPath;
    String testFileRelativePathWindows;
    String testFileRelativePathUnix;
    String testFileAbsolutePathWindows;
    String testFileAbsolutePathUnix;
    
};

TEST_F(StringTests, testSplitFileNameNoPath){
    String basename, path;
    StringUtil::splitFilename(testFileNoPath, basename, path);
    
    EXPECT_EQ(testFileNoPath, basename);
    ASSERT_EQ(path.empty(), true);
}

TEST_F(StringTests, testSplitFileNameRelativePath){
    String basename, path;
    
    // Unix
    StringUtil::splitFilename(testFileRelativePathUnix, basename, path);
    EXPECT_EQ(String("testfile.txt"), basename);
    EXPECT_EQ(String("this/is/relative/"), path);
    
    // Windows
    StringUtil::splitFilename(testFileRelativePathWindows, basename, path);
    EXPECT_EQ(String("testfile.txt"), basename);
    EXPECT_EQ(String("this/is/relative/"), path);
}

TEST_F(StringTests, testSplitFileNameAbsolutePath){
    String basename, path;
    
    // Unix
    StringUtil::splitFilename(testFileAbsolutePathUnix, basename, path);
    EXPECT_EQ(String("testfile.txt"), basename);
    EXPECT_EQ(String("/this/is/absolute/"), path);
    
    // Windows
    StringUtil::splitFilename(testFileAbsolutePathWindows, basename, path);
    EXPECT_EQ(String("testfile.txt"), basename);
    EXPECT_EQ(String("c:/this/is/absolute/"), path);
}

//-----------------------------------------------------------------------------
TEST_F(StringTests, testMatchCaseSensitive){
    // Test positive
    ASSERT_EQ(StringUtil::match(testFileNoPath, testFileNoPath, true), true);
    
    // Test negative
    String upperCase = testFileNoPath;
    StringUtil::toUpperCase(upperCase);
    ASSERT_EQ(!StringUtil::match(testFileNoPath, upperCase, true), true);
}

TEST_F(StringTests, testMatchCaseInSensitive){
    // Test positive
    ASSERT_EQ(StringUtil::match(testFileNoPath, testFileNoPath, false), true);
    
    // Test positive
    String upperCase = testFileNoPath;
    StringUtil::toUpperCase(upperCase);
    ASSERT_EQ(StringUtil::match(testFileNoPath, upperCase, false), true);
}

TEST_F(StringTests, testMatchGlobAll){
    ASSERT_EQ(StringUtil::match(testFileNoPath, "*", true), true);
}

TEST_F(StringTests, testMatchGlobStart){
    ASSERT_EQ(StringUtil::match(testFileNoPath, "*stfile.txt", true), true);
    ASSERT_EQ(!StringUtil::match(testFileNoPath, "*astfile.txt", true), true);
}

TEST_F(StringTests, testMatchGlobEnd){
    ASSERT_EQ(StringUtil::match(testFileNoPath, "testfile.*", true), true);
    ASSERT_EQ(!StringUtil::match(testFileNoPath, "testfile.d*", true), true);
}

TEST_F(StringTests, testMatchGlobStartAndEnd){
    ASSERT_EQ(StringUtil::match(testFileNoPath, "*stfile.*", true), true);
    ASSERT_EQ(!StringUtil::match(testFileNoPath, "*astfile.d*", true), true);
}

TEST_F(StringTests, testMatchGlobMiddle){
    ASSERT_EQ(StringUtil::match(testFileNoPath, "test*.txt", true), true);
    ASSERT_EQ(!StringUtil::match(testFileNoPath, "last*.txt*", true), true);
}

TEST_F(StringTests, testMatchSuperGlobtastic){
    ASSERT_EQ(StringUtil::match(testFileNoPath, "*e*tf*e.t*t", true), true);
}

//----------------------------------------------------------------------------
TEST_F(StringTests, testParseReal){
    Real r = 23.454;

    String s = StringConverter::toString(r);
    Real t = StringConverter::parseReal(s);

    EXPECT_EQ(r, t);
}

TEST_F(StringTests, testParseInt){
    int r = 223546;

    String s = StringConverter::toString(r);
    int t = StringConverter::parseInt(s);

    EXPECT_EQ(r, t);
}

TEST_F(StringTests, testParseLong){
    long r = -2147483647;

    String s = StringConverter::toString(r);
    long t = StringConverter::parseLong(s);

    EXPECT_EQ(r, t);
}

TEST_F(StringTests, testParseUnsignedLong){
    unsigned long r = 4294967295UL;

    String s = StringConverter::toString(r);
    unsigned long t = StringConverter::parseUnsignedLong(s);

    EXPECT_EQ(r, t);
}

TEST_F(StringTests, testParseVector3){
    Vector3 r(0.12, 3.22, -4.04);

    String s = StringConverter::toString(r);
    Vector3 t = StringConverter::parseVector3(s);

    EXPECT_EQ(r, t);
}

TEST_F(StringTests, testParseMatrix4){
    Matrix4 r(1.12, 0, 0, 34, 0, 0.87, 0, 20, 0, 0, 0.56, 10, 0, 0, 0, 1);

    String s = StringConverter::toString(r);
    Matrix4 t = StringConverter::parseMatrix4(s);

    EXPECT_EQ(r, t);
}
    
TEST_F(StringTests, testParseQuaternion){
    Quaternion r(1.12, 0.87, 0.67, 1);

    String s = StringConverter::toString(r);
    Quaternion t = StringConverter::parseQuaternion(s);

    EXPECT_EQ(r, t);
}
    
TEST_F(StringTests, testParseBool){
    bool r = true;
    String s = StringConverter::toString(r);
    bool t = StringConverter::parseBool(s);
    EXPECT_EQ(r, t);

    r = false;
    s = StringConverter::toString(r);
    t = StringConverter::parseBool(s);
    EXPECT_EQ(r, t);
}
    
TEST_F(StringTests, testParseColourValue){
    ColourValue r(0.34, 0.44, 0.77, 1.0);

    String s = StringConverter::toString(r);
    ColourValue t = StringConverter::parseColourValue(s);
    EXPECT_EQ(r, t);
}
