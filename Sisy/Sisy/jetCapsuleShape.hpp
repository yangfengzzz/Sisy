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
    JetCapsuleShape(btCollisionShape* m_shape);
    
private:
    std::pair<Ogre::MeshPtr, Ogre::VertexBufferPacked*>
    virtual createCollisionShapeGraphicsObject(btCollisionShape* collisionShape,
                                               Ogre::String name) override;
};

}

#endif /* jetCapsuleShape_hpp */
