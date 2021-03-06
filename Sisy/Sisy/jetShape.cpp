//
//  jetShape.cpp
//  Sisy
//
//  Created by 杨丰 on 2020/3/20.
//  Copyright © 2020 杨丰. All rights reserved.
//

#include "jetShape.hpp"
#include "OgreSubMesh2.h"
#include "OgreMeshManager2.h"
#include "OgreRenderSystem.h"
#include "OgreRoot.h"

#include "btAlignedObjectArray.h"
#include "btBulletDynamicsCommon.h"
#include "CollisionShape2TriangleMesh.h"
#include "InstanceGraphicsShape.h"

namespace jet{
Ogre::IndexBufferPacked*
JetShape::createIndexBuffer(const Ogre::uint16* indices, int numIndices){
    Ogre::IndexBufferPacked *indexBuffer = 0;
    
    Ogre::uint16 *cubeIndices
    = reinterpret_cast<Ogre::uint16*>( OGRE_MALLOC_SIMD(
                                                        sizeof(Ogre::uint16) * numIndices,
                                                        Ogre::MEMCATEGORY_GEOMETRY ) );
    memcpy( cubeIndices, indices, sizeof(Ogre::uint16) * numIndices );
    
    Ogre::Root *root = Ogre::Root::getSingletonPtr();
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
//-------------------------------------------------------------------------
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
JetShape::createDynamicMesh(const float* vertices, int numvertices,
                            const Ogre::uint16* indices, int numIndices, Ogre::String name){
    Ogre::Root *root = Ogre::Root::getSingletonPtr();
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
//-------------------------------------------------------------------------
void JetShape::createRenderMesh(Ogre::String name){
    m_mesh = createCollisionShapeGraphicsObject(m_shape, name).first;
}
//-------------------------------------------------------------------------
std::pair<Ogre::MeshPtr, Ogre::VertexBufferPacked*>
JetShape::createCollisionShapeGraphicsObject(btCollisionShape* collisionShape,
                                             Ogre::String name){
    std::pair<Ogre::MeshPtr, Ogre::VertexBufferPacked*> mesh;
    
    btTransform startTrans;
    startTrans.setIdentity();
    //todo: create some textured objects for popular objects, like plane, cube, sphere, capsule
    btAlignedObjectArray<Ogre::uint16> indices;
    btAlignedObjectArray<btVector3> vertexPositions;
    btAlignedObjectArray<btVector3> vertexNormals;
    CollisionShape2TriangleMesh(m_shape, startTrans, vertexPositions, vertexNormals, indices);
    
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
//-------------------------------------------------------------------------
}
