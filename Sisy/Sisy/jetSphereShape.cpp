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
    manual_idx = 0;
}
//-------------------------------------------------------------------------
void JetSphereShape::createRenderMesh(Ogre::String name){
    m_mesh = createCollisionShapeGraphicsObject(m_shape, name).first;
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
    btVector3 up(0, 1, 0);
    
    clearManualIdx();
    Ogre::ManualObject * manualObject = scene->createManualObject();
    manualObject->begin("BaseWhite", Ogre::OT_LINE_LIST);
    btVector3 axis(1, 0, 0);
    drawSpherePatch(up, axis, radius, minTh, maxTh, minPs, maxPs, color,
                    manualObject,
                    stepDegrees, false);
    
    axis = btVector3(-1, 0, 0);
    drawSpherePatch(up, axis, radius, minTh, maxTh, minPs, maxPs, color,
                    manualObject,
                    stepDegrees, false);
    manualObject->end();
    
    return manualObject;
}
void JetSphereShape::drawSpherePatch(const btVector3& up,
                                     const btVector3& axis,
                                     btScalar radius,
                                     btScalar minTh, btScalar maxTh,
                                     btScalar minPs, btScalar maxPs,
                                     const btVector3& color,
                                     Ogre::ManualObject* manual,
                                     btScalar stepDegrees,
                                     bool drawCenter){
    btVector3 center(0, 0, 0);
    
    btVector3 vA[74];
    btVector3 vB[74];
    btVector3 *pvA = vA, *pvB = vB, *pT;
    btVector3 npole = center + up * radius;
    btVector3 spole = center - up * radius;
    btVector3 arcStart;
    btScalar step = stepDegrees * SIMD_RADS_PER_DEG;
    const btVector3& kv = up;
    const btVector3& iv = axis;
    btVector3 jv = kv.cross(iv);
    bool drawN = false;
    bool drawS = false;
    if (minTh <= -SIMD_HALF_PI)
    {
        minTh = -SIMD_HALF_PI + step;
        drawN = true;
    }
    if (maxTh >= SIMD_HALF_PI)
    {
        maxTh = SIMD_HALF_PI - step;
        drawS = true;
    }
    if (minTh > maxTh)
    {
        minTh = -SIMD_HALF_PI + step;
        maxTh = SIMD_HALF_PI - step;
        drawN = drawS = true;
    }
    int n_hor = (int)((maxTh - minTh) / step) + 1;
    if (n_hor < 2) n_hor = 2;
    btScalar step_h = (maxTh - minTh) / btScalar(n_hor - 1);
    bool isClosed = false;
    if (minPs > maxPs)
    {
        minPs = -SIMD_PI + step;
        maxPs = SIMD_PI;
        isClosed = true;
    }
    else if ((maxPs - minPs) >= SIMD_PI * btScalar(2.f))
    {
        isClosed = true;
    }
    else
    {
        isClosed = false;
    }
    int n_vert = (int)((maxPs - minPs) / step) + 1;
    if (n_vert < 2) n_vert = 2;
    btScalar step_v = (maxPs - minPs) / btScalar(n_vert - 1);
    
    for (int i = 0; i < n_hor; i++)
    {
        btScalar th = minTh + btScalar(i) * step_h;
        btScalar sth = radius * btSin(th);
        btScalar cth = radius * btCos(th);
        for (int j = 0; j < n_vert; j++)
        {
            btScalar psi = minPs + btScalar(j) * step_v;
            btScalar sps = btSin(psi);
            btScalar cps = btCos(psi);
            pvB[j] = center + cth * cps * iv + cth * sps * jv + sth * kv;
            if (i)
            {
                drawLine(pvA[j], pvB[j], manual_idx, manual_idx+1,
                         manual);
                manual_idx += 2;
            }
            else if (drawS)
            {
                drawLine(spole, pvB[j], manual_idx, manual_idx+1,
                         manual);
                manual_idx += 2;
            }
            if (j)
            {
                drawLine(pvB[j - 1], pvB[j], manual_idx, manual_idx+1,
                         manual);
                manual_idx += 2;
            }
            else
            {
                arcStart = pvB[j];
            }
            if ((i == (n_hor - 1)) && drawN)
            {
                drawLine(npole, pvB[j], manual_idx, manual_idx+1,
                         manual);
                manual_idx += 2;
            }

            if (drawCenter)
            {
                if (isClosed)
                {
                    if (j == (n_vert - 1))
                    {
                        drawLine(arcStart, pvB[j], manual_idx, manual_idx+1,
                                 manual);
                        manual_idx += 2;
                    }
                }
                else
                {
                    if (((!i) || (i == (n_hor - 1))) && ((!j) || (j == (n_vert - 1))))
                    {
                        drawLine(center, pvB[j], manual_idx, manual_idx+1,
                                 manual);
                        manual_idx += 2;
                    }
                }
            }
        }
        pT = pvA;
        pvA = pvB;
        pvB = pT;
    }
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
        
        mesh.first->_setBounds(Ogre::Aabb(Ogre::Vector3(radius),
                                          Ogre::Vector3(radius) ), false );
        mesh.first->_setBoundingSphereRadius(sphereSize);
    }
    
    return mesh;
}
}
