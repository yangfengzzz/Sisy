//
//  jetBoxShape.hpp
//  Sisy
//
//  Created by 杨丰 on 2020/3/20.
//  Copyright © 2020 杨丰. All rights reserved.
//

#ifndef jetBoxShape_hpp
#define jetBoxShape_hpp

#include "jetShape.hpp"

namespace jet {
class JetBoxShape : public JetShape{
public:
    JetBoxShape(const btVector3& boxHalfExtents);
    
    virtual void createRenderMesh(Ogre::String name) override;
    
private:
    std::pair<Ogre::MeshPtr, Ogre::VertexBufferPacked*>
    virtual createCollisionShapeGraphicsObject(btCollisionShape* collisionShape,
                                               Ogre::String name) override;

};
}

#endif /* jetBoxShape_hpp */
