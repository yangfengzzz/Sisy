//
//  BulletConverter.hpp
//  Sisy
//
//  Created by 杨丰 on 2020/3/19.
//  Copyright © 2020 杨丰. All rights reserved.
//

#ifndef BulletConverter_hpp
#define BulletConverter_hpp

#include "OgreString.h"
#include "btVector3.h"
#include "btCollisionShape.h"

namespace Demo {
class GraphicsSystem;

class BulletConverter{
public:
    BulletConverter(GraphicsSystem *graphicsSystem );
    
public:
    Ogre::IndexBufferPacked*
    createIndexBuffer(const Ogre::uint16* indices, int numIndices);

    std::pair<Ogre::MeshPtr, Ogre::VertexBufferPacked*>
    createDynamicMesh(const float* vertices, int numvertices,
                      const Ogre::uint16* indices, int numIndices, Ogre::String name);

    std::pair<Ogre::MeshPtr, Ogre::VertexBufferPacked*>
    createCollisionShapeGraphicsObject(btCollisionShape* collisionShape,
                                       Ogre::String name);
    
private:
    GraphicsSystem* m_glApp;
};




}

#endif /* BulletConverter_hpp */
