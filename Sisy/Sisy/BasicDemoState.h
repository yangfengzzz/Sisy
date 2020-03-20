
#ifndef _Demo_MyGameState_H_
#define _Demo_MyGameState_H_

#include "OgrePrerequisites.h"
#include "TutorialGameState.h"

namespace jet{
class JetRigidActor;
class MyGameState : public jet::TutorialGameState
{
    Ogre::SceneNode* mSceneNode[125];
    JetRigidActor* bulletBody[125];
    
    Ogre::SceneNode* mLightNodes[3];
    
    virtual void generateDebugText( float timeSinceLast,
                                   Ogre::String &outText );
public:
    MyGameState( const Ogre::String &helpDescription );
    
    virtual void createScene01(void);
    virtual void destroyScene(void);
    
    virtual void update( float timeSinceLast );
    
    virtual void keyReleased( const SDL_KeyboardEvent &arg );
};
}

#endif
