//
//  jetActor.hpp
//  Sisy
//
//  Created by 杨丰 on 2020/3/20.
//  Copyright © 2020 杨丰. All rights reserved.
//

#ifndef jetActor_hpp
#define jetActor_hpp

#include "OgreItem.h"
#include "jetShape.hpp"
#include "btBulletDynamicsCommon.h"
#include "GraphicsSystem.h"

namespace jet{
class JetActor{
public:
    enum DebugDrawModes
    {
        DBG_NoDebug = 0,
        DBG_DrawWireframe = 1,
        DBG_DrawAabb = 2,
        DBG_DrawFeaturesText = 4,
        DBG_DrawContactPoints = 8,
        DBG_NoDeactivation = 16,
        DBG_NoHelpText = 32,
        DBG_DrawText = 64,
        DBG_ProfileTimings = 128,
        DBG_EnableSatComparison = 256,
        DBG_DisableBulletLCP = 512,
        DBG_EnableCCD = 1024,
        DBG_DrawConstraints = (1 << 11),
        DBG_DrawConstraintLimits = (1 << 12),
        DBG_FastWireframe = (1 << 13),
        DBG_DrawNormals = (1 << 14),
        DBG_DrawFrames = (1 << 15),
        DBG_MAX_DEBUG_DRAW_MODE
    };
    
public:
    JetActor(btDiscreteDynamicsWorld* m_dynamicsWorld,
             GraphicsSystem* app,
             JetShape* shape){
        m_world = m_dynamicsWorld;
        m_app = app;
        m_shape = shape;
    }
    
    virtual void createRenderItem(Ogre::String name) = 0;
    virtual Ogre::SceneNode* debugDrawObject(const btVector3& color) = 0;
    
    Ogre::Item* getItem(){return m_item; }
    Ogre::SceneNode* getSceneNode(){return m_node; }
    
public:
    int getDebugMode(){return m_debugMode; }
    
    Ogre::SceneNode* drawTransform(const btTransform& transform,
                                   btScalar orthoLen);
    
protected:
    Ogre::SceneNode* m_node;
    Ogre::Item* m_item;
    
    btDiscreteDynamicsWorld* m_world;
    GraphicsSystem* m_app;
    JetShape* m_shape;
    
    int m_debugMode;
};
}



#endif /* jetActor_hpp */
