#ifndef COLLISION_SHAPE_2_GRAPHICS_H
#define COLLISION_SHAPE_2_GRAPHICS_H

#include "btAlignedObjectArray.h"
#include "btTransform.h"
#include "OgrePrerequisites.h"
class btCollisionShape;

void CollisionShape2TriangleMesh(btCollisionShape* collisionShape,
                                 const btTransform& parentTransform,
                                 btAlignedObjectArray<btVector3>& vertexPositions,
                                 btAlignedObjectArray<btVector3>& vertexNormals,
                                 btAlignedObjectArray<Ogre::uint16>& indicesOut);

#endif  //COLLISION_SHAPE_2_GRAPHICS_H
