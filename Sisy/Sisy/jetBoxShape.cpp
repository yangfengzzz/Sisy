//
//  jetBoxShape.cpp
//  Sisy
//
//  Created by 杨丰 on 2020/3/20.
//  Copyright © 2020 杨丰. All rights reserved.
//

#include "jetBoxShape.hpp"
#include "btBoxShape.h"
#include "btAlignedObjectArray.h"
#include "btBulletDynamicsCommon.h"
#include "OgreSceneManager.h"

#include "ShapeData.h"

namespace jet{
JetBoxShape::JetBoxShape(const btVector3& boxHalfExtents){
    m_shape = new btBoxShape(boxHalfExtents);
}
//-------------------------------------------------------------------------
void JetBoxShape::createRenderMesh(Ogre::String name){
    m_mesh = createCollisionShapeGraphicsObject(m_shape, name).first;
}

Ogre::ManualObject* JetBoxShape::debugDrawObject(const btVector3& color,
                                                 Ogre::SceneManager* scene){
    const btBoxShape* boxShape = static_cast<const btBoxShape*>(m_shape);
    btVector3 bbMax = boxShape->getHalfExtentsWithMargin();
    btVector3 bbMin = -bbMax;
    Ogre::ManualObject * manualObject = scene->createManualObject();

    manualObject->begin("BaseWhite", Ogre::OT_LINE_LIST);
    drawLine( btVector3(bbMin[0], bbMin[1], bbMin[2]),  btVector3(bbMax[0], bbMin[1], bbMin[2]),
             0, 1, manualObject);
    drawLine( btVector3(bbMax[0], bbMin[1], bbMin[2]),  btVector3(bbMax[0], bbMax[1], bbMin[2]),
             2, 3, manualObject);
    drawLine( btVector3(bbMax[0], bbMax[1], bbMin[2]),  btVector3(bbMin[0], bbMax[1], bbMin[2]),
             4, 5, manualObject);
    drawLine( btVector3(bbMin[0], bbMax[1], bbMin[2]),  btVector3(bbMin[0], bbMin[1], bbMin[2]),
             6, 7, manualObject);
    drawLine( btVector3(bbMin[0], bbMin[1], bbMin[2]),  btVector3(bbMin[0], bbMin[1], bbMax[2]),
             8, 9, manualObject);
    drawLine( btVector3(bbMax[0], bbMin[1], bbMin[2]),  btVector3(bbMax[0], bbMin[1], bbMax[2]),
             10, 11, manualObject);
    drawLine( btVector3(bbMax[0], bbMax[1], bbMin[2]),  btVector3(bbMax[0], bbMax[1], bbMax[2]),
             12, 13, manualObject);
    drawLine( btVector3(bbMin[0], bbMax[1], bbMin[2]),  btVector3(bbMin[0], bbMax[1], bbMax[2]),
             14, 15, manualObject);
    drawLine( btVector3(bbMin[0], bbMin[1], bbMax[2]),  btVector3(bbMax[0], bbMin[1], bbMax[2]),
             16, 17, manualObject);
    drawLine( btVector3(bbMax[0], bbMin[1], bbMax[2]),  btVector3(bbMax[0], bbMax[1], bbMax[2]),
             18, 19, manualObject);
    drawLine( btVector3(bbMax[0], bbMax[1], bbMax[2]),  btVector3(bbMin[0], bbMax[1], bbMax[2]),
             20, 21, manualObject);
    drawLine( btVector3(bbMin[0], bbMax[1], bbMax[2]),  btVector3(bbMin[0], bbMin[1], bbMax[2]),
             22, 23, manualObject);
    manualObject->end();
    
    return manualObject;
}
//-------------------------------------------------------------------------
std::pair<Ogre::MeshPtr, Ogre::VertexBufferPacked*>
JetBoxShape::createCollisionShapeGraphicsObject(btCollisionShape* collisionShape,
                                                Ogre::String name){
    btAlignedObjectArray<Ogre::uint16> indices;
    std::pair<Ogre::MeshPtr, Ogre::VertexBufferPacked*> mesh;
    
    int strideInBytes = 9 * sizeof(float);
    if (collisionShape->getShapeType() == BOX_SHAPE_PROXYTYPE)
    {
        btBoxShape* boxShape = (btBoxShape*)collisionShape;
        
        btAlignedObjectArray<float> transformedVertices;
        
        btVector3 halfExtents = boxShape->getHalfExtentsWithMargin();
        
        int numVertices = sizeof(cube_vertices_textured) / strideInBytes;
        transformedVertices.resize(numVertices * 6);
        for (int i = 0; i < numVertices; i++)
        {
            btVector3 vert;
            vert.setValue(cube_vertices_textured[i * 9 + 0],
                          cube_vertices_textured[i * 9 + 1],
                          cube_vertices_textured[i * 9 + 2]);
            
            btVector3 trVer = halfExtents * vert;
            transformedVertices[i * 6 + 0] = trVer[0];
            transformedVertices[i * 6 + 1] = trVer[1];
            transformedVertices[i * 6 + 2] = trVer[2];
            transformedVertices[i * 6 + 3] = cube_vertices_textured[i * 9 + 4];
            transformedVertices[i * 6 + 4] = cube_vertices_textured[i * 9 + 5];
            transformedVertices[i * 6 + 5] = cube_vertices_textured[i * 9 + 6];
        }
        
        int numIndices = sizeof(cube_indices) / sizeof(Ogre::uint16);
        mesh = createDynamicMesh(&transformedVertices[0], numVertices,
                                 cube_indices, numIndices,
                                 name);
        
        mesh.first->_setBounds(Ogre::Aabb(Ogre::Vector3::ZERO,
                                          Ogre::Vector3(halfExtents.x(),
                                                        halfExtents.y(),
                                                        halfExtents.z()) ), false );
        mesh.first->_setBoundingSphereRadius(halfExtents.length());
    }
    
    return mesh;
}
//-------------------------------------------------------------------------
}
