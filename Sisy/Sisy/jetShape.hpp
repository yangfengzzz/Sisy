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
    JetShape(btCollisionShape* m_shape):
    m_shape(m_shape){}
    
    Ogre::MeshPtr getMesh(){return m_mesh; }
    btCollisionShape* getShape(){return m_shape; }
    
    virtual void createRenderMesh(Ogre::String name);
    
protected:
    Ogre::IndexBufferPacked*
    createIndexBuffer(const Ogre::uint16* indices, int numIndices);

    std::pair<Ogre::MeshPtr, Ogre::VertexBufferPacked*>
    createDynamicMesh(const float* vertices, int numvertices,
                      const Ogre::uint16* indices, int numIndices, Ogre::String name);

    std::pair<Ogre::MeshPtr, Ogre::VertexBufferPacked*>
    virtual createCollisionShapeGraphicsObject(btCollisionShape* collisionShape,
                                               Ogre::String name);
    
    Ogre::MeshPtr m_mesh;
    btCollisionShape* m_shape;
};

}

#endif /* jetShape_hpp */
