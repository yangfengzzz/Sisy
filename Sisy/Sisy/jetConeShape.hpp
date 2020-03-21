//
//  jetConeShape.hpp
//  Sisy
//
//  Created by 杨丰 on 2020/3/21.
//  Copyright © 2020 杨丰. All rights reserved.
//

#ifndef jetConeShape_hpp
#define jetConeShape_hpp

#include "jetShape.hpp"

namespace jet {
class JetConeShape : public JetShape{
public:
    JetConeShape(btScalar radius, btScalar height);
        
    virtual Ogre::ManualObject* debugDrawObject(const btVector3& color,
                                                Ogre::SceneManager* scene) override;

};
}

#endif /* jetConeShape_hpp */
