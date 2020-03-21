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
    JetSphereShape(btScalar radius);
    
    virtual void createRenderMesh(Ogre::String name) override;
    
    virtual Ogre::ManualObject* debugDrawObject(const btVector3& color,
                                                Ogre::SceneManager* scene) override;
private:
    std::pair<Ogre::MeshPtr, Ogre::VertexBufferPacked*>
    virtual createCollisionShapeGraphicsObject(btCollisionShape* collisionShape,
                                               Ogre::String name) override;
    
    void drawSpherePatch(const btVector3& up,
                         const btVector3& axis,
                         btScalar radius,
                         btScalar minTh, btScalar maxTh,
                         btScalar minPs, btScalar maxPs,
                         const btVector3& color,
                         Ogre::ManualObject* manual,
                         btScalar stepDegrees = btScalar(10.f),
                         bool drawCenter = true);
    
private:
    void clearManualIdx(){manual_idx = 0;}
    int manual_idx;
};
}

#endif /* jetSphereShape_hpp */
