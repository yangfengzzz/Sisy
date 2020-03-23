//
//  OgreMotionState.hpp
//  Sisy
//
//  Created by 杨丰 on 2020/3/23.
//  Copyright © 2020 杨丰. All rights reserved.
//

#ifndef OgreMotionState_hpp
#define OgreMotionState_hpp

#include "btMotionState.h"
#include "OgreSceneNode.h"

namespace jet {

class OgreMotionState : public  btMotionState {
public :
    OgreMotionState(const btTransform &initialpos, Ogre::SceneNode *node) {
        mVisibleobj = node;
        mPos1 = initialpos;
    }

    virtual ~OgreMotionState() {
    }

    void setNode(Ogre::SceneNode *node) {
        mVisibleobj = node;
    }

    virtual void getWorldTransform(btTransform &worldTrans) const  {
        worldTrans = mPos1;
    }

    virtual void setWorldTransform(const  btTransform &worldTrans) {
        if (mVisibleobj == nullptr) return ; // silently return before we set a node
        btQuaternion rot = worldTrans.getRotation();
        mVisibleobj->setOrientation(rot.w(), rot.x(), rot.y(), rot.z());
        btVector3 pos = worldTrans.getOrigin();
        mVisibleobj->setPosition(pos.x(), pos.y(), pos.z());
    }

protected :
    Ogre::SceneNode *mVisibleobj;
    btTransform mPos1;
};

}

#endif /* OgreMotionState_hpp */
