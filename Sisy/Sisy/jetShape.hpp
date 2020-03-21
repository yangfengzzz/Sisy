//
//  jetShape.hpp
//  Sisy
//
//  Created by 杨丰 on 2020/3/20.
//  Copyright © 2020 杨丰. All rights reserved.
//

#ifndef jetShape_hpp
#define jetShape_hpp

#include "btCollisionShape.h"
#include "OgreMesh2.h"
#include "OgreManualObject2.h"

namespace jet{

class JetShape {
public:
    JetShape(){
        manual_idx = 0;
    }
    
    Ogre::MeshPtr getMesh(){return m_mesh; }
    btCollisionShape* getShape(){return m_shape; }
    
    virtual void createRenderMesh(Ogre::String name) = 0;
    
    virtual Ogre::ManualObject* debugDrawObject(const btVector3& color,
                                                Ogre::SceneManager* scene) = 0;
    
protected:
    Ogre::IndexBufferPacked*
    createIndexBuffer(const Ogre::uint16* indices, int numIndices);

    std::pair<Ogre::MeshPtr, Ogre::VertexBufferPacked*>
    createDynamicMesh(const float* vertices, int numvertices,
                      const Ogre::uint16* indices, int numIndices, Ogre::String name);

    std::pair<Ogre::MeshPtr, Ogre::VertexBufferPacked*>
    virtual createCollisionShapeGraphicsObject(btCollisionShape* collisionShape,
                                               Ogre::String name) = 0;
    
    Ogre::MeshPtr m_mesh;
    btCollisionShape* m_shape;
    
protected:
    void drawLine(const btVector3& bbMin, const btVector3& bbMax,
                  Ogre::ManualObject* manual);
    
    void drawSpherePatch(const btVector3& center,
                         const btVector3& up,
                         const btVector3& axis,
                         btScalar radius,
                         btScalar minTh, btScalar maxTh,
                         btScalar minPs, btScalar maxPs,
                         const btVector3& color,
                         Ogre::ManualObject* manual,
                         btScalar stepDegrees = btScalar(10.f),
                         bool drawCenter = true);
    
    void clearManualIdx(){manual_idx = 0;}
    
private:
    void drawLine(const btVector3& bbMin, const btVector3& bbMax,
                  int idx1, int idx2,
                  Ogre::ManualObject* manual);
    
    int manual_idx;
};

}

#endif /* jetShape_hpp */
