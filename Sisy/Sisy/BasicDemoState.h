
#ifndef _Demo_MyGameState_H_
#define _Demo_MyGameState_H_

#include "OgrePrerequisites.h"
#include "TutorialGameState.h"
#include "MyDebugDrawer.hpp"

namespace Demo
{
    class MyGameState : public TutorialGameState
    {
        Ogre::SceneNode* mSceneNode[125];
        btRigidBody* bulletBody[125];
        
        Ogre::SceneNode* mLightNodes[3];
        MyDebugDrawer* debug;
        
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
