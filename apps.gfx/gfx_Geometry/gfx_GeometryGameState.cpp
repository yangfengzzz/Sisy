
#include "gfx_GeometryGameState.h"
#include "CameraController.h"
#include "GraphicsSystem.h"

#include "OgreSceneManager.h"
#include "OgreItem.h"
#include "OgreMesh.h"
#include "OgreMeshManager.h"
#include "OgreMesh2.h"
#include "OgreMeshManager2.h"
#include "OgreSubMesh2.h"
#include "OgreMesh2Serializer.h"

#include "OgreRoot.h"
#include "../../src.gfx/Vao/OgreVaoManager.h"
#include "../../src.gfx/Vao/OgreVertexArrayObject.h"

#include "OgreCamera.h"

using namespace Demo;

namespace Demo
    {
    DynamicGeometryGameState::DynamicGeometryGameState( const Ogre::String &helpDescription ) :
    TutorialGameState( helpDescription ),
    mRotationTime( 0.0f )
    {
        obj = std::make_shared<jet::TriangleMesh3>();
        obj->readObj("spherical_points_to_implicit3_convert_two.obj");
        assert(obj->numberOfPoints()==obj->numberOfNormals());
        c_originalVertices.resize(obj->numberOfPoints());
        
        for (int i = 0; i < obj->numberOfPoints(); i++) {
            jet::Vector3D pt = obj->point(i);
            c_originalVertices[i].px = pt.x;
            c_originalVertices[i].py = pt.y;
            c_originalVertices[i].pz = pt.z;
            
            jet::Vector3D normal = obj->normal(i);
            c_originalVertices[i].nx = normal.x;
            c_originalVertices[i].ny = normal.y;
            c_originalVertices[i].nz = normal.z;
        }
        
        c_indexData.resize(obj->numberOfTriangles()*3);        
        for (int i = 0; i < obj->numberOfTriangles(); i++) {
            jet::Point3UI index = obj->pointIndex(i);
            c_indexData[3*i] = index.x;
            c_indexData[3*i+1] = index.y;
            c_indexData[3*i+2] = index.z;
        }
    }
    //-----------------------------------------------------------------------------------
    Ogre::IndexBufferPacked* DynamicGeometryGameState::createIndexBuffer(void)
    {
        Ogre::IndexBufferPacked *indexBuffer = 0;
        
        Ogre::uint16 *cubeIndices
        = reinterpret_cast<Ogre::uint16*>( OGRE_MALLOC_SIMD(
                                                            sizeof(Ogre::uint16) * c_indexData.size(),
                                                            Ogre::MEMCATEGORY_GEOMETRY ) );
        memcpy( cubeIndices, c_indexData.data(), sizeof(Ogre::uint16) * c_indexData.size() );
        
        Ogre::Root *root = mGraphicsSystem->getRoot();
        Ogre::RenderSystem *renderSystem = root->getRenderSystem();
        Ogre::VaoManager *vaoManager = renderSystem->getVaoManager();
        
        try
        {
            indexBuffer = vaoManager->createIndexBuffer( Ogre::IndexBufferPacked::IT_16BIT,
                                                        c_indexData.size(),
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
    //-----------------------------------------------------------------------------------
    std::pair<Ogre::MeshPtr, Ogre::VertexBufferPacked*>
    DynamicGeometryGameState::createDynamicMesh(size_t idx )
    {
        Ogre::Root *root = mGraphicsSystem->getRoot();
        Ogre::RenderSystem *renderSystem = root->getRenderSystem();
        Ogre::VaoManager *vaoManager = renderSystem->getVaoManager();
        
        //Create the mesh
        Ogre::MeshPtr mesh = Ogre::MeshManager::getSingleton().createManual(
        "My DynamicMesh_" + Ogre::StringConverter::toString(idx),
        Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME );
        
        //Create one submesh
        Ogre::SubMesh *subMesh = mesh->createSubMesh();
        
        //Vertex declaration
        Ogre::VertexElement2Vec vertexElements;
        vertexElements.push_back( Ogre::VertexElement2( Ogre::VET_FLOAT3, Ogre::VES_POSITION ) );
        vertexElements.push_back( Ogre::VertexElement2( Ogre::VET_FLOAT3, Ogre::VES_NORMAL ) );
        
        //For immutable buffers, it is mandatory that cubeVertices is not a null pointer.
        CubeVertices *cubeVertices
        = reinterpret_cast<CubeVertices*>( OGRE_MALLOC_SIMD(sizeof(CubeVertices) * c_originalVertices.size(),
                                                            Ogre::MEMCATEGORY_GEOMETRY ) );
        //Fill the data.
        memcpy( cubeVertices, c_originalVertices.data(),
               sizeof(CubeVertices) * c_originalVertices.size() );
        
        Ogre::VertexBufferPacked *vertexBuffer = 0;
        try
        {
            //Create the actual vertex buffer.
            vertexBuffer = vaoManager->createVertexBuffer( vertexElements,
                                                          c_originalVertices.size(),
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
        Ogre::IndexBufferPacked *indexBuffer = createIndexBuffer(); //Create the actual index buffer
        Ogre::VertexArrayObject *vao = vaoManager->createVertexArrayObject(
                                                                           vertexBuffers, indexBuffer, Ogre::OT_TRIANGLE_LIST );
        
        //Each Vao pushed to the vector refers to an LOD level.
        //Must be in sync with mesh->mLodValues & mesh->mNumLods if you use more than one level
        subMesh->mVao[Ogre::VpNormal].push_back( vao );
        //Use the same geometry for shadow casting.
        subMesh->mVao[Ogre::VpShadow].push_back( vao );
        
        //Set the bounds to get frustum culling and LOD to work correctly.
        jet::BoundingBox3D box = obj->boundingBox();
        mesh->_setBounds( Ogre::Aabb( Ogre::Vector3(box.lowerCorner.x,
                                                    box.lowerCorner.y,
                                                    box.lowerCorner.z),
                                     Ogre::Vector3(box.upperCorner.x,
                                                   box.upperCorner.y,
                                                   box.upperCorner.z) ), false );
        mesh->_setBoundingSphereRadius( box.diagonalLength() );
        
        return std::pair<Ogre::MeshPtr, Ogre::VertexBufferPacked*>( mesh, vertexBuffer );
    }
    //-----------------------------------------------------------------------------------
    void DynamicGeometryGameState::createScene01(void)
    {
        //Create all four types of meshes.
        std::pair<Ogre::MeshPtr, Ogre::VertexBufferPacked*> dynamicMesh;
        dynamicMesh = createDynamicMesh( 0 );
        mDynamicMesh         = dynamicMesh.first;
        mDynamicVertexBuffer = dynamicMesh.second;
        
        //Initialize the scene (items)
        Ogre::SceneManager *sceneManager = mGraphicsSystem->getSceneManager();
        
        Ogre::Item *item = sceneManager->createItem( mDynamicMesh, Ogre::SCENE_DYNAMIC );
        Ogre::SceneNode *sceneNode = sceneManager->getRootSceneNode( Ogre::SCENE_DYNAMIC )->
        createChildSceneNode( Ogre::SCENE_DYNAMIC );
        sceneNode->attachObject( item );
        sceneNode->setPosition( 0, 0, 0 );
        
        Ogre::Light *light = sceneManager->createLight();
        Ogre::SceneNode *lightNode = sceneManager->getRootSceneNode()->createChildSceneNode();
        lightNode->attachObject( light );
        light->setPowerScale( Ogre::Math::PI ); //Since we don't do HDR, counter the PBS' division by PI
        light->setType( Ogre::Light::LT_DIRECTIONAL );
        light->setDirection( Ogre::Vector3( -1, -1, -1 ).normalisedCopy() );
        
        mCameraController = new CameraController( mGraphicsSystem, false );
        
        TutorialGameState::createScene01();
    }
    //-----------------------------------------------------------------------------------
    void DynamicGeometryGameState::destroyScene()
    {
        if( mDynamicVertexBuffer &&
            mDynamicVertexBuffer->getMappingState() != Ogre::MS_UNMAPPED )
        {
            mDynamicVertexBuffer->unmap( Ogre::UO_UNMAP_ALL );
        }
        
        //If we don't do this, the smart pointers will try to
        //delete memory after Ogre has shutdown (and crash)
        mDynamicMesh.reset();
    }
    //-----------------------------------------------------------------------------------
    void DynamicGeometryGameState::update( float timeSinceLast )
    {
        mRotationTime += timeSinceLast;
        mRotationTime = fmod( mRotationTime, Ogre::Math::PI * 2.0f );

        const float cosAlpha = cosf( mRotationTime );
        const float sinAlpha = sinf( mRotationTime );

        float * RESTRICT_ALIAS cubeVertices = reinterpret_cast<float*RESTRICT_ALIAS>(
                    mDynamicVertexBuffer->map( 0, mDynamicVertexBuffer->getNumElements() ) );

        for( size_t i=0; i<c_originalVertices.size(); ++i )
        {
            cubeVertices[0] = c_originalVertices[i].px * cosAlpha - c_originalVertices[i].py * sinAlpha;
            cubeVertices[1] = c_originalVertices[i].px * sinAlpha + c_originalVertices[i].py * cosAlpha;
            cubeVertices[2] = c_originalVertices[i].pz;

            cubeVertices[3] = c_originalVertices[i].nx * cosAlpha - c_originalVertices[i].ny * sinAlpha;
            cubeVertices[4] = c_originalVertices[i].nx * sinAlpha + c_originalVertices[i].ny * cosAlpha;
            cubeVertices[5] = c_originalVertices[i].nz;

            cubeVertices += 6;
        }

        mDynamicVertexBuffer->unmap( Ogre::UO_KEEP_PERSISTENT );
        TutorialGameState::update( timeSinceLast );
    }
}
