
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

#include "OgreHlmsManager.h"
#include "OgreHlms.h"
#include "OgreHlmsCompute.h"
#include "OgreGpuProgramManager.h"
#include "BulletGUIHelper.h"

using namespace Demo;

namespace Demo{
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
    mBulletHelper = new BulletGuiHelper(graphicsSystem);
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
    if( mCameraController )
        mCameraController->mouseMoved( arg );
    
    GameState::mouseMoved( arg );
}
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
btBoxShape* TutorialGameState::createBoxShape(const btVector3& halfExtents)
{
    btBoxShape* box = new btBoxShape(halfExtents);
    return box;
}
//---------------------------------------------------------------------------------
btRigidBody* TutorialGameState::createRigidBody(float mass, const btTransform& startTransform,
                                                btCollisionShape* shape,
                                                const btVector4& color)
{
    btAssert((!shape || shape->getShapeType() != INVALID_SHAPE_PROXYTYPE));
    
    //rigidbody is dynamic if and only if mass is non zero, otherwise static
    bool isDynamic = (mass != 0.f);
    
    btVector3 localInertia(0, 0, 0);
    if (isDynamic)
        shape->calculateLocalInertia(mass, localInertia);
    
    //using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
    
#define USE_MOTIONSTATE 1
#ifdef USE_MOTIONSTATE
    btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
    
    btRigidBody::btRigidBodyConstructionInfo cInfo(mass, myMotionState, shape, localInertia);
    
    btRigidBody* body = new btRigidBody(cInfo);
    //body->setContactProcessingThreshold(m_defaultContactProcessingThreshold);
    
#else
    btRigidBody* body = new btRigidBody(mass, 0, shape, localInertia);
    body->setWorldTransform(startTransform);
#endif  //
    
    body->setUserIndex(-1);
    m_dynamicsWorld->addRigidBody(body);
    return body;
}
//---------------------------------------------------------------------------------
void TutorialGameState::stepSimulation(float deltaTime)
{
    if (m_dynamicsWorld)
    {
        m_dynamicsWorld->stepSimulation(deltaTime);
    }
}
}
