//
//  jetRigidBody.cpp
//  Sisy
//
//  Created by 杨丰 on 2020/3/20.
//  Copyright © 2020 杨丰. All rights reserved.
//

#include "jetRigidBody.hpp"
#include "btCollisionShape.h"
#include "OgreRoot.h"
#include "OgreMotionState.hpp"

namespace jet{
JetRigidActor::JetRigidActor(btDiscreteDynamicsWorld* m_dynamicsWorld,
                             GraphicsSystem* app,
                             JetShape* shape,
                             float mass,
                             const btTransform& startTransform,
                             Ogre::String name):
JetActor(m_dynamicsWorld, app, shape){
    Ogre::SceneManager *sceneManager = m_app->getSceneManager();
    m_item = sceneManager->createItem(name,
                                      Ogre::ResourceGroupManager::
                                      AUTODETECT_RESOURCE_GROUP_NAME,
                                      Ogre::SCENE_DYNAMIC );
    m_node = sceneManager->getRootSceneNode( Ogre::SCENE_DYNAMIC )->
    createChildSceneNode( Ogre::SCENE_DYNAMIC );
    m_node->attachObject( m_item );
    
    btAssert((!shape || shape->getShape()->getShapeType() != INVALID_SHAPE_PROXYTYPE));
    
    //rigidbody is dynamic if and only if mass is non zero, otherwise static
    bool isDynamic = (mass != 0.f);
    
    btVector3 localInertia(0, 0, 0);
    if (isDynamic)
        shape->getShape()->calculateLocalInertia(mass, localInertia);
    
    //using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
    
#define USE_MOTIONSTATE 1
#ifdef USE_MOTIONSTATE
    OgreMotionState* myMotionState = new OgreMotionState(startTransform, m_node);
    
    btRigidBody::btRigidBodyConstructionInfo cInfo(mass, myMotionState,
                                                   shape->getShape(), localInertia);
    
    m_body = new btRigidBody(cInfo);
    //body->setContactProcessingThreshold(m_defaultContactProcessingThreshold);
    
#else
    m_body = new btRigidBody(mass, 0, shape, localInertia);
    m_body->setWorldTransform(startTransform);
#endif  //
    
    m_body->setUserIndex(-1);
    m_dynamicsWorld->addRigidBody(m_body);
    
    btVector3 pos = m_body->getCenterOfMassPosition();
    btQuaternion orn = m_body->getCenterOfMassTransform().getRotation();
    m_node->setPosition(pos.x(), pos.y(), pos.z());
    m_node->setOrientation(orn.w(), orn.x(), orn.y(), orn.z());
}
}
