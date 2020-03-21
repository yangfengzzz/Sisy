//
//  jetConeShape.cpp
//  Sisy
//
//  Created by 杨丰 on 2020/3/21.
//  Copyright © 2020 杨丰. All rights reserved.
//

#include "jetConeShape.hpp"
#include "btConeShape.h"
#include "btAlignedObjectArray.h"
#include "btBulletDynamicsCommon.h"
#include "OgreSceneManager.h"

#include "ShapeData.h"

namespace jet{
JetConeShape::JetConeShape(btScalar radius, btScalar height){
    m_shape = new btConeShape(radius, height);
}
//-------------------------------------------------------------------------
Ogre::ManualObject* JetConeShape::debugDrawObject(const btVector3& color,
                                                  Ogre::SceneManager* scene){
    const btConeShape* coneShape = static_cast<const btConeShape*>(m_shape);
    btScalar radius = coneShape->getRadius();  //+coneShape->getMargin();
    btScalar height = coneShape->getHeight();  //+coneShape->getMargin();

    int upAxis = coneShape->getConeUpIndex();
    int stepDegrees = 30;
    btVector3 start(0, 0, 0);

    btVector3 offsetHeight(0, 0, 0);
    btScalar halfHeight = height * btScalar(0.5);
    offsetHeight[upAxis] = halfHeight;
    btVector3 offsetRadius(0, 0, 0);
    offsetRadius[(upAxis + 1) % 3] = radius;
    btVector3 offset2Radius(0, 0, 0);
    offset2Radius[(upAxis + 2) % 3] = radius;

    btVector3 capEnd(0.f, 0.f, 0.f);
    capEnd[upAxis] = -halfHeight;

    clearManualIdx();
    Ogre::ManualObject * manualObject = scene->createManualObject();
    manualObject->begin("BaseWhite", Ogre::OT_LINE_LIST);
    
    for (int i = 0; i < 360; i += stepDegrees)
    {
        capEnd[(upAxis + 1) % 3] = btSin(btScalar(i) * SIMD_RADS_PER_DEG) * radius;
        capEnd[(upAxis + 2) % 3] = btCos(btScalar(i) * SIMD_RADS_PER_DEG) * radius;
        drawLine(start + offsetHeight, start + capEnd, manualObject);
    }

    drawLine(start + offsetHeight, start + (-offsetHeight + offsetRadius), manualObject);
    drawLine(start + offsetHeight, start + (-offsetHeight - offsetRadius), manualObject);
    drawLine(start + offsetHeight, start + (-offsetHeight + offset2Radius), manualObject);
    drawLine(start + offsetHeight, start + (-offsetHeight - offset2Radius), manualObject);

    // Drawing the base of the cone
    btVector3 yaxis(0, 0, 0);
    yaxis[upAxis] = btScalar(1.0);
    btVector3 xaxis(0, 0, 0);
    xaxis[(upAxis + 1) % 3] = btScalar(1.0);
    drawArc(start - offsetHeight, yaxis, xaxis, radius, radius, 0, SIMD_2_PI, color, false,
            manualObject,
            10.0);
    
    manualObject->end();
    return manualObject;
}
//-------------------------------------------------------------------------
JetConeShapeX::JetConeShapeX(btScalar radius, btScalar height){
    m_shape = new btConeShapeX(radius, height);
}
JetConeShapeZ::JetConeShapeZ(btScalar radius, btScalar height){
    m_shape = new btConeShapeZ(radius, height);
}
}
