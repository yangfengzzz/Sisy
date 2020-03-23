//
//  RollingFrictionDemoState.cpp
//  Sisy
//
//  Created by 杨丰 on 2020/3/22.
//  Copyright © 2020 杨丰. All rights reserved.
//

#include "RollingFrictionDemoState.hpp"
#include "GraphicsSystem.h"
#include "CameraController.h"
#include "OgreSceneManager.h"
#include "OgreCamera.h"
#include "OgreHlmsUnlitDatablock.h"
#include "OgreHlmsPbsDatablock.h"
#include "OgreHlmsSamplerblock.h"
#include "OgreMath.h"
#include "OgreRoot.h"
#include "OgreHlmsManager.h"
#include "OgreHlmsPbs.h"
#include "OgreHlmsUnlit.h"
#include "OgreItem.h"
#include "OgreTextureGpuManager.h"
#include "OgreTextAreaOverlayElement.h"

#include "BulletConverter.hpp"

///create 125 (5x5x5) dynamic object
#define ARRAY_SIZE_Y 5
#define ARRAY_SIZE_X 5
#define ARRAY_SIZE_Z 5

//maximum number of objects (and allow user to shoot additional boxes)
#define MAX_PROXIES (ARRAY_SIZE_X * ARRAY_SIZE_Y * ARRAY_SIZE_Z + 1024)

///scaling of the objects (0.1 = 20 centimeter boxes )
#define SCALING 1.
#define START_POS_X -5
#define START_POS_Y -5
#define START_POS_Z -3

namespace jet{
MyGameState::MyGameState( const Ogre::String &helpDescription ) :
TutorialGameState( helpDescription )
{
}
//-----------------------------------------------------------------------------------
void MyGameState::createPhysicalWorld(){
    ///collision configuration contains default setup for memory, collision setup
    m_collisionConfiguration = new btDefaultCollisionConfiguration();
    //m_collisionConfiguration->setConvexConvexMultipointIterations();
    
    ///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
    m_dispatcher = new btCollisionDispatcher(m_collisionConfiguration);
    
    m_broadphase = new btDbvtBroadphase();
    
    ///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
    btSequentialImpulseConstraintSolver* sol = new btSequentialImpulseConstraintSolver;
    m_solver = sol;
    
    m_dynamicsWorld = new btDiscreteDynamicsWorld(m_dispatcher, m_broadphase, m_solver, m_collisionConfiguration);
    //    m_dynamicsWorld->getSolverInfo().m_singleAxisRollingFrictionThreshold = 0.f;//faster but lower quality
    m_dynamicsWorld->setGravity(btVector3(0, 0, -10));
    
    {
        ///create a few basic rigid bodies
        btCollisionShape* groundShape = new btBoxShape(btVector3(btScalar(10.), btScalar(5.), btScalar(25.)));
        
        m_collisionShapes.push_back(groundShape);
        
        btTransform groundTransform;
        groundTransform.setIdentity();
        groundTransform.setOrigin(btVector3(0, 0, -28));
        groundTransform.setRotation(btQuaternion(btVector3(0, 1, 0), SIMD_PI * 0.03));
        //We can also use DemoApplication::localCreateRigidBody, but for clarity it is provided here:
        btScalar mass(0.);
        std::pair<Ogre::SceneNode*, btRigidBody*> plane =
        createRigidBody(mass, groundTransform,
                        groundShape);
        bulletBody.push_back(plane.first);
        plane.second->setFriction(.5);
    }
    
    {
        ///create a few basic rigid bodies
        btCollisionShape* groundShape = new btBoxShape(btVector3(btScalar(100.), btScalar(100.), btScalar(50.)));
        
        m_collisionShapes.push_back(groundShape);
        
        btTransform groundTransform;
        groundTransform.setIdentity();
        groundTransform.setOrigin(btVector3(0, 0, -54));
        //We can also use DemoApplication::localCreateRigidBody, but for clarity it is provided here:
        btScalar mass(0.);
        std::pair<Ogre::SceneNode*, btRigidBody*> plane =
        createRigidBody(mass, groundTransform,
                        groundShape);
        bulletBody.push_back(plane.first);
        plane.second->setFriction(.1);
    }
    
    {
        //create a few dynamic rigidbodies
        // Re-using the same collision is better for memory usage and performance
#define NUM_SHAPES 10
        btCollisionShape* colShapes[NUM_SHAPES] = {
            new btSphereShape(btScalar(0.5)),
            new btCapsuleShape(0.25, 0.5),
            new btCapsuleShapeX(0.25, 0.5),
            new btCapsuleShapeZ(0.25, 0.5),
            new btConeShape(0.25, 0.5),
            new btConeShapeX(0.25, 0.5),
            new btConeShapeZ(0.25, 0.5),
            new btCylinderShape(btVector3(0.25, 0.5, 0.25)),
            new btCylinderShapeX(btVector3(0.5, 0.25, 0.25)),
            new btCylinderShapeZ(btVector3(0.25, 0.25, 0.5)),
        };
        for (int i = 0; i < NUM_SHAPES; i++)
            m_collisionShapes.push_back(colShapes[i]);
        
        /// Create Dynamic Objects
        btTransform startTransform;
        startTransform.setIdentity();
        
        btScalar mass(1.f);
        
        //rigidbody is dynamic if and only if mass is non zero, otherwise static
        
        float start_x = START_POS_X - ARRAY_SIZE_X / 2;
        float start_y = START_POS_Y;
        float start_z = START_POS_Z - ARRAY_SIZE_Z / 2;
        
        {
            int shapeIndex = 0;
            for (int k = 0; k < ARRAY_SIZE_Y; k++)
            {
                for (int i = 0; i < ARRAY_SIZE_X; i++)
                {
                    for (int j = 0; j < ARRAY_SIZE_Z; j++)
                    {
                        startTransform.setOrigin(SCALING * btVector3(
                                                                     btScalar(2.0 * i + start_x),
                                                                     btScalar(2.0 * j + start_z),
                                                                     btScalar(20 + 2.0 * k + start_y)));
                        
                        btCollisionShape* colShape = colShapes[shapeIndex % NUM_SHAPES];
                        
                        std::pair<Ogre::SceneNode*, btRigidBody*> plane =
                        createRigidBody(mass, startTransform,
                                        colShape);
                        bulletBody.push_back(plane.first);
                        plane.second->setFriction(1.f);
                        plane.second->setRollingFriction(.1);
                        plane.second->setSpinningFriction(0.1);
                        plane.second->setAnisotropicFriction(colShape->getAnisotropicRollingFrictionDirection(),
                                                             btCollisionObject::CF_ANISOTROPIC_ROLLING_FRICTION);
                        shapeIndex++;
                    }
                }
            }
        }
    }
}

void MyGameState::createScene01(void)
{
    createPhysicalWorld();
    
    Ogre::SceneManager *sceneManager = mGraphicsSystem->getSceneManager();
    BulletConverter converter(mGraphicsSystem);
    {
        ///create a few basic rigid bodies
        btCollisionShape* groundShape = m_collisionShapes[0];
        converter.createCollisionShapeGraphicsObject(groundShape, "ground1");
        
        Ogre::Item* m_item = sceneManager->createItem("ground1",
                                                      Ogre::ResourceGroupManager::
                                                      AUTODETECT_RESOURCE_GROUP_NAME,
                                                      Ogre::SCENE_DYNAMIC );
        bulletBody[0]->attachObject(m_item);
        
        m_item->setDatablock( "Marble" );
        //Change the addressing mode of the roughness map to wrap via code.
        //Detail maps default to wrap, but the rest to clamp.
        assert( dynamic_cast<Ogre::HlmsPbsDatablock*>( m_item->getSubItem(0)->getDatablock() ) );
        Ogre::HlmsPbsDatablock *datablock
        = static_cast<Ogre::HlmsPbsDatablock*>(
                                               m_item->getSubItem(0)->getDatablock() );
        //Make a hard copy of the sampler block
        Ogre::HlmsSamplerblock samplerblock( *datablock->getSamplerblock( Ogre::PBSM_ROUGHNESS ) );
        samplerblock.mU = Ogre::TAM_WRAP;
        samplerblock.mV = Ogre::TAM_WRAP;
        samplerblock.mW = Ogre::TAM_WRAP;
        //Set the new samplerblock. The Hlms system will
        //automatically create the API block if necessary
        datablock->setSamplerblock( Ogre::PBSM_ROUGHNESS, samplerblock );
    }
    
    {
        ///create a few basic rigid bodies
        btCollisionShape* groundShape = m_collisionShapes[1];
        converter.createCollisionShapeGraphicsObject(groundShape, "ground2");
        
        Ogre::Item* m_item = sceneManager->createItem("ground2",
                                                      Ogre::ResourceGroupManager::
                                                      AUTODETECT_RESOURCE_GROUP_NAME,
                                                      Ogre::SCENE_DYNAMIC );
        bulletBody[1]->attachObject(m_item);
        
        m_item->setDatablock( "Marble" );
        //Change the addressing mode of the roughness map to wrap via code.
        //Detail maps default to wrap, but the rest to clamp.
        assert( dynamic_cast<Ogre::HlmsPbsDatablock*>( m_item->getSubItem(0)->getDatablock() ) );
        Ogre::HlmsPbsDatablock *datablock
        = static_cast<Ogre::HlmsPbsDatablock*>(
                                               m_item->getSubItem(0)->getDatablock() );
        //Make a hard copy of the sampler block
        Ogre::HlmsSamplerblock samplerblock( *datablock->getSamplerblock( Ogre::PBSM_ROUGHNESS ) );
        samplerblock.mU = Ogre::TAM_WRAP;
        samplerblock.mV = Ogre::TAM_WRAP;
        samplerblock.mW = Ogre::TAM_WRAP;
        //Set the new samplerblock. The Hlms system will
        //automatically create the API block if necessary
        datablock->setSamplerblock( Ogre::PBSM_ROUGHNESS, samplerblock );
    }
    
    {
        for (int i = 2; i < NUM_SHAPES+2; i++){
            converter.createCollisionShapeGraphicsObject(m_collisionShapes[i],
                                                         "dynamics_"+std::to_string(i));
        }
        
        
        {
            Ogre::HlmsManager *hlmsManager = mGraphicsSystem->getRoot()->getHlmsManager();
            assert( dynamic_cast<Ogre::HlmsPbs*>( hlmsManager->getHlms( Ogre::HLMS_PBS ) ) );
            Ogre::HlmsPbs *hlmsPbs = static_cast<Ogre::HlmsPbs*>( hlmsManager->getHlms(Ogre::HLMS_PBS) );
            size_t idx = 0;
            for (int k = 0; k < ARRAY_SIZE_Y; k++)
            {
                for (int i = 0; i < ARRAY_SIZE_X; i++)
                {
                    for (int j = 0; j < ARRAY_SIZE_Z; j++)
                    {
                        Ogre::Item* m_item = sceneManager->createItem("dynamics_"+std::to_string(idx % NUM_SHAPES+2),
                                                                      Ogre::ResourceGroupManager::
                                                                      AUTODETECT_RESOURCE_GROUP_NAME,
                                                                      Ogre::SCENE_DYNAMIC );
                        bulletBody[idx+2]->attachObject(m_item);
                        m_item->setVisibilityFlags( 0x000000001 );
                        
                        Ogre::String datablockName = "Test" + Ogre::StringConverter::toString( idx);
                        Ogre::HlmsPbsDatablock *datablock
                        = static_cast<Ogre::HlmsPbsDatablock*>(
                                                               hlmsPbs->createDatablock(datablockName,
                                                                                        datablockName,
                                                                                        Ogre::HlmsMacroblock(),
                                                                                        Ogre::HlmsBlendblock(),
                                                                                        Ogre::HlmsParamVec() ) );
                        
                        datablock->setDiffuse( Ogre::Vector3( 0.0f, 1.0f, 0.0f ) );
                        datablock->setRoughness(0.02f);
                        datablock->setFresnel( Ogre::Vector3( 1 ), false );
                        datablock->setTransparency( 0.9 );
                        m_item->setDatablock( datablock );
                        idx++;
                    }
                }
            }
        }
    }
    
    Ogre::SceneNode *rootNode = sceneManager->getRootSceneNode();
    
    mDebugNode = sceneManager->getRootSceneNode( Ogre::SCENE_DYNAMIC )->
    createChildSceneNode( Ogre::SCENE_DYNAMIC );
    manual = sceneManager->createManualObject();
    manual->setVisibilityFlags( 0x000000002 );
    mDebugNode->attachObject( manual );
    
    Ogre::HlmsManager *hlmsManager = mGraphicsSystem->getRoot()->getHlmsManager();
    assert( dynamic_cast<Ogre::HlmsUnlit*>( hlmsManager->getHlms( Ogre::HLMS_UNLIT ) ) );
    Ogre::HlmsUnlit *hlmsUnlit = static_cast<Ogre::HlmsUnlit*>( hlmsManager->getHlms(Ogre::HLMS_UNLIT) );
    Ogre::String datablockName = "debugDatalock";
    Ogre::HlmsUnlitDatablock *datablock
    = static_cast<Ogre::HlmsUnlitDatablock*>(
                                           hlmsUnlit->createDatablock(datablockName,
                                                                      datablockName,
                                                                      Ogre::HlmsMacroblock(),
                                                                      Ogre::HlmsBlendblock(),
                                                                      Ogre::HlmsParamVec() ) );
    datablock->setUseColour(true);
    
    debug = new OgreDebugDrawer(manual, "debugDatalock");
    m_dynamicsWorld->setDebugDrawer(debug);
    
    Ogre::Light *light = sceneManager->createLight();
    Ogre::SceneNode *lightNode = rootNode->createChildSceneNode();
    lightNode->attachObject( light );
    light->setPowerScale( 1.0f );
    light->setType( Ogre::Light::LT_DIRECTIONAL );
    light->setDirection( Ogre::Vector3( -1, -1, -1 ).normalisedCopy() );
    
    mLightNodes[0] = lightNode;
    
    sceneManager->setAmbientLight( Ogre::ColourValue( 0.3f, 0.5f, 0.7f ) * 0.1f * 0.75f,
                                  Ogre::ColourValue( 0.6f, 0.45f, 0.3f ) * 0.065f * 0.75f,
                                  -light->getDirection() + Ogre::Vector3::UNIT_Y * 0.2f );
    
    light = sceneManager->createLight();
    lightNode = rootNode->createChildSceneNode();
    lightNode->attachObject( light );
    light->setDiffuseColour( 0.8f, 0.4f, 0.2f ); //Warm
    light->setSpecularColour( 0.8f, 0.4f, 0.2f );
    light->setPowerScale( Ogre::Math::PI );
    light->setType( Ogre::Light::LT_SPOTLIGHT );
    lightNode->setPosition( -10.0f, 10.0f, 10.0f );
    light->setDirection( Ogre::Vector3( 1, -1, -1 ).normalisedCopy() );
    light->setAttenuationBasedOnRadius( 10.0f, 0.01f );
    
    mLightNodes[1] = lightNode;
    
    light = sceneManager->createLight();
    lightNode = rootNode->createChildSceneNode();
    lightNode->attachObject( light );
    light->setDiffuseColour( 0.2f, 0.4f, 0.8f ); //Cold
    light->setSpecularColour( 0.2f, 0.4f, 0.8f );
    light->setPowerScale( Ogre::Math::PI );
    light->setType( Ogre::Light::LT_SPOTLIGHT );
    lightNode->setPosition( 10.0f, 10.0f, -10.0f );
    light->setDirection( Ogre::Vector3( -1, -1, 1 ).normalisedCopy() );
    light->setAttenuationBasedOnRadius( 10.0f, 0.01f );
    
    mLightNodes[2] = lightNode;
    
    mCameraController = new CameraController( mGraphicsSystem, false );
    
    TutorialGameState::createScene01();
}
//-----------------------------------------------------------------------------------
void MyGameState::update( float timeSinceLast )
{
    stepSimulation(timeSinceLast);
    
    manual->clear();
    m_dynamicsWorld->debugDrawWorld();
    
    TutorialGameState::update( timeSinceLast );
}

void MyGameState::generateDebugText( float timeSinceLast, Ogre::String &outText )
{
    Ogre::uint32 visibilityMask = mGraphicsSystem->getSceneManager()->getVisibilityMask();
    
    TutorialGameState::generateDebugText( timeSinceLast, outText );
    outText += "\nPress F2 to show/hide animated objects. ";
    outText += (visibilityMask & 0x000000001) ? "[On]" : "[Off]";
    outText += "\nPress F3 to show/hide Decal's debug visualization. ";
    outText += (visibilityMask & 0x000000002) ? "[On]" : "[Off]";
}

//-----------------------------------------------------------------------------------
void MyGameState::keyReleased( const SDL_KeyboardEvent &arg )
{
    if( (arg.keysym.mod & ~(KMOD_NUM|KMOD_CAPS)) != 0 )
    {
        TutorialGameState::keyReleased( arg );
        return;
    }
    
    if( arg.keysym.sym == SDLK_F2 )
    {
        Ogre::uint32 visibilityMask = mGraphicsSystem->getSceneManager()->getVisibilityMask();
        bool showMovingObjects = (visibilityMask & 0x000000001);
        showMovingObjects = !showMovingObjects;
        visibilityMask &= ~0x000000001;
        visibilityMask |= (Ogre::uint32)showMovingObjects;
        mGraphicsSystem->getSceneManager()->setVisibilityMask( visibilityMask );
    }
    else if( arg.keysym.sym == SDLK_F3 )
    {
        Ogre::uint32 visibilityMask = mGraphicsSystem->getSceneManager()->getVisibilityMask();
        bool showMovingObjects = (visibilityMask & 0x000000002);
        showMovingObjects = !showMovingObjects;
        visibilityMask &= ~0x000000002;
        visibilityMask |= (Ogre::uint32)showMovingObjects << 1;
        mGraphicsSystem->getSceneManager()->setVisibilityMask( visibilityMask );
    }
    else
    {
        TutorialGameState::keyReleased( arg );
    }
}

void MyGameState::destroyScene(void){
    //cleanup in the reverse order of creation/initialization

    //remove the rigidbodies from the dynamics world and delete them
    int i;
    for (i = m_dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
    {
        btCollisionObject* obj = m_dynamicsWorld->getCollisionObjectArray()[i];
        btRigidBody* body = btRigidBody::upcast(obj);
        if (body && body->getMotionState())
        {
            delete body->getMotionState();
        }
        m_dynamicsWorld->removeCollisionObject(obj);
        delete obj;
    }

    //delete collision shapes
    for (int j = 0; j < m_collisionShapes.size(); j++)
    {
        btCollisionShape* shape = m_collisionShapes[j];
        delete shape;
    }
    m_collisionShapes.clear();

    delete m_dynamicsWorld;

    delete m_solver;

    delete m_broadphase;

    delete m_dispatcher;

    delete m_collisionConfiguration;
}
}
