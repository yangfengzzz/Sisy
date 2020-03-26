//
//  BulletConverter.cpp
//  Sisy
//
//  Created by 杨丰 on 2020/3/19.
//  Copyright © 2020 杨丰. All rights reserved.
//

#include "BulletConverter.hpp"

#include "OgreRoot.h"
#include "OgreMeshManager2.h"
#include "OgreMesh2.h"
#include "OgreSubMesh2.h"
#include "OgreItem.h"
#include "GraphicsSystem.h"

#include "ShapeData.h"
#include "CollisionShape2TriangleMesh.h"
#include "btBulletDynamicsCommon.h"
#include "btHeightfieldTerrainShape.h"
#include "btSoftBody.h"

namespace jet {
BulletConverter::BulletConverter(GraphicsSystem *graphicsSystem ){
    m_glApp = graphicsSystem;
}
std::pair<Ogre::MeshPtr, Ogre::VertexBufferPacked*>
BulletConverter::createCollisionShapeGraphicsObject(btCollisionShape* collisionShape,
                                                    Ogre::String name){
    std::pair<Ogre::MeshPtr, Ogre::VertexBufferPacked*> mesh;
    if (collisionShape->getShapeType() == BOX_SHAPE_PROXYTYPE)
    {
        boxCreator(collisionShape, name);
        return mesh;
    }
    
    if (collisionShape->getShapeType() == TERRAIN_SHAPE_PROXYTYPE)
    {
        TerrianCreator(collisionShape, name);
        return mesh;
    }
    
    if (collisionShape->getShapeType() == SOFTBODY_SHAPE_PROXYTYPE)
    {
        softBodyCreator(collisionShape, name);
        return mesh;
    }
    
    if (collisionShape->getShapeType() == MULTI_SPHERE_SHAPE_PROXYTYPE)
    {
        multi_sphereCreator(collisionShape, name);
        return mesh;
    }
    
    if (collisionShape->getShapeType() == SPHERE_SHAPE_PROXYTYPE)
    {
        sphereCreator(collisionShape, name);
        return mesh;
    }
    
    if (collisionShape->getShapeType() == COMPOUND_SHAPE_PROXYTYPE)
    {
        btCompoundShape* compound = (btCompoundShape*)collisionShape;
        if (compound->getNumChildShapes() == 1)
        {
            if (compound->getChildShape(0)->getShapeType() == SPHERE_SHAPE_PROXYTYPE)
            {
                sphereCreator(compound->getChildShape(0), name);
                return mesh;
            }
            
            if (compound->getChildShape(0)->getShapeType() == CAPSULE_SHAPE_PROXYTYPE)
            {
                capsuleCreator(compound->getChildShape(0), name);
                return mesh;
            }
            
            if (compound->getChildShape(0)->getShapeType() == MULTI_SPHERE_SHAPE_PROXYTYPE)
            {
                multi_sphereCreator(collisionShape, name);
                return mesh;
            }
        }
    }
    
    if (collisionShape->getShapeType() == CAPSULE_SHAPE_PROXYTYPE)
    {
        capsuleCreator(collisionShape, name);
        return mesh;
    }
    
    if (collisionShape->getShapeType() == STATIC_PLANE_PROXYTYPE)
    {
        staticPlaneCreator(collisionShape, name);
        return mesh;
    }
    
    defaultCreator(collisionShape, name);
    return mesh;
    
}
//--------------------------------------------------------------------------------
std::pair<Ogre::MeshPtr, Ogre::VertexBufferPacked*>
BulletConverter::defaultCreator(btCollisionShape* collisionShape,
                                Ogre::String name){
    std::pair<Ogre::MeshPtr, Ogre::VertexBufferPacked*> mesh;
    
    btTransform startTrans;
    startTrans.setIdentity();
    //todo: create some textured objects for popular objects, like plane, cube, sphere, capsule
    btAlignedObjectArray<Ogre::uint16> indices;
    btAlignedObjectArray<btVector3> vertexPositions;
    btAlignedObjectArray<btVector3> vertexNormals;
    CollisionShape2TriangleMesh(collisionShape, startTrans, vertexPositions, vertexNormals, indices);
    
    btAlignedObjectArray<float> gfxVertices;
    gfxVertices.resize(vertexPositions.size()*6);
    for (int i = 0; i < vertexPositions.size(); i++)
    {
        gfxVertices[i * 6 + 0] = vertexPositions[i][0];
        gfxVertices[i * 6 + 1] = vertexPositions[i][1];
        gfxVertices[i * 6 + 2] = vertexPositions[i][2];
        gfxVertices[i * 6 + 3] = vertexNormals[i][0];
        gfxVertices[i * 6 + 4] = vertexNormals[i][1];
        gfxVertices[i * 6 + 5] = vertexNormals[i][2];
    }
    
    mesh = createDynamicMesh(&gfxVertices[0], vertexPositions.size(),
                             &indices[0], indices.size(),
                             name);
    
    btTransform t = btTransform::getIdentity();
    btVector3 aabbMin;
    btVector3 aabbMax;
    collisionShape->getAabb(t, aabbMin, aabbMax);
    mesh.first->_setBounds(Ogre::Aabb(Ogre::Vector3(aabbMin.x(), aabbMin.y(), aabbMin.z()),
                                      Ogre::Vector3(aabbMax.x(), aabbMax.y(), aabbMax.z()) ), false );
    mesh.first->_setBoundingSphereRadius((aabbMax-aabbMin).length());
    
    return mesh;
}
//--------------------------------------------------------------------------------
std::pair<Ogre::MeshPtr, Ogre::VertexBufferPacked*>
BulletConverter::boxCreator(btCollisionShape* collisionShape,
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
//--------------------------------------------------------------------------------
class MyTriangleCollector2 : public btTriangleCallback
{
public:
    btAlignedObjectArray<GLInstanceVertex>* m_pVerticesOut;
    btAlignedObjectArray<Ogre::uint16>* m_pIndicesOut;
    btVector3 m_aabbMin, m_aabbMax;
    btScalar m_textureScaling;
    
    MyTriangleCollector2(const btVector3& aabbMin, const btVector3& aabbMax)
    :m_aabbMin(aabbMin), m_aabbMax(aabbMax), m_textureScaling(1)
    {
        m_pVerticesOut = 0;
        m_pIndicesOut = 0;
    }
    
    virtual void processTriangle(btVector3* tris, int partId, int triangleIndex)
    {
        for (int k = 0; k < 3; k++)
        {
            GLInstanceVertex v;
            v.xyzw[3] = 0;
            
            btVector3 normal = (tris[0] - tris[1]).cross(tris[0] - tris[2]);
            normal.safeNormalize();
            for (int l = 0; l < 3; l++)
            {
                v.xyzw[l] = tris[k][l];
                v.normal[l] = normal[l];
            }
            
            v.uv[0] = (1.-((v.xyzw[0] - m_aabbMin[0]) / (m_aabbMax[0] - m_aabbMin[0])))*m_textureScaling;
            v.uv[1] = (1.-(v.xyzw[1] - m_aabbMin[1]) / (m_aabbMax[1] - m_aabbMin[1]))*m_textureScaling;
            
            m_pIndicesOut->push_back((Ogre::uint16)m_pVerticesOut->size());
            m_pVerticesOut->push_back(v);
        }
    }
};
std::pair<Ogre::MeshPtr, Ogre::VertexBufferPacked*>
BulletConverter::TerrianCreator(btCollisionShape* collisionShape,
                                Ogre::String name){
    btAlignedObjectArray<GLInstanceVertex> gfxVertices;
    btAlignedObjectArray<Ogre::uint16> indices;
    std::pair<Ogre::MeshPtr, Ogre::VertexBufferPacked*> mesh;
    
    if (collisionShape->getShapeType() == TERRAIN_SHAPE_PROXYTYPE)
    {
        const btHeightfieldTerrainShape* heightField = static_cast<const btHeightfieldTerrainShape*>(collisionShape);
        
        btVector3 aabbMin, aabbMax;
        btTransform tr;
        tr.setIdentity();
        heightField->getAabb(tr, aabbMin, aabbMax);
        MyTriangleCollector2  col(aabbMin, aabbMax);
        if (heightField->getUserValue3())
        {
            col.m_textureScaling = heightField->getUserValue3();
        }
        col.m_pVerticesOut = &gfxVertices;
        col.m_pIndicesOut = &indices;
        for (int k = 0; k < 3; k++)
        {
            aabbMin[k] = -BT_LARGE_FLOAT;
            aabbMax[k] = BT_LARGE_FLOAT;
        }
        heightField->processAllTriangles(&col, aabbMin, aabbMax);
        
        if (gfxVertices.size() && indices.size())
        {
            btAlignedObjectArray<float> vertexPositions;
            vertexPositions.resize(gfxVertices.size()*6);
            for (int i = 0; i < gfxVertices.size(); i++)
            {
                vertexPositions[i * 6 + 0] = gfxVertices[i].xyzw[0];
                vertexPositions[i * 6 + 1] = gfxVertices[i].xyzw[1];
                vertexPositions[i * 6 + 2] = gfxVertices[i].xyzw[2];
                vertexPositions[i * 6 + 3] = gfxVertices[i].normal[0];
                vertexPositions[i * 6 + 4] = gfxVertices[i].normal[1];
                vertexPositions[i * 6 + 5] = gfxVertices[i].normal[2];
            }
            
            mesh = createDynamicMesh(&vertexPositions[0], gfxVertices.size(),
                                     &indices[0], indices.size(),
                                     name);
            
            btTransform t = btTransform::getIdentity();
            btVector3 aabbMin;
            btVector3 aabbMax;
            collisionShape->getAabb(t, aabbMin, aabbMax);
            mesh.first->_setBounds(Ogre::Aabb(Ogre::Vector3(aabbMin.x(), aabbMin.y(), aabbMin.z()),
                                              Ogre::Vector3(aabbMax.x(), aabbMax.y(), aabbMax.z()) ), false );
            mesh.first->_setBoundingSphereRadius((aabbMax-aabbMin).length());
        }
    }
    return mesh;
}
//--------------------------------------------------------------------------------
std::pair<Ogre::MeshPtr, Ogre::VertexBufferPacked*>
BulletConverter::sphereCreator(btCollisionShape* collisionShape,
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
//--------------------------------------------------------------------------------
std::pair<Ogre::MeshPtr, Ogre::VertexBufferPacked*>
BulletConverter::multi_sphereCreator(btCollisionShape* collisionShape,
                                     Ogre::String name){
    btAlignedObjectArray<Ogre::uint16> indices;
    std::pair<Ogre::MeshPtr, Ogre::VertexBufferPacked*> mesh;
    
    int strideInBytes = 9 * sizeof(float);
    if (collisionShape->getShapeType() == MULTI_SPHERE_SHAPE_PROXYTYPE)
    {
        btMultiSphereShape* ms = (btMultiSphereShape*)collisionShape;
        if (ms->getSphereCount() == 2)
        {
            btAlignedObjectArray<float> transformedVertices;
            int numVertices = sizeof(textured_detailed_sphere_vertices) / strideInBytes;
            transformedVertices.resize(numVertices * 9);
            btVector3 sphere0Pos = ms->getSpherePosition(0);
            btVector3 sphere1Pos = ms->getSpherePosition(1);
            btVector3 fromTo = sphere1Pos - sphere0Pos;
            //cache miss
            for (int i = 0; i < numVertices; i++)
            {
                btVector3 vert;
                vert.setValue(textured_detailed_sphere_vertices[i * 9 + 0],
                              textured_detailed_sphere_vertices[i * 9 + 1],
                              textured_detailed_sphere_vertices[i * 9 + 2]);
                
                btVector3 trVer(0, 0, 0);
                
                if (vert.dot(fromTo) > 0)
                {
                    btScalar radiusScale = 2. * ms->getSphereRadius(1);
                    trVer = radiusScale * vert;
                    trVer += sphere1Pos;
                }
                else
                {
                    btScalar radiusScale = 2. * ms->getSphereRadius(0);
                    trVer = radiusScale * vert;
                    trVer += sphere0Pos;
                }
                
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
            
            btTransform t = btTransform::getIdentity();
            btVector3 aabbMin;
            btVector3 aabbMax;
            collisionShape->getAabb(t, aabbMin, aabbMax);
            mesh.first->_setBounds(Ogre::Aabb(Ogre::Vector3(aabbMin.x(), aabbMin.y(), aabbMin.z()),
                                              Ogre::Vector3(aabbMax.x(), aabbMax.y(), aabbMax.z()) ), false );
            mesh.first->_setBoundingSphereRadius((aabbMax-aabbMin).length());
        }
    }
    return mesh;
}
//--------------------------------------------------------------------------------
std::pair<Ogre::MeshPtr, Ogre::VertexBufferPacked*>
BulletConverter::capsuleCreator(btCollisionShape* collisionShape,
                                Ogre::String name){
    btAlignedObjectArray<Ogre::uint16> indices;
    std::pair<Ogre::MeshPtr, Ogre::VertexBufferPacked*> mesh;
    
    int strideInBytes = 9 * sizeof(float);
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
//--------------------------------------------------------------------------------
std::pair<Ogre::MeshPtr, Ogre::VertexBufferPacked*>
BulletConverter::staticPlaneCreator(btCollisionShape* collisionShape,
                                    Ogre::String name){
    btAlignedObjectArray<Ogre::uint16> indices;
    btAlignedObjectArray<float> gfxVertices;
    
    std::pair<Ogre::MeshPtr, Ogre::VertexBufferPacked*> mesh;
    if (collisionShape->getShapeType() == STATIC_PLANE_PROXYTYPE)
    {
        const btStaticPlaneShape* staticPlaneShape = static_cast<const btStaticPlaneShape*>(collisionShape);
        btScalar planeConst = staticPlaneShape->getPlaneConstant();
        const btVector3& planeNormal = staticPlaneShape->getPlaneNormal();
        btVector3 planeOrigin = planeNormal * planeConst;
        btVector3 vec0, vec1;
        btPlaneSpace1(planeNormal, vec0, vec1);
        
        btScalar vecLen = 128;
        btVector3 verts[4];
        
        verts[0] = planeOrigin + vec0 * vecLen + vec1 * vecLen;
        verts[1] = planeOrigin - vec0 * vecLen + vec1 * vecLen;
        verts[2] = planeOrigin - vec0 * vecLen - vec1 * vecLen;
        verts[3] = planeOrigin + vec0 * vecLen - vec1 * vecLen;
        
        Ogre::uint16 startIndex = 0;
        indices.push_back(startIndex + 0);
        indices.push_back(startIndex + 1);
        indices.push_back(startIndex + 2);
        indices.push_back(startIndex + 0);
        indices.push_back(startIndex + 2);
        indices.push_back(startIndex + 3);
        btTransform parentTransform;
        parentTransform.setIdentity();
        btVector3 triNormal = parentTransform.getBasis() * planeNormal;
        
        gfxVertices.resize(4*6);
        
        for (int i = 0; i < 4; i++)
        {
            btVector3 vtxPos;
            btVector3 pos = parentTransform * verts[i];
            
            gfxVertices[i*6] = pos[0];
            gfxVertices[i*6+1] = pos[1];
            gfxVertices[i*6+2] = pos[2];
            gfxVertices[i*6+3] = triNormal[0];
            gfxVertices[i*6+4] = triNormal[1];
            gfxVertices[i*6+5] = triNormal[2];
        }
        
        mesh = createDynamicMesh(&gfxVertices[0], 4,
                                 &indices[0], indices.size(),
                                 name);
        
        mesh.first->_setBounds(Ogre::Aabb(Ogre::Vector3(planeOrigin
                                                        - vec0.absolute()*vecLen
                                                        - vec1.absolute()*vecLen),
                                          Ogre::Vector3(planeOrigin
                                                        + vec0.absolute()*vecLen
                                                        + vec1.absolute()*vecLen) ), false );
        mesh.first->_setBoundingSphereRadius(vecLen*2*std::sqrt(2.0));
    }
    
    return mesh;
}
//--------------------------------------------------------------------------------
std::pair<Ogre::MeshPtr, Ogre::VertexBufferPacked*>
BulletConverter::softBodyCreator(btCollisionShape* collisionShape,
                                 Ogre::String name){
    btAlignedObjectArray<Ogre::uint16> indices;
    btAlignedObjectArray<GLInstanceVertex> gfxVertices;
    std::pair<Ogre::MeshPtr, Ogre::VertexBufferPacked*> mesh;
    computeSoftBodyVertices(collisionShape, gfxVertices, indices);
    
    if (gfxVertices.size() && indices.size())
    {
        btAlignedObjectArray<float> vertexPositions;
        vertexPositions.resize(gfxVertices.size()*6);
        for (int i = 0; i < gfxVertices.size(); i++)
        {
            vertexPositions[i * 6 + 0] = gfxVertices[i].xyzw[0];
            vertexPositions[i * 6 + 1] = gfxVertices[i].xyzw[1];
            vertexPositions[i * 6 + 2] = gfxVertices[i].xyzw[2];
            vertexPositions[i * 6 + 3] = gfxVertices[i].normal[0];
            vertexPositions[i * 6 + 4] = gfxVertices[i].normal[1];
            vertexPositions[i * 6 + 5] = gfxVertices[i].normal[2];
        }
        
        mesh = createDynamicMesh(&vertexPositions[0], gfxVertices.size(),
                                 &indices[0], indices.size(),
                                 name);
        
        btTransform t = btTransform::getIdentity();
        btVector3 aabbMin;
        btVector3 aabbMax;
        collisionShape->getAabb(t, aabbMin, aabbMax);
        mesh.first->_setBounds(Ogre::Aabb(Ogre::Vector3(aabbMin.x(), aabbMin.y(), aabbMin.z()),
                                          Ogre::Vector3(aabbMax.x(), aabbMax.y(), aabbMax.z()) ), false );
        mesh.first->_setBoundingSphereRadius((aabbMax-aabbMin).length());
    }
    
    return mesh;
}
void BulletConverter::computeSoftBodyVertices(btCollisionShape* collisionShape,
                                              btAlignedObjectArray<GLInstanceVertex>& gfxVertices,
                                              btAlignedObjectArray<Ogre::uint16>& indices){
    btSoftBody* psb = (btSoftBody*)collisionShape->getUserPointer();
    gfxVertices.resize(psb->m_faces.size() * 3);

    for (int i = 0; i < psb->m_faces.size(); i++)  // Foreach face
    {
        for (int k = 0; k < 3; k++)  // Foreach vertex on a face
        {
            int currentIndex = i * 3 + k;
            for (int j = 0; j < 3; j++)
            {
                gfxVertices[currentIndex].xyzw[j] = psb->m_faces[i].m_n[k]->m_x[j];
            }
            for (int j = 0; j < 3; j++)
            {
                gfxVertices[currentIndex].normal[j] = psb->m_faces[i].m_n[k]->m_n[j];
            }
            for (int j = 0; j < 2; j++)
            {
                gfxVertices[currentIndex].uv[j] = 0.5;  //we don't have UV info...
            }
            indices.push_back(currentIndex);
        }
    }
}
//--------------------------------------------------------------------------------
Ogre::IndexBufferPacked*
BulletConverter::createIndexBuffer(const Ogre::uint16* indices, int numIndices){
    Ogre::IndexBufferPacked *indexBuffer = 0;
    
    Ogre::uint16 *cubeIndices
    = reinterpret_cast<Ogre::uint16*>( OGRE_MALLOC_SIMD(
                                                        sizeof(Ogre::uint16) * numIndices,
                                                        Ogre::MEMCATEGORY_GEOMETRY ) );
    memcpy( cubeIndices, indices, sizeof(Ogre::uint16) * numIndices );
    
    Ogre::Root *root = m_glApp->getRoot();
    Ogre::RenderSystem *renderSystem = root->getRenderSystem();
    Ogre::VaoManager *vaoManager = renderSystem->getVaoManager();
    
    try
    {
        indexBuffer = vaoManager->createIndexBuffer( Ogre::IndexBufferPacked::IT_16BIT,
                                                    numIndices,
                                                    Ogre::BT_IMMUTABLE,
                                                    cubeIndices, true );
    }
    catch( Ogre::Exception &e )
    {
        // When keepAsShadow = true, the memory will be freed when the index buffer is destroyed.
        // However if for some weird reason there is an exception raised, the memory will
        // not be freed, so it is up to us to do so.
        // The reasons for exceptions are very rare. But we're doing this for correctness.
        // Important: Please note that we passed keepAsShadow = true to createIndexBuffer,
        // thus Ogre will free the pointer. However had we passed keepAsShadow = false,
        // it would be YOUR responsability to free the pointer, not Ogre's
        OGRE_FREE_SIMD( indexBuffer, Ogre::MEMCATEGORY_GEOMETRY );
        indexBuffer = 0;
        throw e;
    }
    
    return indexBuffer;
}
//--------------------------------------------------------------------------------
struct CubeVertices
{
    float px, py, pz;   //Position
    float nx, ny, nz;   //Normals
    
    CubeVertices() {}
    CubeVertices( float _px, float _py, float _pz,
                 float _nx, float _ny, float _nz ) :
    px( _px ), py( _py ), pz( _pz ),
    nx( _nx ), ny( _ny ), nz( _nz )
    {
    }
};

std::pair<Ogre::MeshPtr, Ogre::VertexBufferPacked*>
BulletConverter::createDynamicMesh(const float* vertices, int numvertices,
                                   const Ogre::uint16* indices, int numIndices,
                                   Ogre::String name){
    Ogre::Root *root = m_glApp->getRoot();
    Ogre::RenderSystem *renderSystem = root->getRenderSystem();
    Ogre::VaoManager *vaoManager = renderSystem->getVaoManager();
    
    //Create the mesh
    Ogre::MeshPtr mesh
    = Ogre::MeshManager::getSingleton().createManual(
                                                     name,
                                                     Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME );
    
    //Create one submesh
    Ogre::SubMesh *subMesh = mesh->createSubMesh();
    
    //Vertex declaration
    Ogre::VertexElement2Vec vertexElements;
    vertexElements.push_back( Ogre::VertexElement2( Ogre::VET_FLOAT3, Ogre::VES_POSITION ) );
    vertexElements.push_back( Ogre::VertexElement2( Ogre::VET_FLOAT3, Ogre::VES_NORMAL ) );
    
    //For immutable buffers, it is mandatory that cubeVertices is not a null pointer.
    CubeVertices *cubeVertices
    = reinterpret_cast<CubeVertices*>( OGRE_MALLOC_SIMD(sizeof(CubeVertices) * numvertices,
                                                        Ogre::MEMCATEGORY_GEOMETRY ) );
    //Fill the data.
    memcpy( cubeVertices, vertices,
           sizeof(CubeVertices) * numvertices );
    
    Ogre::VertexBufferPacked *vertexBuffer = 0;
    try
    {
        //Create the actual vertex buffer.
        vertexBuffer = vaoManager->createVertexBuffer( vertexElements,
                                                      numvertices,
                                                      Ogre::BT_DYNAMIC_PERSISTENT,
                                                      cubeVertices, false );
    }
    catch( Ogre::Exception &e )
    {
        // Important: Please note that we passed keepAsShadow = true to createVertexBuffer,
        // thus Ogre will free the pointer. However had we passed keepAsShadow = false,
        // it would be YOUR responsability to free the pointer, not Ogre's
        OGRE_FREE_SIMD( vertexBuffer, Ogre::MEMCATEGORY_GEOMETRY );
        vertexBuffer = 0;
        throw e;
    }
    
    //Now the Vao. We'll just use one vertex buffer source (multi-source not working yet)
    Ogre::VertexBufferPackedVec vertexBuffers;
    vertexBuffers.push_back( vertexBuffer );
    Ogre::IndexBufferPacked *indexBuffer = createIndexBuffer(indices, numIndices); //Create the actual index buffer
    Ogre::VertexArrayObject *vao = vaoManager->createVertexArrayObject(
                                                                       vertexBuffers, indexBuffer,
                                                                       Ogre::OT_TRIANGLE_LIST );
    
    //Each Vao pushed to the vector refers to an LOD level.
    //Must be in sync with mesh->mLodValues & mesh->mNumLods if you use more than one level
    subMesh->mVao[Ogre::VpNormal].push_back( vao );
    //Use the same geometry for shadow casting.
    subMesh->mVao[Ogre::VpShadow].push_back( vao );
    
    //Set the bounds to get frustum culling and LOD to work correctly.
    mesh->_setBounds( Ogre::Aabb(Ogre::Vector3::ZERO,
                                 50*Ogre::Vector3::UNIT_SCALE ), false );
    mesh->_setBoundingSphereRadius(50 );
    
    return std::pair<Ogre::MeshPtr, Ogre::VertexBufferPacked*>( mesh, vertexBuffer );
}
//--------------------------------------------------------------------------------
}
