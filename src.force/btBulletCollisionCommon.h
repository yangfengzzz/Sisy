/*
Bullet Continuous Collision Detection and Physics Library
Copyright (c) 2003-2006 Erwin Coumans  http://continuousphysics.com/Bullet/

This software is provided 'as-is', without any express or implied warranty.
In no event will the authors be held liable for any damages arising from the use of this software.
Permission is granted to anyone to use this software for any purpose, 
including commercial applications, and to alter it and redistribute it freely, 
subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/

#ifndef BULLET_COLLISION_COMMON_H
#define BULLET_COLLISION_COMMON_H

///Common headerfile includes for Bullet Collision Detection

///Bullet's btCollisionWorld and btCollisionObject definitions
#include "BulletCollision/btCollisionWorld.h"
#include "BulletCollision/btCollisionObject.h"

///Collision Shapes
#include "BulletCollision/btBoxShape.h"
#include "BulletCollision/btSphereShape.h"
#include "BulletCollision/btCapsuleShape.h"
#include "BulletCollision/btCylinderShape.h"
#include "BulletCollision/btConeShape.h"
#include "BulletCollision/btStaticPlaneShape.h"
#include "BulletCollision/btConvexHullShape.h"
#include "BulletCollision/btTriangleMesh.h"
#include "BulletCollision/btConvexTriangleMeshShape.h"
#include "BulletCollision/btBvhTriangleMeshShape.h"
#include "BulletCollision/btScaledBvhTriangleMeshShape.h"
#include "BulletCollision/btTriangleMeshShape.h"
#include "BulletCollision/btTriangleIndexVertexArray.h"
#include "BulletCollision/btCompoundShape.h"
#include "BulletCollision/btTetrahedronShape.h"
#include "BulletCollision/btEmptyShape.h"
#include "BulletCollision/btMultiSphereShape.h"
#include "BulletCollision/btUniformScalingShape.h"

///Narrowphase Collision Detector
#include "BulletCollision/btSphereSphereCollisionAlgorithm.h"

//#include "BulletCollision/btSphereBoxCollisionAlgorithm.h"
#include "BulletCollision/btDefaultCollisionConfiguration.h"

///Dispatching and generation of collision pairs (broadphase)
#include "BulletCollision/btCollisionDispatcher.h"
#include "BulletCollision/btSimpleBroadphase.h"
#include "BulletCollision/btAxisSweep3.h"
#include "BulletCollision/btDbvtBroadphase.h"

///Math library & Utils
#include "LinearMath/btQuaternion.h"
#include "LinearMath/btTransform.h"
#include "LinearMath/btDefaultMotionState.h"
#include "LinearMath/btQuickprof.h"
#include "LinearMath/btIDebugDraw.h"
#include "LinearMath/btSerializer.h"

#endif  //BULLET_COLLISION_COMMON_H
