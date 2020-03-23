//
//  jetActor.hpp
//  Sisy
//
//  Created by 杨丰 on 2020/3/20.
//  Copyright © 2020 杨丰. All rights reserved.
//

#ifndef jetActor_hpp
#define jetActor_hpp

#include "OgreItem.h"
#include "jetShape.hpp"
#include "btBulletDynamicsCommon.h"
#include "GraphicsSystem.h"

namespace jet{
class JetActor{
public:
    JetActor(btDiscreteDynamicsWorld* m_dynamicsWorld,
             GraphicsSystem* app,
             JetShape* shape){
        m_world = m_dynamicsWorld;
        m_app = app;
        m_shape = shape;
    }
    
    Ogre::Item* getItem(){return m_item; }
    Ogre::SceneNode* getSceneNode(){return m_node; }
    
protected:
    Ogre::SceneNode* m_node;
    Ogre::Item* m_item;
    
    btDiscreteDynamicsWorld* m_world;
    GraphicsSystem* m_app;
    JetShape* m_shape;
    
    int m_debugMode;
};
}



#endif /* jetActor_hpp */
