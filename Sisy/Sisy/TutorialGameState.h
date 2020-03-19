
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

namespace Demo{
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
    BulletConverter     *mBulletHelper;
    
public:
    TutorialGameState( const Ogre::String &helpDescription );
    virtual ~TutorialGameState();
    
    void _notifyGraphicsSystem( GraphicsSystem *graphicsSystem );
    
    virtual void createScene01(void);
    
    virtual void update( float timeSinceLast );
    
    virtual void keyPressed( const SDL_KeyboardEvent &arg );
    virtual void keyReleased( const SDL_KeyboardEvent &arg );
    
    virtual void mouseMoved( const SDL_Event &arg );
    
public:
    void createEmptyDynamicsWorld();
    
    btBoxShape* createBoxShape(const btVector3& halfExtents);
    
    btRigidBody* createRigidBody(float mass, const btTransform& startTransform,
                                 btCollisionShape* shape,
                                 const btVector4& color = btVector4(1, 0, 0, 1));
    
    void stepSimulation(float deltaTime);
};
}

#endif
