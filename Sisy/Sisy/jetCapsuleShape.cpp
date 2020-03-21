//
//  jetCapsuleShape.cpp
//  Sisy
//
//  Created by 杨丰 on 2020/3/21.
//  Copyright © 2020 杨丰. All rights reserved.
//

#include "jetCapsuleShape.hpp"
#include "btCapsuleShape.h"
#include "btAlignedObjectArray.h"
#include "btBulletDynamicsCommon.h"
#include "OgreSceneManager.h"

#include "ShapeData.h"

namespace jet{
JetCapsuleShape::JetCapsuleShape(btScalar radius, btScalar height){
    m_shape = new btCapsuleShape(radius, height);
}
//-------------------------------------------------------------------------
Ogre::ManualObject* JetCapsuleShape::debugDrawObject(const btVector3& color,
                                                     Ogre::SceneManager* scene){
    const btCapsuleShape* capsuleShape = static_cast<const btCapsuleShape*>(m_shape);

    btScalar radius = capsuleShape->getRadius();
    btScalar halfHeight = capsuleShape->getHalfHeight();

    int upAxis = capsuleShape->getUpAxis();
    int stepDegrees = 30;

    btVector3 capStart(0.f, 0.f, 0.f);
    capStart[upAxis] = -halfHeight;

    btVector3 capEnd(0.f, 0.f, 0.f);
    capEnd[upAxis] = halfHeight;

    clearManualIdx();
    Ogre::ManualObject * manualObject = scene->createManualObject();
    manualObject->begin("BaseWhite", Ogre::OT_LINE_LIST);
    // Draw the ends
    {
        btTransform childTransform = btTransform::getIdentity();
        childTransform.getOrigin() = capStart;
        {
            btVector3 center = childTransform.getOrigin();
            btVector3 up = childTransform.getBasis().getColumn((upAxis + 1) % 3);
            btVector3 axis = -childTransform.getBasis().getColumn(upAxis);
            btScalar minTh = -SIMD_HALF_PI;
            btScalar maxTh = SIMD_HALF_PI;
            btScalar minPs = -SIMD_HALF_PI;
            btScalar maxPs = SIMD_HALF_PI;

            drawSpherePatch(center, up, axis, radius,
                            minTh, maxTh, minPs, maxPs, color,
                            manualObject,
                            btScalar(stepDegrees), false);
        }
    }

    {
        btTransform childTransform = btTransform::getIdentity();
        childTransform.getOrigin() = capEnd;
        {
            btVector3 center = childTransform.getOrigin();
            btVector3 up = childTransform.getBasis().getColumn((upAxis + 1) % 3);
            btVector3 axis = childTransform.getBasis().getColumn(upAxis);
            btScalar minTh = -SIMD_HALF_PI;
            btScalar maxTh = SIMD_HALF_PI;
            btScalar minPs = -SIMD_HALF_PI;
            btScalar maxPs = SIMD_HALF_PI;
            drawSpherePatch(center, up, axis, radius,
                            minTh, maxTh, minPs, maxPs, color,
                            manualObject,
                            btScalar(stepDegrees), false);
        }
    }

    // Draw some additional lines
    for (int i = 0; i < 360; i += stepDegrees)
    {
        capEnd[(upAxis + 1) % 3] = capStart[(upAxis + 1) % 3] = btSin(btScalar(i) * SIMD_RADS_PER_DEG) * radius;
        capEnd[(upAxis + 2) % 3] = capStart[(upAxis + 2) % 3] = btCos(btScalar(i) * SIMD_RADS_PER_DEG) * radius;
        drawLine(capStart, capEnd, manualObject);
    }
    manualObject->end();
    
    return manualObject;
}
//-------------------------------------------------------------------------
std::pair<Ogre::MeshPtr, Ogre::VertexBufferPacked*>
JetCapsuleShape::createCollisionShapeGraphicsObject(btCollisionShape* collisionShape,
                                                    Ogre::String name){
    btAlignedObjectArray<Ogre::uint16> indices;
    std::pair<Ogre::MeshPtr, Ogre::VertexBufferPacked*> mesh;
    
    int strideInBytes = 6 * sizeof(float);
    if (collisionShape->getShapeType() == CAPSULE_SHAPE_PROXYTYPE)
    {
        btCapsuleShape* sphereShape = (btCapsuleShape*)collisionShape;  //Y up
        int up = sphereShape->getUpAxis();
        btScalar halfHeight = sphereShape->getHalfHeight();
        
        btScalar radius = sphereShape->getRadius();
        btScalar sphereSize = 2. * radius;
        btVector3 radiusScale(sphereSize, sphereSize, sphereSize);
        
        btAlignedObjectArray<float> transformedVertices;
        int numVertices = sizeof(textured_detailed_sphere_vertices) / strideInBytes;
        transformedVertices.resize(numVertices * 6);
        for (int i = 0; i < numVertices; i++)
        {
            btVector3 vert;
            vert.setValue(textured_detailed_sphere_vertices[i * 9 + 0],
                          textured_detailed_sphere_vertices[i * 9 + 1],
                          textured_detailed_sphere_vertices[i * 9 + 2]);
            
            btVector3 trVer = radiusScale * vert;
            if (trVer[up] > 0)
                trVer[up] += halfHeight;
            else
                trVer[up] -= halfHeight;
            
            transformedVertices[i * 6 + 0] = trVer[0];
            transformedVertices[i * 6 + 1] = trVer[1];
            transformedVertices[i * 6 + 2] = trVer[2];
            transformedVertices[i * 6 + 3] = textured_detailed_sphere_vertices[i * 9 + 4];
            transformedVertices[i * 6 + 4] = textured_detailed_sphere_vertices[i * 9 + 5];
            transformedVertices[i * 6 + 5] = textured_detailed_sphere_vertices[i * 9 + 6];
        }
        
        int numIndices = sizeof(textured_detailed_sphere_indices) / sizeof(Ogre::uint16);
        mesh = createDynamicMesh(&transformedVertices[0], numVertices,
                                 textured_detailed_sphere_indices, numIndices,
                                 name);
        
        mesh.first->_setBounds(Ogre::Aabb(Ogre::Vector3(-radius-halfHeight),
                                          Ogre::Vector3(radius+halfHeight) ), false );
        mesh.first->_setBoundingSphereRadius(sphereSize+halfHeight*2);
    }
    return mesh;
}
//-------------------------------------------------------------------------
}
