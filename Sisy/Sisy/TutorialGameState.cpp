
#include "TutorialGameState.h"
#include "CameraController.h"
#include "GraphicsSystem.h"

#include "OgreSceneManager.h"

#include "OgreOverlayManager.h"
#include "OgreOverlay.h"
#include "OgreOverlayContainer.h"
#include "OgreTextAreaOverlayElement.h"

#include "OgreRoot.h"
#include "OgreFrameStats.h"
#include "OgreCamera.h"
#include "OgreWindow.h"
#include "OgreHlmsManager.h"
#include "OgreHlms.h"
#include "OgreHlmsCompute.h"
#include "OgreGpuProgramManager.h"
#include "BulletConverter.hpp"

using namespace jet;

namespace jet{
TutorialGameState::TutorialGameState( const Ogre::String &helpDescription ) :
mGraphicsSystem( 0 ),
mCameraController( 0 ),
mHelpDescription( helpDescription ),
mDisplayHelpMode( 1 ),
mNumDisplayHelpModes( 2 ),
mDebugText( 0 )
{
}
//-----------------------------------------------------------------------------------
TutorialGameState::~TutorialGameState()
{
    delete mCameraController;
    mCameraController = 0;
}
//-----------------------------------------------------------------------------------
void TutorialGameState::_notifyGraphicsSystem( GraphicsSystem *graphicsSystem )
{
    mGraphicsSystem = graphicsSystem;
}
//-----------------------------------------------------------------------------------
void TutorialGameState::createScene01(void)
{
    createDebugTextOverlay();
}
//-----------------------------------------------------------------------------------
void TutorialGameState::createDebugTextOverlay(void)
{
    Ogre::v1::OverlayManager &overlayManager = Ogre::v1::OverlayManager::getSingleton();
    Ogre::v1::Overlay *overlay = overlayManager.create( "DebugText" );
    
    Ogre::v1::OverlayContainer *panel = static_cast<Ogre::v1::OverlayContainer*>(
                                                                                 overlayManager.createOverlayElement("Panel", "DebugPanel"));
    mDebugText = static_cast<Ogre::v1::TextAreaOverlayElement*>(
                                                                overlayManager.createOverlayElement( "TextArea", "DebugText" ) );
    mDebugText->setFontName( "DebugFont" );
    mDebugText->setCharHeight( 0.025f );
    
    mDebugTextShadow= static_cast<Ogre::v1::TextAreaOverlayElement*>(
                                                                     overlayManager.createOverlayElement( "TextArea", "0DebugTextShadow" ) );
    mDebugTextShadow->setFontName( "DebugFont" );
    mDebugTextShadow->setCharHeight( 0.025f );
    mDebugTextShadow->setColour( Ogre::ColourValue::Black );
    mDebugTextShadow->setPosition( 0.002f, 0.002f );
    
    panel->addChild( mDebugTextShadow );
    panel->addChild( mDebugText );
    overlay->add2D( panel );
    overlay->show();
}
//-----------------------------------------------------------------------------------
void TutorialGameState::generateDebugText( float timeSinceLast, Ogre::String &outText )
{
    if( mDisplayHelpMode == 0 )
    {
        outText = mHelpDescription;
        outText += "\n\nPress F1 to toggle help";
        outText += "\n\nProtip: Ctrl+F1 will reload PBS shaders (for real time template editing).\n"
        "Ctrl+F2 reloads Unlit shaders.\n"
        "Ctrl+F3 reloads Compute shaders.\n"
        "Note: If the modified templates produce invalid shader code, "
        "crashes or exceptions can happen.\n";
        return;
    }
    
    const Ogre::FrameStats *frameStats = mGraphicsSystem->getRoot()->getFrameStats();
    
    Ogre::String finalText;
    finalText.reserve( 128 );
    finalText  = "Frame time:\t";
    finalText += Ogre::StringConverter::toString( timeSinceLast * 1000.0f );
    finalText += " ms\n";
    finalText += "Frame FPS:\t";
    finalText += Ogre::StringConverter::toString( 1.0f / timeSinceLast );
    finalText += "\nAvg time:\t";
    finalText += Ogre::StringConverter::toString( frameStats->getAvgTime() );
    finalText += " ms\n";
    finalText += "Avg FPS:\t";
    finalText += Ogre::StringConverter::toString( 1000.0f / frameStats->getAvgTime() );
    finalText += "\n\nPress F1 to toggle help";
    
    outText.swap( finalText );
    
    mDebugText->setCaption( finalText );
    mDebugTextShadow->setCaption( finalText );
}
//-----------------------------------------------------------------------------------
void TutorialGameState::update( float timeSinceLast )
{
    if( mDisplayHelpMode != 0 )
    {
        //Show FPS
        Ogre::String finalText;
        generateDebugText( timeSinceLast, finalText );
        mDebugText->setCaption( finalText );
        mDebugTextShadow->setCaption( finalText );
    }
    
    if( mCameraController )
        mCameraController->update( timeSinceLast );
}
//-----------------------------------------------------------------------------------
void TutorialGameState::keyPressed( const SDL_KeyboardEvent &arg )
{
    bool handledEvent = false;
    
    if( mCameraController )
        handledEvent = mCameraController->keyPressed( arg );
    
    if( !handledEvent )
        GameState::keyPressed( arg );
}
//-----------------------------------------------------------------------------------
void TutorialGameState::keyReleased( const SDL_KeyboardEvent &arg )
{
    if( arg.keysym.scancode == SDL_SCANCODE_F1 && (arg.keysym.mod & ~(KMOD_NUM|KMOD_CAPS)) == 0 )
    {
        mDisplayHelpMode = (mDisplayHelpMode + 1) % mNumDisplayHelpModes;
        
        Ogre::String finalText;
        generateDebugText( 0, finalText );
        mDebugText->setCaption( finalText );
        mDebugTextShadow->setCaption( finalText );
    }
    else if( arg.keysym.scancode == SDL_SCANCODE_F1 && (arg.keysym.mod & (KMOD_LCTRL|KMOD_RCTRL)) )
    {
        //Hot reload of PBS shaders. We need to clear the microcode cache
        //to prevent using old compiled versions.
        Ogre::Root *root = mGraphicsSystem->getRoot();
        Ogre::HlmsManager *hlmsManager = root->getHlmsManager();
        
        Ogre::Hlms *hlms = hlmsManager->getHlms( Ogre::HLMS_PBS );
        Ogre::GpuProgramManager::getSingleton().clearMicrocodeCache();
        hlms->reloadFrom( hlms->getDataFolder() );
    }
    else if( arg.keysym.scancode == SDL_SCANCODE_F2  && (arg.keysym.mod & (KMOD_LCTRL|KMOD_RCTRL)) )
    {
        //Hot reload of Unlit shaders.
        Ogre::Root *root = mGraphicsSystem->getRoot();
        Ogre::HlmsManager *hlmsManager = root->getHlmsManager();
        
        Ogre::Hlms *hlms = hlmsManager->getHlms( Ogre::HLMS_UNLIT );
        Ogre::GpuProgramManager::getSingleton().clearMicrocodeCache();
        hlms->reloadFrom( hlms->getDataFolder() );
    }
    else if( arg.keysym.scancode == SDL_SCANCODE_F3 && (arg.keysym.mod & (KMOD_LCTRL|KMOD_RCTRL)) )
    {
        //Hot reload of Compute shaders.
        Ogre::Root *root = mGraphicsSystem->getRoot();
        Ogre::HlmsManager *hlmsManager = root->getHlmsManager();
        
        Ogre::Hlms *hlms = hlmsManager->getComputeHlms();
        Ogre::GpuProgramManager::getSingleton().clearMicrocodeCache();
        hlms->reloadFrom( hlms->getDataFolder() );
    }
    else if(arg.keysym.scancode == SDL_SCANCODE_ESCAPE)
    {
        mGraphicsSystem->setQuit();
    }
    else
    {
        bool handledEvent = false;
        
        if( mCameraController )
            handledEvent = mCameraController->keyReleased( arg );
        
        if( !handledEvent )
            GameState::keyReleased( arg );
    }
}
//-----------------------------------------------------------------------------------
void TutorialGameState::mouseMoved( const SDL_Event &arg )
{
    if( mCameraController ){
        mCameraController->mouseMoved( arg );
        
        float width  = static_cast<float>( mGraphicsSystem->getRenderWindow()->getWidth() );
        float height = static_cast<float>( mGraphicsSystem->getRenderWindow()->getHeight() );
        
        Ogre::Ray ray = mGraphicsSystem->getCamera()->getCameraToViewportRay(arg.motion.x/width*2,
                                                                             arg.motion.y/height*2);
        Ogre::Vector3 pos = ray.getPoint(10000);
        movePickedBody(btVector3(ray.getOrigin().x,
                                 ray.getOrigin().y,
                                 ray.getOrigin().z),
                       btVector3(pos.x,
                                 pos.y,
                                 pos.z));
    }
    
    GameState::mouseMoved( arg );
}
void TutorialGameState::mousePressed( const SDL_MouseButtonEvent &arg, Ogre::uint8 id ){
    
    float width  = static_cast<float>( mGraphicsSystem->getRenderWindow()->getWidth() );
    float height = static_cast<float>( mGraphicsSystem->getRenderWindow()->getHeight() );
    Ogre::Ray ray = mGraphicsSystem->getCamera()->getCameraToViewportRay(arg.x/width*2,
                                                                         arg.y/height*2);
    Ogre::Vector3 pos = ray.getPoint(10000);
    pickBody(btVector3(ray.getOrigin().x,
                       ray.getOrigin().y,
                       ray.getOrigin().z),
             btVector3(pos.x,
                       pos.y,
                       pos.z));
}

void TutorialGameState::mouseReleased( const SDL_MouseButtonEvent &arg, Ogre::uint8 id ){
    removePickingConstraint();
}
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
void TutorialGameState::createEmptyDynamicsWorld()
{
    ///collision configuration contains default setup for memory, collision setup
    m_collisionConfiguration = new btDefaultCollisionConfiguration();
    //m_collisionConfiguration->setConvexConvexMultipointIterations();
    
    ///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
    m_dispatcher = new btCollisionDispatcher(m_collisionConfiguration);
    
    m_broadphase = new btDbvtBroadphase();
    
    ///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
    btSequentialImpulseConstraintSolver* sol = new btSequentialImpulseConstraintSolver;
    m_solver = sol;
    
    m_dynamicsWorld = new btDiscreteDynamicsWorld(m_dispatcher, m_broadphase,
                                                  m_solver, m_collisionConfiguration);
    
    m_dynamicsWorld->setGravity(btVector3(0, -10, 0));
}
//---------------------------------------------------------------------------------
void TutorialGameState::stepSimulation(float deltaTime)
{
    if (m_dynamicsWorld)
    {
        m_dynamicsWorld->stepSimulation(deltaTime);
    }
}
//---------------------------------------------------------------------------------
bool TutorialGameState::pickBody(const btVector3& rayFromWorld,
                                 const btVector3& rayToWorld){
    if (m_dynamicsWorld == 0)
        return false;
    
    btCollisionWorld::ClosestRayResultCallback rayCallback(rayFromWorld, rayToWorld);
    
    rayCallback.m_flags |= btTriangleRaycastCallback::kF_UseGjkConvexCastRaytest;
    m_dynamicsWorld->rayTest(rayFromWorld, rayToWorld, rayCallback);
    if (rayCallback.hasHit())
    {
        btVector3 pickPos = rayCallback.m_hitPointWorld;
        btRigidBody* body = (btRigidBody*)btRigidBody::upcast(rayCallback.m_collisionObject);
        if (body)
        {
            //other exclusions?
            if (!(body->isStaticObject() || body->isKinematicObject()))
            {
                m_pickedBody = body;
                m_savedState = m_pickedBody->getActivationState();
                m_pickedBody->setActivationState(DISABLE_DEACTIVATION);
                //printf("pickPos=%f,%f,%f\n",pickPos.getX(),pickPos.getY(),pickPos.getZ());
                btVector3 localPivot = body->getCenterOfMassTransform().inverse() * pickPos;
                btPoint2PointConstraint* p2p = new btPoint2PointConstraint(*body, localPivot);
                m_dynamicsWorld->addConstraint(p2p, true);
                m_pickedConstraint = p2p;
                btScalar mousePickClamping = 30.f;
                p2p->m_setting.m_impulseClamp = mousePickClamping;
                //very weak constraint for picking
                p2p->m_setting.m_tau = 0.001f;
            }
        }
        
        //                    pickObject(pickPos, rayCallback.m_collisionObject);
        m_oldPickingPos = rayToWorld;
        m_hitPos = pickPos;
        m_oldPickingDist = (pickPos - rayFromWorld).length();
        //                    printf("hit !\n");
        //add p2p
    }
    return false;
}
//---------------------------------------------------------------------------------
bool TutorialGameState::movePickedBody(const btVector3& rayFromWorld,
                                       const btVector3& rayToWorld)
{
    if (m_pickedBody && m_pickedConstraint)
    {
        btPoint2PointConstraint* pickCon = static_cast<btPoint2PointConstraint*>(m_pickedConstraint);
        if (pickCon)
        {
            //keep it at the same picking distance
            
            btVector3 newPivotB;
            
            btVector3 dir = rayToWorld - rayFromWorld;
            dir.normalize();
            dir *= m_oldPickingDist;
            
            newPivotB = rayFromWorld + dir;
            pickCon->setPivotB(newPivotB);
            return true;
        }
    }
    return false;
}
//---------------------------------------------------------------------------------
void TutorialGameState::removePickingConstraint()
{
    if (m_pickedConstraint)
    {
        m_pickedBody->forceActivationState(m_savedState);
        m_pickedBody->activate();
        m_dynamicsWorld->removeConstraint(m_pickedConstraint);
        delete m_pickedConstraint;
        m_pickedConstraint = 0;
        m_pickedBody = 0;
    }
}
//---------------------------------------------------------------------------------
}
