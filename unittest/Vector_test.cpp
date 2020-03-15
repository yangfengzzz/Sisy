//
//  Vector_test.cpp
//  GUnitTest
//
//  Created by 杨丰 on 2019/12/1.
//  Copyright © 2019 杨丰. All rights reserved.
//

#include "../src.gfx/OgreVector2.h"
#include "../src.gfx/OgreVector3.h"
#include "../src.gfx/OgreVector4.h"
#include "../external/gtest/include/gtest/gtest.h"

using namespace Ogre;

TEST(VectorTests, testVector2Scaler){
    EXPECT_EQ(Vector2(1, 1) + Vector2(2, 2), Vector2(3, 3));
    EXPECT_EQ(1 + Vector2(2), Vector2(3, 3));
    Vector2 v1;
    v1 = 1;
    EXPECT_EQ(v1, Vector2(1, 1));
    v1 = 0.0;
    EXPECT_EQ(v1, Vector2::ZERO);
    v1 += 3;
    EXPECT_EQ(v1, Vector2(3, 3));

    v1 = 3 - Vector2(2);
    EXPECT_EQ(v1, Vector2(1));
    v1 = Vector2(5) - 7;
    EXPECT_EQ(v1, Vector2(-2));
    v1 -= 4;
    EXPECT_EQ(v1, Vector2(-6));
}

TEST(VectorTests, testVector3Scaler){
    EXPECT_EQ(Vector3(1, 1, 1) + Vector3(2, 2, 2), Vector3(3, 3, 3));
    EXPECT_EQ(1 + Vector3(2), Vector3(3, 3, 3));
    Vector3 v1;
    v1 = 1;
    EXPECT_EQ(v1, Vector3(1));
    v1 = 0.0;
    EXPECT_EQ(v1, Vector3::ZERO);
    v1 += 3;
    EXPECT_EQ(v1, Vector3(3));

    v1 = 3 - Vector3(2);
    EXPECT_EQ(v1, Vector3(1));
    v1 = Vector3(5) - 7;
    EXPECT_EQ(v1, Vector3(-2));
    v1 -= 4;
    EXPECT_EQ(v1, Vector3(-6));
}

TEST(VectorTests, testVector4Scaler){
    EXPECT_EQ(Vector4(1, 1, 1, 1) + Vector4(2, 2, 2, 2), Vector4(3, 3, 3, 3));
    EXPECT_EQ(1 + Vector4(2, 2, 2, 2), Vector4(3, 3, 3, 3));
    Vector4 v1;
    v1 = 1;
    EXPECT_EQ(v1, Vector4(1, 1, 1, 1));
    v1 = 0.0;
    EXPECT_EQ(v1, Vector4(0,0,0,0));
    v1 += 3;
    EXPECT_EQ(v1, Vector4(3,3,3,3));

    v1 = 3 - Vector4(2,2,2,2);
    EXPECT_EQ(v1, Vector4(1,1,1,1));
    v1 = Vector4(5,5,5,5) - 7;
    EXPECT_EQ(v1, Vector4(-2,-2,-2,-2));
    v1 -= 4;
    EXPECT_EQ(v1, Vector4(-6,-6,-6,-6));
}
