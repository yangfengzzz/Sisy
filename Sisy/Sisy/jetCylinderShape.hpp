//
//  jetCylinderShape.hpp
//  Sisy
//
//  Created by 杨丰 on 2020/3/21.
//  Copyright © 2020 杨丰. All rights reserved.
//

#ifndef jetCylinderShape_hpp
#define jetCylinderShape_hpp

#include "jetShape.hpp"

namespace jet {
class JetCylinderShape : public JetShape{
public:
    JetCylinderShape(){}
    JetCylinderShape(const btVector3& halfExtents);
        
    virtual Ogre::ManualObject* debugDrawObject(const btVector3& color,
                                                Ogre::SceneManager* scene) override;
};
class JetCylinderShapeX : public JetCylinderShape{
public:
    JetCylinderShapeX(const btVector3& halfExtents);
};

class JetCylinderShapeZ : public JetCylinderShape{
public:
    JetCylinderShapeZ(const btVector3& halfExtents);
};
}

#endif /* jetCylinderShape_hpp */
