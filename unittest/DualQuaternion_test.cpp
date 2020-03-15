//
//  DualQuaternion_test.cpp
//  GUnitTest
//
//  Created by 杨丰 on 2019/12/1.
//  Copyright © 2019 杨丰. All rights reserved.
//

#include "../src.gfx/OgreVector3.h"
#include "../src.gfx/OgreMatrix4.h"
#include "../src.gfx/OgreQuaternion.h"
#include "../src.gfx/OgreDualQuaternion.h"
#include "../external/gtest/include/gtest/gtest.h"

using namespace Ogre;

TEST(DualQuaternionTests, testConversion){
    DualQuaternion dQuat;
    Quaternion quat(Radian(Degree(60)), Vector3::UNIT_Y);
    Vector3 translation(0, 0, 10);
    dQuat.fromRotationTranslation(quat, translation);
        
    Quaternion result;
    Vector3 resTrans;
    dQuat.toRotationTranslation(result, resTrans);

    EXPECT_EQ(result, quat);
    ASSERT_EQ(resTrans.positionEquals(translation), true);
}

TEST(DualQuaternionTests, testDefaultValue){
    DualQuaternion dQuatDefault;
    
    Quaternion quatDefault;
    Vector3 transDefault;
    
    dQuatDefault.toRotationTranslation(quatDefault, transDefault);

    EXPECT_EQ(quatDefault, Quaternion::IDENTITY);
    ASSERT_EQ(transDefault.positionEquals(Vector3::ZERO), true);
}

TEST(DualQuaternionTests, testMatrix){
    Matrix4 transform;
    Vector3 translation(10, 4, 0);
    Vector3 scale = Vector3::UNIT_SCALE;
    Quaternion rotation;
    rotation.FromAngleAxis(Radian(Math::PI), Vector3::UNIT_Z);
    transform.makeTransform(translation, scale, rotation);

    DualQuaternion dQuat;
    dQuat.fromTransformationMatrix(transform);
    Matrix4 transformResult;
    dQuat.toTransformationMatrix(transformResult);

    Vector3 translationResult;
    Vector3 scaleResult;
    Quaternion rotationResult;
    transformResult.decomposition(translationResult, scaleResult, rotationResult);

    ASSERT_EQ(translationResult.positionEquals(translation), true);
    ASSERT_EQ(scaleResult.positionEquals(scale), true);
    ASSERT_EQ(rotationResult.equals(rotation, Radian(0.001)), true);
}
