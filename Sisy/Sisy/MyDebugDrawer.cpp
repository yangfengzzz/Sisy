//
//  MyDebugDrawer.cpp
//  Sisy
//
//  Created by 杨丰 on 2020/3/20.
//  Copyright © 2020 杨丰. All rights reserved.
//

#include "MyDebugDrawer.hpp"

namespace jet{

void MyDebugDrawer::flushLines()
{
    int sz = m_linePoints.size();
    if (sz)
    {
        m_glApp->clear();
        m_glApp->begin(m_datablockName, Ogre::OT_LINE_LIST);
        for (int i = 0; i < sz; i+=2) {
            m_glApp->position(m_linePoints[i].x,
                                   m_linePoints[i].y,
                                   m_linePoints[i].z);
            m_glApp->position(m_linePoints[i+1].x,
                                   m_linePoints[i+1].y,
                                   m_linePoints[i+1].z);
            m_glApp->line(m_lineIndices[i], m_lineIndices[i+1]);
            m_glApp->colour(m_currentLineColor.x(),
                            m_currentLineColor.y(),
                            m_currentLineColor.z(),
                            1.f);
        }
        m_glApp->end();
        
        m_linePoints.clear();
        m_lineIndices.clear();
    }
}

}
