//
//  jetSphereShape.hpp
//  Sisy
//
//  Created by 杨丰 on 2020/3/21.
//  Copyright © 2020 杨丰. All rights reserved.
//

#ifndef jetSphereShape_hpp
#define jetSphereShape_hpp

#include "jetShape.hpp"

namespace jet {
class JetSphereShape : public JetShape{
public:
    JetSphereShape(btCollisionShape* m_shape);
    
private:
    std::pair<Ogre::MeshPtr, Ogre::VertexBufferPacked*>
    virtual createCollisionShapeGraphicsObject(btCollisionShape* collisionShape,
                                               Ogre::String name) override;
};
}

#endif /* jetSphereShape_hpp */
