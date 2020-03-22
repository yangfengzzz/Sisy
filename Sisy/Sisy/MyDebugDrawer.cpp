//
//  MyDebugDrawer.cpp
//  Sisy
//
//  Created by 杨丰 on 2020/3/20.
//  Copyright © 2020 杨丰. All rights reserved.
//

#include "MyDebugDrawer.hpp"
#include "OgreSceneManager.h"


namespace jet{

void MyDebugDrawer::flushLines()
{
    int sz = m_linePoints.size();
    if (sz)
    {
//        Ogre::ManualObject* manual = m_scene->createManualObject();
        manual->setVisibilityFlags(flags);
        manual->begin(m_datablockName, Ogre::OT_LINE_LIST);
        for (int i = 0; i < sz; i+=2) {
            manual->position(m_linePoints[i].x,
                             m_linePoints[i].y,
                             m_linePoints[i].z);
            manual->position(m_linePoints[i+1].x,
                             m_linePoints[i+1].y,
                             m_linePoints[i+1].z);
            manual->line(m_lineIndices[i], m_lineIndices[i+1]);
        }
        manual->colour(m_currentLineColor.x(),
                       m_currentLineColor.y(),
                       m_currentLineColor.z(),
                       1.f);
        manual->end();
        
        m_linePoints.clear();
        m_lineIndices.clear();
    }
}

}
