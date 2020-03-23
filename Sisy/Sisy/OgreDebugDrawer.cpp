//
//  MyDebugDrawer.cpp
//  Sisy
//
//  Created by 杨丰 on 2020/3/20.
//  Copyright © 2020 杨丰. All rights reserved.
//

#include "OgreDebugDrawer.hpp"
#include "OgreSceneManager.h"


namespace jet{
OgreDebugDrawer::OgreDebugDrawer(Ogre::ManualObject* node,
                                 Ogre::HlmsUnlit *hlmsUnlit)
: m_manual(node),
m_hlmsUnlit(hlmsUnlit),
m_debugMode(btIDebugDraw::DBG_DrawWireframe),
m_currentLineColor(-1, -1, -1){
    Ogre::String datablockName = "debugDatalock";
    
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            for (int k = 0; k < 2; k++) {
                Ogre::String name = datablockName
                + "_" + std::to_string(i)
                + "_" + std::to_string(j)
                + "_" + std::to_string(k);
                
                Ogre::HlmsUnlitDatablock *datablock
                = static_cast<Ogre::HlmsUnlitDatablock*>(
                                                         hlmsUnlit->createDatablock(name,
                                                                                    name,
                                                                                    Ogre::HlmsMacroblock(),
                                                                                    Ogre::HlmsBlendblock(),
                                                                                    Ogre::HlmsParamVec() ) );
                datablock->setUseColour(true);
                datablock->setColour(Ogre::ColourValue(i,j,k,0));
            }
        }
    }
}

void OgreDebugDrawer::flushLines()
{
    int sz = m_linePoints.size();
    if (sz)
    {
        std::string name = std::string("debugDatalock")
        + "_" + std::to_string((int)m_currentLineColor.x())
        + "_" + std::to_string((int)m_currentLineColor.y())
        + "_" + std::to_string((int)m_currentLineColor.z());
        
        m_manual->begin(name, Ogre::OT_LINE_LIST);
        for (int i = 0; i < sz; i+=2) {
            m_manual->position(m_linePoints[i].x,
                               m_linePoints[i].y,
                               m_linePoints[i].z);
            m_manual->position(m_linePoints[i+1].x,
                               m_linePoints[i+1].y,
                               m_linePoints[i+1].z);
            m_manual->line(m_lineIndices[i], m_lineIndices[i+1]);
        }
        m_manual->end();
        
        m_linePoints.clear();
        m_lineIndices.clear();
    }
}

}
