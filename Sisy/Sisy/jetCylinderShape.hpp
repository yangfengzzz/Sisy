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
    JetCylinderShape(const btVector3& halfExtents);
        
    virtual Ogre::ManualObject* debugDrawObject(const btVector3& color,
                                                Ogre::SceneManager* scene) override;

};
}

#endif /* jetCylinderShape_hpp */
