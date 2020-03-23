
#ifndef _Demo_TutorialGameState_H_
#define _Demo_TutorialGameState_H_

#include "OgrePrerequisites.h"
#include "GameState.h"
#include "btBulletDynamicsCommon.h"
#include "btRaycastCallback.h"

namespace Ogre{
namespace v1
{
    class TextAreaOverlayElement;
}
}

namespace jet{
class GraphicsSystem;
class CameraController;
class BulletConverter;
/// Base game state for the tutorials. All it does is show a little text on screen :)
class TutorialGameState : public GameState
{
protected:
    GraphicsSystem      *mGraphicsSystem;
    
    /// Optional, for controlling the camera with WASD and the mouse
    CameraController    *mCameraController;
    
    Ogre::String        mHelpDescription;
    Ogre::uint16        mDisplayHelpMode;
    Ogre::uint16        mNumDisplayHelpModes;
    
    Ogre::v1::TextAreaOverlayElement *mDebugText;
    Ogre::v1::TextAreaOverlayElement *mDebugTextShadow;
    
    virtual void createDebugTextOverlay(void);
    virtual void generateDebugText( float timeSinceLast, Ogre::String &outText );
    
    btAlignedObjectArray<btCollisionShape*> m_collisionShapes;
    btBroadphaseInterface* m_broadphase;
    btCollisionDispatcher* m_dispatcher;
    btConstraintSolver* m_solver;
    btDefaultCollisionConfiguration* m_collisionConfiguration;
    btDiscreteDynamicsWorld* m_dynamicsWorld;

    //data for picking objects
    class btRigidBody* m_pickedBody;
    class btTypedConstraint* m_pickedConstraint;
    int m_savedState;
    btVector3 m_oldPickingPos;
    btVector3 m_hitPos;
    btScalar m_oldPickingDist;
    
    struct RigidActor{
        Ogre::Item* item;
        Ogre::SceneNode* node;
        btRigidBody* body;
    };
    
public:
    TutorialGameState( const Ogre::String &helpDescription );
    virtual ~TutorialGameState();
    
    void _notifyGraphicsSystem( GraphicsSystem *graphicsSystem );
    
    virtual void createScene01(void);
    
    virtual void update( float timeSinceLast );
    
    virtual void keyPressed( const SDL_KeyboardEvent &arg );
    virtual void keyReleased( const SDL_KeyboardEvent &arg );
    
    virtual void mouseMoved( const SDL_Event &arg );
    virtual void mousePressed( const SDL_MouseButtonEvent &arg, Ogre::uint8 id );
    virtual void mouseReleased( const SDL_MouseButtonEvent &arg, Ogre::uint8 id );
    
public:
    void createEmptyDynamicsWorld();
    
    void stepSimulation(float deltaTime);
    
    bool pickBody(const btVector3& rayFromWorld, const btVector3& rayToWorld);
    
    bool movePickedBody(const btVector3& rayFromWorld, const btVector3& rayToWorld);
    
    void removePickingConstraint();
    
    RigidActor createRigidBody(float mass, const btTransform& startTransform,
                               btCollisionShape* shape,
                               Ogre::String name);
};
}

#endif
