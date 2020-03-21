//
//  jetCapsuleShape.hpp
//  Sisy
//
//  Created by 杨丰 on 2020/3/21.
//  Copyright © 2020 杨丰. All rights reserved.
//

#ifndef jetCapsuleShape_hpp
#define jetCapsuleShape_hpp

#include "jetShape.hpp"

namespace jet {
class JetCapsuleShape : public JetShape{
public:
    JetCapsuleShape(){}
    JetCapsuleShape(btScalar radius, btScalar height);
        
    virtual Ogre::ManualObject* debugDrawObject(const btVector3& color,
                                                Ogre::SceneManager* scene) override;
    
private:
    std::pair<Ogre::MeshPtr, Ogre::VertexBufferPacked*>
    virtual createCollisionShapeGraphicsObject(btCollisionShape* collisionShape,
                                               Ogre::String name) override;
};

class JetCapsuleShapeX : public JetCapsuleShape{
public:
    JetCapsuleShapeX(btScalar radius, btScalar height);
};

class JetCapsuleShapeZ : public JetCapsuleShape{
public:
    JetCapsuleShapeZ(btScalar radius, btScalar height);
};

}

#endif /* jetCapsuleShape_hpp */
