//
//  jetSphereShape.cpp
//  Sisy
//
//  Created by 杨丰 on 2020/3/21.
//  Copyright © 2020 杨丰. All rights reserved.
//

#include "jetSphereShape.hpp"
#include "btSphereShape.h"
#include "btAlignedObjectArray.h"
#include "btBulletDynamicsCommon.h"
#include "OgreSceneManager.h"

#include "ShapeData.h"

namespace jet{
JetSphereShape::JetSphereShape(btScalar radius){
    m_shape = new btSphereShape(radius);
}
//-------------------------------------------------------------------------
Ogre::ManualObject* JetSphereShape::debugDrawObject(const btVector3& color,
                                                    Ogre::SceneManager* scene){
    const btSphereShape* sphereShape = static_cast<const btSphereShape*>(m_shape);
    btScalar radius = sphereShape->getMargin();
    btScalar minTh = -SIMD_HALF_PI;
    btScalar maxTh = SIMD_HALF_PI;
    btScalar minPs = -SIMD_HALF_PI;
    btScalar maxPs = SIMD_HALF_PI;
    btScalar stepDegrees = 30.f;
    btVector3 center(0, 0, 0);
    btVector3 up(0, 1, 0);
    
    clearManualIdx();
    Ogre::ManualObject * manualObject = scene->createManualObject();
    manualObject->begin("BaseWhite", Ogre::OT_LINE_LIST);
    
    btVector3 axis(1, 0, 0);
    drawSpherePatch(center, up, axis, radius,
                    minTh, maxTh, minPs, maxPs, color,
                    manualObject,
                    stepDegrees, false);
    
    axis = btVector3(-1, 0, 0);
    drawSpherePatch(center, up, axis, radius,
                    minTh, maxTh, minPs, maxPs, color,
                    manualObject,
                    stepDegrees, false);
    manualObject->end();
    
    return manualObject;
}
//-------------------------------------------------------------------------
std::pair<Ogre::MeshPtr, Ogre::VertexBufferPacked*>
JetSphereShape::createCollisionShapeGraphicsObject(btCollisionShape* collisionShape,
                                                   Ogre::String name){
    btAlignedObjectArray<Ogre::uint16> indices;
    std::pair<Ogre::MeshPtr, Ogre::VertexBufferPacked*> mesh;
    
    int strideInBytes = 9 * sizeof(float);
    if (collisionShape->getShapeType() == SPHERE_SHAPE_PROXYTYPE)
    {
        btSphereShape* sphereShape = (btSphereShape*)collisionShape;
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
        
        mesh.first->_setBounds(Ogre::Aabb(Ogre::Vector3(-radius),
                                          Ogre::Vector3(radius) ), false );
        mesh.first->_setBoundingSphereRadius(sphereSize);
    }
    
    return mesh;
}
}
