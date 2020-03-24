//
//  ConstraintPhysicsSetupState.cpp
//  Sisy
//
//  Created by 杨丰 on 2020/3/24.
//  Copyright © 2020 杨丰. All rights reserved.
//

#include "ConstraintPhysicsSetupState.hpp"
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
#include "imgui.h"

static btScalar val;
static btScalar targetVel = 0;
static btScalar maxImpulse = 10000;
static btHingeAccumulatedAngleConstraint* spDoorHinge = 0;
static btScalar actualHingeVelocity = 0.f;

static btVector3 btAxisA(0, 1, 0);

namespace jet{
MyGameState::MyGameState( const Ogre::String &helpDescription ) :
TutorialGameState( helpDescription )
{
}
//-----------------------------------------------------------------------------------
void MyGameState::setupEmptyDynamicsWorld(){
    m_collisionConfiguration = new btDefaultCollisionConfiguration();
    m_dispatcher = new btCollisionDispatcher(m_collisionConfiguration);
    m_broadphase = new btDbvtBroadphase();
    m_solver = new btSequentialImpulseConstraintSolver();
    m_dynamicsWorld = new btDiscreteDynamicsWorld(m_dispatcher, m_broadphase, m_solver, m_collisionConfiguration);
}

void MyGameState::createPhysicalWorld(){
    setupEmptyDynamicsWorld();
    
    {  // create a door using hinge constraint attached to the world
        btCollisionShape* pDoorShape = new btBoxShape(btVector3(2.0f, 5.0f, 0.2f));
        m_collisionShapes.push_back(pDoorShape);
        btTransform doorTrans;
        doorTrans.setIdentity();
        doorTrans.setOrigin(btVector3(-5.0f, -2.0f, 0.0f));
        
        std::pair<Ogre::SceneNode*, btRigidBody*> pDoorBody =
        createRigidBody(1.0, doorTrans,
                        pDoorShape);
        bulletBody.push_back(pDoorBody.first);
        
        pDoorBody.second->setActivationState(DISABLE_DEACTIVATION);
        const btVector3 btPivotA(10.f + 2.1f, -2.0f, 0.0f);  // right next to the door slightly outside
        spDoorHinge = new btHingeAccumulatedAngleConstraint(*pDoorBody.second, btPivotA, btAxisA);

        m_dynamicsWorld->addConstraint(spDoorHinge);

        spDoorHinge->setDbgDrawSize(btScalar(5.f));
    }
}

void MyGameState::createScene01(void)
{
    createPhysicalWorld();
    
    Ogre::SceneManager *sceneManager = mGraphicsSystem->getSceneManager();
    BulletConverter converter(mGraphicsSystem);
    
    Ogre::HlmsManager *hlmsManager = mGraphicsSystem->getRoot()->getHlmsManager();
    assert( dynamic_cast<Ogre::HlmsPbs*>( hlmsManager->getHlms( Ogre::HLMS_PBS ) ) );
    Ogre::HlmsPbs *hlmsPbs = static_cast<Ogre::HlmsPbs*>( hlmsManager->getHlms(Ogre::HLMS_PBS) );
    {
        ///create a few basic rigid bodies
        btCollisionShape* groundShape = m_collisionShapes[0];
        converter.createCollisionShapeGraphicsObject(groundShape, "door");
        
        Ogre::Item* m_item = sceneManager->createItem("door",
                                                      Ogre::ResourceGroupManager::
                                                      AUTODETECT_RESOURCE_GROUP_NAME,
                                                      Ogre::SCENE_DYNAMIC );
        bulletBody[0]->attachObject(m_item);
        m_item->setVisibilityFlags( 0x000000001 );
        
        Ogre::String datablockName = "Test";
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
    }
    
    Ogre::SceneNode *rootNode = sceneManager->getRootSceneNode();
    
    mDebugNode = sceneManager->getRootSceneNode( Ogre::SCENE_DYNAMIC )->
    createChildSceneNode( Ogre::SCENE_DYNAMIC );
    manual = sceneManager->createManualObject();
    manual->setVisibilityFlags( 0x000000002 );
    mDebugNode->attachObject( manual );
    
    assert( dynamic_cast<Ogre::HlmsUnlit*>( hlmsManager->getHlms( Ogre::HLMS_UNLIT ) ) );
    Ogre::HlmsUnlit *hlmsUnlit = static_cast<Ogre::HlmsUnlit*>( hlmsManager->getHlms(Ogre::HLMS_UNLIT) );
    
    debug = new OgreDebugDrawer(manual, hlmsUnlit);
    debug->setDebugMode(OgreDebugDrawer::DBG_DrawWireframe + OgreDebugDrawer::DBG_DrawConstraintLimits);
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
    val = spDoorHinge->getAccumulatedHingeAngle() * SIMD_DEGS_PER_RAD;
    spDoorHinge->enableAngularMotor(true, targetVel, maxImpulse);
    stepSimulation(timeSinceLast);
    
    btHingeConstraint* hinge = spDoorHinge;

    if (hinge)
    {
        const btRigidBody& bodyA = hinge->getRigidBodyA();
        const btRigidBody& bodyB = hinge->getRigidBodyB();

        btTransform trA = bodyA.getWorldTransform();
        btVector3 angVelA = bodyA.getAngularVelocity();
        btVector3 angVelB = bodyB.getAngularVelocity();

        {
            btVector3 ax1 = trA.getBasis() * hinge->getFrameOffsetA().getBasis().getColumn(2);
            btScalar vel = angVelA.dot(ax1);
            vel -= angVelB.dot(ax1);
            printf("hinge velocity (q) = %f\n", vel);
            actualHingeVelocity = vel;
        }
        btVector3 ortho0, ortho1;
        btPlaneSpace1(btAxisA, ortho0, ortho1);
        {
            btScalar vel2 = angVelA.dot(ortho0);
            vel2 -= angVelB.dot(ortho0);
            printf("hinge orthogonal1 velocity (q) = %f\n", vel2);
        }
        {
            btScalar vel0 = angVelA.dot(ortho1);
            vel0 -= angVelB.dot(ortho1);
            printf("hinge orthogonal0 velocity (q) = %f\n", vel0);
        }
    }
    
    manual->clear();
    m_dynamicsWorld->debugDrawWorld();
    
    TutorialGameState::update( timeSinceLast );
}
//-----------------------------------------------------------------------------------
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
//-----------------------------------------------------------------------------------
void MyGameState::destroyScene(void){
    ;
}
//-----------------------------------------------------------------------------------
void MyGameState::createGUI(void){
    ImGui::Begin("Velocity");

    ImGui::SliderFloat("target vel", &targetVel, -4.0f, 4.0f);
    ImGui::SliderFloat("max impulse", &maxImpulse, 0.0f, 1000.0f);
    ImGui::SliderFloat("actual vel", &actualHingeVelocity, -4.0f, 4.0f);
    ImGui::Text("angle = %f", val);

    ImGui::End();
}
//-----------------------------------------------------------------------------------
}
