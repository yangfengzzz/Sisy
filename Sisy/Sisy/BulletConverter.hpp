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

namespace jet {
class GraphicsSystem;

class BulletConverter{
public:
    BulletConverter(GraphicsSystem *graphicsSystem );
    
    std::pair<Ogre::MeshPtr, Ogre::VertexBufferPacked*>
    createCollisionShapeGraphicsObject(btCollisionShape* collisionShape,
                                       Ogre::String name);
    
public:
    std::pair<Ogre::MeshPtr, Ogre::VertexBufferPacked*>
    defaultCreator(btCollisionShape* collisionShape,
                   Ogre::String name);
    
    std::pair<Ogre::MeshPtr, Ogre::VertexBufferPacked*>
    boxCreator(btCollisionShape* collisionShape,
               Ogre::String name);
    
    std::pair<Ogre::MeshPtr, Ogre::VertexBufferPacked*>
    TerrianCreator(btCollisionShape* collisionShape,
                   Ogre::String name);
    
    std::pair<Ogre::MeshPtr, Ogre::VertexBufferPacked*>
    sphereCreator(btCollisionShape* collisionShape,
                  Ogre::String name);
    
    std::pair<Ogre::MeshPtr, Ogre::VertexBufferPacked*>
    capsuleCreator(btCollisionShape* collisionShape,
                   Ogre::String name);
    
    std::pair<Ogre::MeshPtr, Ogre::VertexBufferPacked*>
    staticPlaneCreator(btCollisionShape* collisionShape,
                       Ogre::String name);
    
public:
    Ogre::IndexBufferPacked*
    createIndexBuffer(const Ogre::uint16* indices, int numIndices);
    
    std::pair<Ogre::MeshPtr, Ogre::VertexBufferPacked*>
    createDynamicMesh(const float* vertices, int numvertices,
                      const Ogre::uint16* indices, int numIndices, Ogre::String name);
    
private:
    GraphicsSystem* m_glApp;
};

}

#endif /* BulletConverter_hpp */
