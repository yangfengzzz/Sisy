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
class JetRigidBody : public JetActor{
public:
    JetRigidBody(btDiscreteDynamicsWorld* m_dynamicsWorld,
                 Demo::GraphicsSystem * app,
                 float mass,
                 const btTransform& startTransform,
                 btCollisionShape* shape);
    
    virtual void createRenderItem(Ogre::String name) override;
    
private:
    btRigidBody* m_body;
};
}

#endif /* jetRigidBody_hpp */
