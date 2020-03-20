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
#include "btBulletDynamicsCommon.h"

namespace jet {
BulletConverter::BulletConverter(GraphicsSystem *graphicsSystem ){
    m_glApp = graphicsSystem;
}

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
std::pair<Ogre::MeshPtr, Ogre::VertexBufferPacked*>
BulletConverter::createCollisionShapeGraphicsObject(btCollisionShape* collisionShape,
                                                    Ogre::String name){
    btAlignedObjectArray<GLInstanceVertex> gfxVertices;
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
}
