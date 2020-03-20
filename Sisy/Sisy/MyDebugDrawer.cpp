//
//  MyDebugDrawer.cpp
//  Sisy
//
//  Created by 杨丰 on 2020/3/20.
//  Copyright © 2020 杨丰. All rights reserved.
//

#include "MyDebugDrawer.hpp"
#include "OgreManualObject2.h"
#include "OgreSceneManager.h"

namespace Demo{

void MyDebugDrawer::flushLines()
{
    Ogre::SceneManager *sceneManager = m_glApp->getSceneManager();
    int sz = m_linePoints.size();
    if (sz)
    {
        Ogre::ManualObject * manualObject = sceneManager->createManualObject();
        manualObject->begin("BaseWhite", Ogre::OT_LINE_LIST);
        for (int i = 0; i < sz; i+=2) {
            manualObject->position(m_linePoints[i].x,
                                   m_linePoints[i].y,
                                   m_linePoints[i].z);
            manualObject->position(m_linePoints[i+1].x,
                                   m_linePoints[i+1].y,
                                   m_linePoints[i+1].z);
            manualObject->line(m_lineIndices[i], m_lineIndices[i+1]);
            manualObject->colour(m_currentLineColor.x(),
                                 m_currentLineColor.y(),
                                 m_currentLineColor.z(),
                                 1.f);
        }
        manualObject->end();
        
        Ogre::SceneNode *sceneNodeLines = sceneManager->getRootSceneNode( Ogre::SCENE_DYNAMIC )->
                                     createChildSceneNode( Ogre::SCENE_DYNAMIC );
        sceneNodeLines->attachObject(manualObject);
        sceneNodeLines->setVisible( true );
        visibleFlag = false;
        nodes.push_back(sceneNodeLines);
        
        m_linePoints.clear();
        m_lineIndices.clear();
    }
}

void MyDebugDrawer::reverseVisible(){
    for (int i = 0; i < nodes.size(); i++) {
        nodes[i]->flipVisibility();
    }
    visibleFlag = !visibleFlag;
}

bool MyDebugDrawer::isVisible(){
    return visibleFlag;
}

}
