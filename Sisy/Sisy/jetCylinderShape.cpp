//
//  jetCylinderShape.cpp
//  Sisy
//
//  Created by 杨丰 on 2020/3/21.
//  Copyright © 2020 杨丰. All rights reserved.
//

#include "jetCylinderShape.hpp"
#include "btCylinderShape.h"
#include "btAlignedObjectArray.h"
#include "btBulletDynamicsCommon.h"
#include "OgreSceneManager.h"

#include "ShapeData.h"

namespace jet{
JetCylinderShape::JetCylinderShape(const btVector3& halfExtents){
    m_shape = new btCylinderShape(halfExtents);
}
//-------------------------------------------------------------------------
Ogre::ManualObject* JetCylinderShape::debugDrawObject(const btVector3& color,
                                                      Ogre::SceneManager* scene){
    const btCylinderShape* cylinder = static_cast<const btCylinderShape*>(m_shape);
    int upAxis = cylinder->getUpAxis();
    btScalar radius = cylinder->getRadius();
    btScalar halfHeight = cylinder->getHalfExtentsWithMargin()[upAxis];
    btVector3 start(0, 0, 0);
    btVector3 offsetHeight(0, 0, 0);
    offsetHeight[upAxis] = halfHeight;
    int stepDegrees = 30;
    btVector3 capStart(0.f, 0.f, 0.f);
    capStart[upAxis] = -halfHeight;
    btVector3 capEnd(0.f, 0.f, 0.f);
    capEnd[upAxis] = halfHeight;

    clearManualIdx();
    Ogre::ManualObject * manualObject = scene->createManualObject();
    manualObject->begin("BaseWhite", Ogre::OT_LINE_LIST);
    
    for (int i = 0; i < 360; i += stepDegrees)
    {
        capEnd[(upAxis + 1) % 3] = capStart[(upAxis + 1) % 3] = btSin(btScalar(i) * SIMD_RADS_PER_DEG) * radius;
        capEnd[(upAxis + 2) % 3] = capStart[(upAxis + 2) % 3] = btCos(btScalar(i) * SIMD_RADS_PER_DEG) * radius;
        drawLine(start + capStart, start + capEnd, manualObject);
    }
    // Drawing top and bottom caps of the cylinder
    btVector3 yaxis(0, 0, 0);
    yaxis[upAxis] = btScalar(1.0);
    btVector3 xaxis(0, 0, 0);
    xaxis[(upAxis + 1) % 3] = btScalar(1.0);
    drawArc(start - offsetHeight, yaxis, xaxis,
            radius, radius, 0, SIMD_2_PI, color, false,
            manualObject,
            btScalar(10.0));
    drawArc(start + offsetHeight, yaxis, xaxis,
            radius, radius, 0, SIMD_2_PI, color, false,
            manualObject,
            btScalar(10.0));
    
    manualObject->end();
    return manualObject;
}
//-------------------------------------------------------------------------
JetCylinderShapeX::JetCylinderShapeX(const btVector3& halfExtents){
    m_shape = new btCylinderShapeX(halfExtents);
}
JetCylinderShapeZ::JetCylinderShapeZ(const btVector3& halfExtents){
    m_shape = new btCylinderShapeZ(halfExtents);
}
}
