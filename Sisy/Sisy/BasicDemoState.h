
#ifndef _Demo_MyGameState_H_
#define _Demo_MyGameState_H_

#include "OgrePrerequisites.h"
#include "TutorialGameState.h"

namespace Demo
{
    class MyGameState : public TutorialGameState
    {
        Ogre::SceneNode* mSceneNode[125];
        btRigidBody* bulletBody[125];
        
        Ogre::SceneNode     *mLightNodes[3];
    public:
        MyGameState( const Ogre::String &helpDescription );
        
        virtual void createScene01(void);
        
        virtual void update( float timeSinceLast );
    };
}

#endif
