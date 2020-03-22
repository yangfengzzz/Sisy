//
//  MyDebugDrawer.hpp
//  Sisy
//
//  Created by 杨丰 on 2020/3/20.
//  Copyright © 2020 杨丰. All rights reserved.
//

#ifndef MyDebugDrawer_hpp
#define MyDebugDrawer_hpp

#include "btBulletDynamicsCommon.h"
#include "OgreSceneManager.h"
#include "OgreHlmsUnlitDatablock.h"

namespace jet{
#define BT_LINE_BATCH_SIZE 512
struct MyDebugVec3
{
    MyDebugVec3(const btVector3& org)
        : x(org.x()),
          y(org.y()),
          z(org.z())
    {
    }

    float x;
    float y;
    float z;
};

ATTRIBUTE_ALIGNED16(class)
MyDebugDrawer : public btIDebugDraw
{
    Ogre::SceneNode* m_node;
    Ogre::SceneManager* m_scene;
    const Ogre::String m_datablockName;
    Ogre::uint32 flags;
    int m_debugMode;
    btVector3 m_currentLineColor;
    DefaultColors m_ourColors;
    
    btAlignedObjectArray<MyDebugVec3> m_linePoints;
    btAlignedObjectArray<unsigned int> m_lineIndices;
    
    Ogre::ManualObject* manual;

public:
    BT_DECLARE_ALIGNED_ALLOCATOR();

    MyDebugDrawer(Ogre::SceneNode* node,
                  Ogre::SceneManager* scene,
                  const Ogre::String& name,
                  Ogre::uint32 flags)
    : m_node(node),
    m_scene(scene),
    m_datablockName(name),
    flags(flags),
    m_debugMode(btIDebugDraw::DBG_DrawWireframe | btIDebugDraw::DBG_DrawAabb),
    m_currentLineColor(-1, -1, -1)
    {
        manual = m_scene->createManualObject();
        m_node->attachObject(manual);
    }

    virtual ~MyDebugDrawer()
    {
    }
    virtual DefaultColors getDefaultColors() const
    {
        return m_ourColors;
    }
    ///the default implementation for setDefaultColors has no effect. A derived class can implement it and store the colors.
    virtual void setDefaultColors(const DefaultColors& colors)
    {
        m_ourColors = colors;
    }

    virtual void drawLine(const btVector3& from1, const btVector3& to1, const btVector3& color1)
    {
        //float from[4] = {from1[0],from1[1],from1[2],from1[3]};
        //float to[4] = {to1[0],to1[1],to1[2],to1[3]};
        //float color[4] = {color1[0],color1[1],color1[2],color1[3]};
        //m_glApp->m_instancingRenderer->drawLine(from,to,color);
        if (m_currentLineColor != color1 || m_linePoints.size() >= BT_LINE_BATCH_SIZE)
        {
            flushLines();
            m_currentLineColor = color1;
        }
        MyDebugVec3 from(from1);
        MyDebugVec3 to(to1);

        m_linePoints.push_back(from);
        m_linePoints.push_back(to);

        m_lineIndices.push_back(m_lineIndices.size());
        m_lineIndices.push_back(m_lineIndices.size());
    }

    virtual void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color)
    {
        drawLine(PointOnB, PointOnB + normalOnB * distance, color);
        btVector3 ncolor(0, 0, 0);
        drawLine(PointOnB, PointOnB + normalOnB * 0.01, ncolor);
    }

    virtual void reportErrorWarning(const char* warningString)
    {
    }

    virtual void draw3dText(const btVector3& location, const char* textString)
    {
    }

    virtual void setDebugMode(int debugMode)
    {
        m_debugMode = debugMode;
    }

    virtual int getDebugMode() const
    {
        return m_debugMode;
    }

    virtual void flushLines();
};

}

#endif /* MyDebugDrawer_hpp */
