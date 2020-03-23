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
#include "OgreHlmsUnlit.h"

#define BT_LINE_BATCH_SIZE 409600

namespace jet{
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
OgreDebugDrawer : public btIDebugDraw
{
    Ogre::ManualObject* m_manual;
    Ogre::HlmsUnlit *m_hlmsUnlit;
    
    int m_debugMode;
    btVector3 m_currentLineColor;
    DefaultColors m_ourColors;
    
    btAlignedObjectArray<MyDebugVec3> m_linePoints;
    btAlignedObjectArray<unsigned int> m_lineIndices;
    
public:
    BT_DECLARE_ALIGNED_ALLOCATOR();
    
    OgreDebugDrawer(Ogre::ManualObject* node,
                    Ogre::HlmsUnlit *hlmsUnlit);
    
    virtual ~OgreDebugDrawer()
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
    
    virtual void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB,
                                  btScalar distance, int lifeTime, const btVector3& color)
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
