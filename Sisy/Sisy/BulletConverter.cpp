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

#include "InstanceGraphicsShape.h"
#include "btAlignedObjectArray.h"
#include "ShapeData.h"
#include "CollisionShape2TriangleMesh.h"
#include "btBulletDynamicsCommon.h"

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
    
    if (collisionShape->getShapeType() == SPHERE_SHAPE_PROXYTYPE)
    {
        sphereCreator(collisionShape, name);
        return mesh;
    }
    
    if (collisionShape->getShapeType() == CAPSULE_SHAPE_PROXYTYPE)
    {
        capsuleCreator(collisionShape, name);
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
BulletConverter::capsuleCreator(btCollisionShape* collisionShape,
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
