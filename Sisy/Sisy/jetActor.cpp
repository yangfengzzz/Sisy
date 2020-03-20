//
//  jetActor.cpp
//  Sisy
//
//  Created by 杨丰 on 2020/3/20.
//  Copyright © 2020 杨丰. All rights reserved.
//

#include "jetActor.hpp"
#include "OgreSceneManager.h"
namespace jet{
Ogre::SceneNode* JetActor::drawTransform(const btTransform& transform,
                                         btScalar orthoLen){
    btVector3 start = transform.getOrigin();
    Ogre::ManualObject * manualObject = m_app->getSceneManager()->createManualObject();

    manualObject->begin("BaseWhite", Ogre::OT_LINE_LIST);
    
    btVector3 shift = transform.getBasis() * btVector3(orthoLen, 0, 0);
    manualObject->position(start.x(), start.y(), start.z());
    manualObject->position(start.x() + shift.x(),
                           start.y() + shift.y(),
                           start.z() + shift.z());
    manualObject->line(0, 1);
    
    shift = transform.getBasis() * btVector3(0, orthoLen, 0);
    manualObject->position(start.x(), start.y(), start.z());
    manualObject->position(start.x() + shift.x(),
                           start.y() + shift.y(),
                           start.z() + shift.z());
    manualObject->line(2, 3);
    
    shift = transform.getBasis() * btVector3(0, 0, orthoLen);
    manualObject->position(start.x(), start.y(), start.z());
    manualObject->position(start.x() + shift.x(),
                           start.y() + shift.y(),
                           start.z() + shift.z());
    manualObject->line(4, 5);
    
    manualObject->end();
    
    Ogre::SceneNode* sceneNodeLines = m_node->createChildSceneNode( Ogre::SCENE_DYNAMIC );
    sceneNodeLines->attachObject(manualObject);
    
    return sceneNodeLines;
}

}
