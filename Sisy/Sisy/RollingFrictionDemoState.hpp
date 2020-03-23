//
//  RollingFrictionDemoState.hpp
//  Sisy
//
//  Created by 杨丰 on 2020/3/22.
//  Copyright © 2020 杨丰. All rights reserved.
//

#ifndef RollingFrictionDemoState_hpp
#define RollingFrictionDemoState_hpp

#include "OgrePrerequisites.h"
#include "TutorialGameState.h"

#include "OgreDebugDrawer.hpp"

namespace jet{
class JetRigidActor;
class MyGameState : public jet::TutorialGameState
{    
    Ogre::SceneNode* mDebugNode;
    OgreDebugDrawer* debug;
    Ogre::ManualObject* manual;
    
    std::vector<Ogre::SceneNode*> bulletBody;
    Ogre::SceneNode* mLightNodes[3];
    
    virtual void generateDebugText( float timeSinceLast,
                                   Ogre::String &outText );
public:
    MyGameState( const Ogre::String &helpDescription );
    
    virtual void createScene01(void);
    virtual void destroyScene(void);
    
    virtual void update( float timeSinceLast );
    
    virtual void keyReleased( const SDL_KeyboardEvent &arg );
    
private:
    void createPhysicalWorld();
};
}

#endif /* RollingFrictionDemoState_hpp */
