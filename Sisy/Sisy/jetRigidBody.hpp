//
//  jetRigidBody.hpp
//  Sisy
//
//  Created by 杨丰 on 2020/3/20.
//  Copyright © 2020 杨丰. All rights reserved.
//

#ifndef jetRigidBody_hpp
#define jetRigidBody_hpp

#include "jetActor.hpp"
#include "btRigidBody.h"

namespace jet{
class JetRigidActor : public JetActor{
public:

    
public:
    JetRigidActor(btDiscreteDynamicsWorld* m_dynamicsWorld,
                  GraphicsSystem* app,
                  JetShape* shape,
                  float mass,
                  const btTransform& startTransform);
    
    void createRenderItem(Ogre::String name) override;
    
    Ogre::SceneNode* debugDrawObject(const btVector3& color) override;
    
    btRigidBody* getBody(){return m_body;}
    
private:
    btRigidBody* m_body;

};
}

#endif /* jetRigidBody_hpp */
