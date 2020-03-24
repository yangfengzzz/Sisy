//
//  ConstraintDemoState.cpp
//  Sisy
//
//  Created by 杨丰 on 2020/3/24.
//  Copyright © 2020 杨丰. All rights reserved.
//

#include "ConstraintDemoState.hpp"
#include "GraphicsSystem.h"
#include "CameraController.h"
#include "OgreSceneManager.h"
#include "OgreCamera.h"
#include "OgreHlmsUnlitDatablock.h"
#include "OgreHlmsPbsDatablock.h"
#include "OgreHlmsSamplerblock.h"
#include "OgreMath.h"
#include "OgreRoot.h"
#include "OgreHlmsManager.h"
#include "OgreHlmsPbs.h"
#include "OgreHlmsUnlit.h"
#include "OgreItem.h"
#include "OgreTextureGpuManager.h"
#include "OgreTextAreaOverlayElement.h"

#include "BulletConverter.hpp"

#define ENABLE_ALL_DEMOS 1

#define CUBE_HALF_EXTENTS 1.f

#define SIMD_PI_2 ((SIMD_PI)*0.5f)
#define SIMD_PI_4 ((SIMD_PI)*0.25f)

btTransform sliderTransform;
btVector3 lowerSliderLimit = btVector3(-10, 0, 0);
btVector3 hiSliderLimit = btVector3(10, 0, 0);

btRigidBody* d6body0 = 0;

btHingeConstraint* spDoorHinge = NULL;
btHingeConstraint* spHingeDynAB = NULL;
btGeneric6DofConstraint* spSlider6Dof = NULL;

static bool s_bTestConeTwistMotor = false;

namespace jet{
MyGameState::MyGameState( const Ogre::String &helpDescription ) :
TutorialGameState( helpDescription )
{
}
//-----------------------------------------------------------------------------------
void MyGameState::setupEmptyDynamicsWorld(){
    m_collisionConfiguration = new btDefaultCollisionConfiguration();
    m_dispatcher = new btCollisionDispatcher(m_collisionConfiguration);
    m_broadphase = new btDbvtBroadphase();
    m_solver = new btSequentialImpulseConstraintSolver();
    m_dynamicsWorld = new btDiscreteDynamicsWorld(m_dispatcher, m_broadphase, m_solver, m_collisionConfiguration);
}

void MyGameState::createPhysicalWorld(){
    setupEmptyDynamicsWorld();
    
    //btCollisionShape* groundShape = new btBoxShape(btVector3(btScalar(50.),btScalar(40.),btScalar(50.)));
    btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(0, 1, 0), 40);
    m_collisionShapes.push_back(groundShape);
    
    btTransform groundTransform;
    groundTransform.setIdentity();
    groundTransform.setOrigin(btVector3(0, -56, 0));
    std::pair<Ogre::SceneNode*, btRigidBody*> plane =
    createRigidBody(0, groundTransform,
                    groundShape);
    bulletBody.push_back(plane.first);
    
    btCollisionShape* shape = new btBoxShape(btVector3(CUBE_HALF_EXTENTS, CUBE_HALF_EXTENTS, CUBE_HALF_EXTENTS));
    m_collisionShapes.push_back(shape);
    btTransform trans;
    trans.setIdentity();
    trans.setOrigin(btVector3(0, 20, 0));
    
    float mass = 1.f;
    
#if ENABLE_ALL_DEMOS
    ///gear constraint demo
    
#define THETA SIMD_PI / 4.f
#define L_1 (2 - std::tan(THETA))
#define L_2 (1 / std::cos(THETA))
#define RATIO L_2 / L_1
    
    btRigidBody* bodyA = 0;
    btRigidBody* bodyB = 0;
    
    {
        btCollisionShape* cylA = new btCylinderShape(btVector3(0.2, 0.25, 0.2));
        btCollisionShape* cylB = new btCylinderShape(btVector3(L_1, 0.025, L_1));
        btCompoundShape* cyl0 = new btCompoundShape();
        cyl0->addChildShape(btTransform::getIdentity(), cylA);
        cyl0->addChildShape(btTransform::getIdentity(), cylB);
        
        btScalar mass = 6.28;
        btVector3 localInertia;
        cyl0->calculateLocalInertia(mass, localInertia);
        btRigidBody::btRigidBodyConstructionInfo ci(mass, 0, cyl0, localInertia);
        ci.m_startWorldTransform.setOrigin(btVector3(-8, 1, -8));
        
        btRigidBody* body = new btRigidBody(ci);  //1,0,cyl0,localInertia);
        m_dynamicsWorld->addRigidBody(body);
        
        body->setLinearFactor(btVector3(0, 0, 0));
        body->setAngularFactor(btVector3(0, 1, 0));
        bodyA = body;
    }
    
    {
        btCollisionShape* cylA = new btCylinderShape(btVector3(0.2, 0.26, 0.2));
        btCollisionShape* cylB = new btCylinderShape(btVector3(L_2, 0.025, L_2));
        btCompoundShape* cyl0 = new btCompoundShape();
        cyl0->addChildShape(btTransform::getIdentity(), cylA);
        cyl0->addChildShape(btTransform::getIdentity(), cylB);
        
        btScalar mass = 6.28;
        btVector3 localInertia;
        cyl0->calculateLocalInertia(mass, localInertia);
        btRigidBody::btRigidBodyConstructionInfo ci(mass, 0, cyl0, localInertia);
        ci.m_startWorldTransform.setOrigin(btVector3(-10, 2, -8));
        
        btQuaternion orn(btVector3(0, 0, 1), -THETA);
        ci.m_startWorldTransform.setRotation(orn);
        
        btRigidBody* body = new btRigidBody(ci);  //1,0,cyl0,localInertia);
        body->setLinearFactor(btVector3(0, 0, 0));
        btHingeConstraint* hinge = new btHingeConstraint(*body, btVector3(0, 0, 0), btVector3(0, 1, 0), true);
        m_dynamicsWorld->addConstraint(hinge);
        bodyB = body;
        body->setAngularVelocity(btVector3(0, 3, 0));
        
        m_dynamicsWorld->addRigidBody(body);
    }
    
    btVector3 axisA(0, 1, 0);
    btVector3 axisB(0, 1, 0);
    btQuaternion orn(btVector3(0, 0, 1), -THETA);
    btMatrix3x3 mat(orn);
    axisB = mat.getRow(1);
    
    btGearConstraint* gear = new btGearConstraint(*bodyA, *bodyB, axisA, axisB, RATIO);
    m_dynamicsWorld->addConstraint(gear, true);
    
#endif
    
#if ENABLE_ALL_DEMOS
    //point to point constraint with a breaking threshold
    {
        trans.setIdentity();
        trans.setOrigin(btVector3(1, 30, -5));
        createRigidBody(mass, trans, shape);
        trans.setOrigin(btVector3(0, 0, -5));
        
        std::pair<Ogre::SceneNode*, btRigidBody*> body0 =
        createRigidBody(mass, trans,
                        shape);
        bulletBody.push_back(body0.first);
        
        trans.setOrigin(btVector3(2 * CUBE_HALF_EXTENTS, 20, 0));
        mass = 1.f;
        //    btRigidBody* body1 = 0;//createRigidBody( mass,trans,shape);
        btVector3 pivotInA(CUBE_HALF_EXTENTS, CUBE_HALF_EXTENTS, 0);
        btTypedConstraint* p2p = new btPoint2PointConstraint(*body0.second, pivotInA);
        m_dynamicsWorld->addConstraint(p2p);
        p2p->setBreakingImpulseThreshold(10.2);
        p2p->setDbgDrawSize(btScalar(5.f));
    }
#endif
    
#if ENABLE_ALL_DEMOS
    //point to point constraint (ball socket)
    {
        std::pair<Ogre::SceneNode*, btRigidBody*> body0 =
        createRigidBody(mass, trans,
                        shape);
        bulletBody.push_back(body0.first);
        
        trans.setOrigin(btVector3(2 * CUBE_HALF_EXTENTS, 20, 0));
        
        mass = 1.f;
        
        btVector3 pivotInA(CUBE_HALF_EXTENTS, -CUBE_HALF_EXTENTS, -CUBE_HALF_EXTENTS);
        btVector3 axisInA(0, 0, 1);
        
        body0.second->getCenterOfMassTransform().getBasis() * axisInA;
        
#define P2P
#ifdef P2P
        btTypedConstraint* p2p = new btPoint2PointConstraint(*body0.second, pivotInA);
        m_dynamicsWorld->addConstraint(p2p);
        p2p->setDbgDrawSize(btScalar(5.f));
#else
        btHingeConstraint* hinge = new btHingeConstraint(*body0, pivotInA, axisInA);
        
        float targetVelocity = 1.f;
        float maxMotorImpulse = 1.0f;
        hinge->enableAngularMotor(true, targetVelocity, maxMotorImpulse);
        m_dynamicsWorld->addConstraint(hinge);
        hinge->setDbgDrawSize(btScalar(5.f));
#endif  //P2P
    }
#endif
    
#if ENABLE_ALL_DEMOS
    {
        btTransform trans;
        trans.setIdentity();
        btVector3 worldPos(-20, 0, 30);
        trans.setOrigin(worldPos);
        
        btTransform frameInA, frameInB;
        frameInA = btTransform::getIdentity();
        frameInB = btTransform::getIdentity();
        
        std::pair<Ogre::SceneNode*, btRigidBody*> pRbA1 =
        createRigidBody(mass, trans,
                        shape);
        bulletBody.push_back(pRbA1.first);
        pRbA1.second->setActivationState(DISABLE_DEACTIVATION);
        
        // add dynamic rigid body B1
        worldPos.setValue(-30, 0, 30);
        trans.setOrigin(worldPos);
        
        std::pair<Ogre::SceneNode*, btRigidBody*> pRbB1 =
        createRigidBody(mass, trans,
                        shape);
        bulletBody.push_back(pRbB1.first);
        pRbB1.second->setActivationState(DISABLE_DEACTIVATION);
        
        // create slider constraint between A1 and B1 and add it to world
        btSliderConstraint* spSlider1 = new btSliderConstraint(*pRbA1.second, *pRbB1.second,
                                                               frameInA, frameInB, true);
        spSlider1->setLowerLinLimit(-15.0F);
        spSlider1->setUpperLinLimit(-5.0F);
        spSlider1->setLowerAngLimit(-SIMD_PI / 3.0F);
        spSlider1->setUpperAngLimit(SIMD_PI / 3.0F);
        
        m_dynamicsWorld->addConstraint(spSlider1, true);
        spSlider1->setDbgDrawSize(btScalar(5.f));
    }
#endif
    
#if ENABLE_ALL_DEMOS
    //create a slider, using the generic D6 constraint
    {
        mass = 1.f;
        btVector3 sliderWorldPos(0, 10, 0);
        btVector3 sliderAxis(1, 0, 0);
        btScalar angle = 0.f;  //SIMD_RADS_PER_DEG * 10.f;
        btMatrix3x3 sliderOrientation(btQuaternion(sliderAxis, angle));
        trans.setIdentity();
        trans.setOrigin(sliderWorldPos);
        sliderTransform = trans;
        
        std::pair<Ogre::SceneNode*, btRigidBody*> d6body0 =
        createRigidBody(mass, trans,
                        shape);
        bulletBody.push_back(d6body0.first);
        d6body0.second->setActivationState(DISABLE_DEACTIVATION);
        
        std::pair<Ogre::SceneNode*, btRigidBody*> fixedBody1 =
        createRigidBody(0, trans,
                        0);
        bulletBody.push_back(fixedBody1.first);
        
        m_dynamicsWorld->addRigidBody(fixedBody1.second);
        
        btTransform frameInA, frameInB;
        frameInA = btTransform::getIdentity();
        frameInB = btTransform::getIdentity();
        frameInA.setOrigin(btVector3(0., 5., 0.));
        frameInB.setOrigin(btVector3(0., 5., 0.));
        
        bool useLinearReferenceFrameA = true;  //use fixed frame A for linear llimits
        spSlider6Dof = new btGeneric6DofConstraint(*fixedBody1.second, *d6body0.second,
                                                   frameInA, frameInB, useLinearReferenceFrameA);
        spSlider6Dof->setLinearLowerLimit(lowerSliderLimit);
        spSlider6Dof->setLinearUpperLimit(hiSliderLimit);
        spSlider6Dof->setAngularLowerLimit(btVector3(-SIMD_PI, 0, 0));
        spSlider6Dof->setAngularUpperLimit(btVector3(1.5, 0, 0));
        
        spSlider6Dof->getTranslationalLimitMotor()->m_enableMotor[0] = true;
        spSlider6Dof->getTranslationalLimitMotor()->m_targetVelocity[0] = -5.0f;
        spSlider6Dof->getTranslationalLimitMotor()->m_maxMotorForce[0] = 6.0f;
        
        m_dynamicsWorld->addConstraint(spSlider6Dof);
        spSlider6Dof->setDbgDrawSize(btScalar(5.f));
    }
#endif
#if ENABLE_ALL_DEMOS
    {  // create a door using hinge constraint attached to the world
        btCollisionShape* pDoorShape = new btBoxShape(btVector3(2.0f, 5.0f, 0.2f));
        m_collisionShapes.push_back(pDoorShape);
        btTransform doorTrans;
        doorTrans.setIdentity();
        doorTrans.setOrigin(btVector3(-5.0f, -2.0f, 0.0f));
        
        std::pair<Ogre::SceneNode*, btRigidBody*> pDoorBody =
        createRigidBody(1.0, doorTrans,
                        pDoorShape);
        bulletBody.push_back(pDoorBody.first);
        
        pDoorBody.second->setActivationState(DISABLE_DEACTIVATION);
        const btVector3 btPivotA(10.f + 2.1f, -2.0f, 0.0f);  // right next to the door slightly outside
        btVector3 btAxisA(0.0f, 1.0f, 0.0f);                 // pointing upwards, aka Y-axis
        
        spDoorHinge = new btHingeConstraint(*pDoorBody.second, btPivotA, btAxisA);
        
        spDoorHinge->setLimit(-SIMD_PI * 0.25f, SIMD_PI * 0.25f);
        m_dynamicsWorld->addConstraint(spDoorHinge);
        spDoorHinge->setDbgDrawSize(btScalar(5.f));
    }
#endif
#if ENABLE_ALL_DEMOS
    {  // create a generic 6DOF constraint
        btTransform tr;
        tr.setIdentity();
        tr.setOrigin(btVector3(btScalar(10.), btScalar(6.), btScalar(0.)));
        tr.getBasis().setEulerZYX(0, 0, 0);
        
        std::pair<Ogre::SceneNode*, btRigidBody*> pBodyA =
        createRigidBody(0.0, tr,
                        shape);
        bulletBody.push_back(pBodyA.first);
        pBodyA.second->setActivationState(DISABLE_DEACTIVATION);
        
        tr.setIdentity();
        tr.setOrigin(btVector3(btScalar(0.), btScalar(6.), btScalar(0.)));
        tr.getBasis().setEulerZYX(0, 0, 0);
        
        std::pair<Ogre::SceneNode*, btRigidBody*> pBodyB =
        createRigidBody(mass, tr,
                        shape);
        bulletBody.push_back(pBodyB.first);
        pBodyB.second->setActivationState(DISABLE_DEACTIVATION);
        
        btTransform frameInA, frameInB;
        frameInA = btTransform::getIdentity();
        frameInA.setOrigin(btVector3(btScalar(-5.), btScalar(0.), btScalar(0.)));
        frameInB = btTransform::getIdentity();
        frameInB.setOrigin(btVector3(btScalar(5.), btScalar(0.), btScalar(0.)));
        
        btGeneric6DofConstraint* pGen6DOF = new btGeneric6DofConstraint(*pBodyA.second, *pBodyB.second,
                                                                        frameInA, frameInB, true);
        pGen6DOF->setLinearLowerLimit(btVector3(-10., -2., -1.));
        pGen6DOF->setLinearUpperLimit(btVector3(10., 2., 1.));
        pGen6DOF->setAngularLowerLimit(btVector3(-SIMD_HALF_PI * 0.5f, -0.75, -SIMD_HALF_PI * 0.8f));
        pGen6DOF->setAngularUpperLimit(btVector3(SIMD_HALF_PI * 0.5f, 0.75, SIMD_HALF_PI * 0.8f));
        
        m_dynamicsWorld->addConstraint(pGen6DOF, true);
        pGen6DOF->setDbgDrawSize(btScalar(5.f));
    }
#endif
#if ENABLE_ALL_DEMOS
    {  // create a ConeTwist constraint
        btTransform tr;
        tr.setIdentity();
        tr.setOrigin(btVector3(btScalar(-10.), btScalar(5.), btScalar(0.)));
        tr.getBasis().setEulerZYX(0, 0, 0);
        
        std::pair<Ogre::SceneNode*, btRigidBody*> pBodyA =
        createRigidBody(1.0, tr,
                        shape);
        bulletBody.push_back(pBodyA.first);
        pBodyA.second->setActivationState(DISABLE_DEACTIVATION);
        
        tr.setIdentity();
        tr.setOrigin(btVector3(btScalar(-10.), btScalar(-5.), btScalar(0.)));
        tr.getBasis().setEulerZYX(0, 0, 0);
        
        std::pair<Ogre::SceneNode*, btRigidBody*> pBodyB =
        createRigidBody(0.0, tr,
                        shape);
        bulletBody.push_back(pBodyB.first);
        
        btTransform frameInA, frameInB;
        frameInA = btTransform::getIdentity();
        frameInA.getBasis().setEulerZYX(0, 0, SIMD_PI_2);
        frameInA.setOrigin(btVector3(btScalar(0.), btScalar(-5.), btScalar(0.)));
        frameInB = btTransform::getIdentity();
        frameInB.getBasis().setEulerZYX(0, 0, SIMD_PI_2);
        frameInB.setOrigin(btVector3(btScalar(0.), btScalar(5.), btScalar(0.)));
        
        m_ctc = new btConeTwistConstraint(*pBodyA.second, *pBodyB.second,
                                          frameInA, frameInB);
        m_ctc->setLimit(btScalar(SIMD_PI_4 * 0.6f), btScalar(SIMD_PI_4), btScalar(SIMD_PI) * 0.8f, 0.5f);
        m_dynamicsWorld->addConstraint(m_ctc, true);
        m_ctc->setDbgDrawSize(btScalar(5.f));
        s_bTestConeTwistMotor = false;
    }
#endif
#if ENABLE_ALL_DEMOS
    {  // Hinge connected to the world, with motor (to hinge motor with new and old constraint solver)
        btTransform tr;
        tr.setIdentity();
        tr.setOrigin(btVector3(btScalar(0.), btScalar(0.), btScalar(0.)));
        
        std::pair<Ogre::SceneNode*, btRigidBody*> pBody =
        createRigidBody(1.0, tr,
                        shape);
        bulletBody.push_back(pBody.first);
        pBody.second->setActivationState(DISABLE_DEACTIVATION);
        const btVector3 btPivotA(10.0f, 0.0f, 0.0f);
        btVector3 btAxisA(0.0f, 0.0f, 1.0f);
        
        btHingeConstraint* pHinge = new btHingeConstraint(*pBody.second, btPivotA, btAxisA);
        pHinge->enableAngularMotor(true, -1.0f, 1.65f);  // use for the new SIMD solver
        m_dynamicsWorld->addConstraint(pHinge);
        pHinge->setDbgDrawSize(btScalar(5.f));
    }
#endif
    
#if ENABLE_ALL_DEMOS
    {
        // create a universal joint using generic 6DOF constraint
        // create two rigid bodies
        // static bodyA (parent) on top:
        btTransform tr;
        tr.setIdentity();
        tr.setOrigin(btVector3(btScalar(20.), btScalar(4.), btScalar(0.)));
        
        std::pair<Ogre::SceneNode*, btRigidBody*> pBodyA =
        createRigidBody(0.0, tr,
                        shape);
        bulletBody.push_back(pBodyA.first);
        
        pBodyA.second->setActivationState(DISABLE_DEACTIVATION);
        // dynamic bodyB (child) below it :
        tr.setIdentity();
        tr.setOrigin(btVector3(btScalar(20.), btScalar(0.), btScalar(0.)));
        
        std::pair<Ogre::SceneNode*, btRigidBody*> pBodyB =
        createRigidBody(1.0, tr,
                        shape);
        bulletBody.push_back(pBodyB.first);
        
        pBodyB.second->setActivationState(DISABLE_DEACTIVATION);
        // add some (arbitrary) data to build constraint frames
        btVector3 parentAxis(1.f, 0.f, 0.f);
        btVector3 childAxis(0.f, 0.f, 1.f);
        btVector3 anchor(20.f, 2.f, 0.f);
        
        btUniversalConstraint* pUniv = new btUniversalConstraint(*pBodyA.second, *pBodyB.second,
                                                                 anchor, parentAxis, childAxis);
        pUniv->setLowerLimit(-SIMD_HALF_PI * 0.5f, -SIMD_HALF_PI * 0.5f);
        pUniv->setUpperLimit(SIMD_HALF_PI * 0.5f, SIMD_HALF_PI * 0.5f);
        // add constraint to world
        m_dynamicsWorld->addConstraint(pUniv, true);
        // draw constraint frames and limits for debugging
        pUniv->setDbgDrawSize(btScalar(5.f));
    }
#endif
    
#if ENABLE_ALL_DEMOS
    {  // create a generic 6DOF constraint with springs
        btTransform tr;
        tr.setIdentity();
        tr.setOrigin(btVector3(btScalar(-20.), btScalar(16.), btScalar(0.)));
        tr.getBasis().setEulerZYX(0, 0, 0);
        
        std::pair<Ogre::SceneNode*, btRigidBody*> pBodyA =
        createRigidBody(0.0, tr,
                        shape);
        bulletBody.push_back(pBodyA.first);
        pBodyA.second->setActivationState(DISABLE_DEACTIVATION);
        
        tr.setIdentity();
        tr.setOrigin(btVector3(btScalar(-10.), btScalar(16.), btScalar(0.)));
        tr.getBasis().setEulerZYX(0, 0, 0);
        
        std::pair<Ogre::SceneNode*, btRigidBody*> pBodyB =
        createRigidBody(1.0, tr,
                        shape);
        bulletBody.push_back(pBodyB.first);
        pBodyB.second->setActivationState(DISABLE_DEACTIVATION);
        
        btTransform frameInA, frameInB;
        frameInA = btTransform::getIdentity();
        frameInA.setOrigin(btVector3(btScalar(10.), btScalar(0.), btScalar(0.)));
        frameInB = btTransform::getIdentity();
        frameInB.setOrigin(btVector3(btScalar(0.), btScalar(0.), btScalar(0.)));
        
        btGeneric6DofSpringConstraint* pGen6DOFSpring
        = new btGeneric6DofSpringConstraint(*pBodyA.second, *pBodyB.second,
                                            frameInA, frameInB, true);
        pGen6DOFSpring->setLinearUpperLimit(btVector3(5., 0., 0.));
        pGen6DOFSpring->setLinearLowerLimit(btVector3(-5., 0., 0.));
        
        pGen6DOFSpring->setAngularLowerLimit(btVector3(0.f, 0.f, -1.5f));
        pGen6DOFSpring->setAngularUpperLimit(btVector3(0.f, 0.f, 1.5f));
        
        m_dynamicsWorld->addConstraint(pGen6DOFSpring, true);
        pGen6DOFSpring->setDbgDrawSize(btScalar(5.f));
        
        pGen6DOFSpring->enableSpring(0, true);
        pGen6DOFSpring->setStiffness(0, 39.478f);
        pGen6DOFSpring->setDamping(0, 0.5f);
        pGen6DOFSpring->enableSpring(5, true);
        pGen6DOFSpring->setStiffness(5, 39.478f);
        pGen6DOFSpring->setDamping(0, 0.3f);
        pGen6DOFSpring->setEquilibriumPoint();
    }
#endif
#if ENABLE_ALL_DEMOS
    {
        // create a Hinge2 joint
        // create two rigid bodies
        // static bodyA (parent) on top:
        btTransform tr;
        tr.setIdentity();
        tr.setOrigin(btVector3(btScalar(-20.), btScalar(4.), btScalar(0.)));
        
        std::pair<Ogre::SceneNode*, btRigidBody*> pBodyA =
        createRigidBody(0.0, tr,
                        shape);
        bulletBody.push_back(pBodyA.first);
        
        pBodyA.second->setActivationState(DISABLE_DEACTIVATION);
        // dynamic bodyB (child) below it :
        tr.setIdentity();
        tr.setOrigin(btVector3(btScalar(-20.), btScalar(0.), btScalar(0.)));
        
        std::pair<Ogre::SceneNode*, btRigidBody*> pBodyB =
        createRigidBody(1.0, tr,
                        shape);
        bulletBody.push_back(pBodyB.first);
        pBodyB.second->setActivationState(DISABLE_DEACTIVATION);
        
        // add some data to build constraint frames
        btVector3 parentAxis(0.f, 1.f, 0.f);
        btVector3 childAxis(1.f, 0.f, 0.f);
        btVector3 anchor(-20.f, 0.f, 0.f);
        btHinge2Constraint* pHinge2 = new btHinge2Constraint(*pBodyA.second, *pBodyB.second,
                                                             anchor, parentAxis, childAxis);
        pHinge2->setLowerLimit(-SIMD_HALF_PI * 0.5f);
        pHinge2->setUpperLimit(SIMD_HALF_PI * 0.5f);
        // add constraint to world
        m_dynamicsWorld->addConstraint(pHinge2, true);
        // draw constraint frames and limits for debugging
        pHinge2->setDbgDrawSize(btScalar(5.f));
    }
#endif
#if ENABLE_ALL_DEMOS
    {
        // create a Hinge joint between two dynamic bodies
        // create two rigid bodies
        // static bodyA (parent) on top:
        btTransform tr;
        tr.setIdentity();
        tr.setOrigin(btVector3(btScalar(-20.), btScalar(-2.), btScalar(0.)));
        
        std::pair<Ogre::SceneNode*, btRigidBody*> pBodyA =
        createRigidBody(1.0, tr,
                        shape);
        bulletBody.push_back(pBodyA.first);
        pBodyA.second->setActivationState(DISABLE_DEACTIVATION);
        
        // dynamic bodyB:
        tr.setIdentity();
        tr.setOrigin(btVector3(btScalar(-30.), btScalar(-2.), btScalar(0.)));
        
        std::pair<Ogre::SceneNode*, btRigidBody*> pBodyB =
        createRigidBody(10.0, tr,
                        shape);
        bulletBody.push_back(pBodyB.first);
        pBodyB.second->setActivationState(DISABLE_DEACTIVATION);
        
        // add some data to build constraint frames
        btVector3 axisA(0.f, 1.f, 0.f);
        btVector3 axisB(0.f, 1.f, 0.f);
        btVector3 pivotA(-5.f, 0.f, 0.f);
        btVector3 pivotB(5.f, 0.f, 0.f);
        spHingeDynAB = new btHingeConstraint(*pBodyA.second, *pBodyB.second,
                                             pivotA, pivotB, axisA, axisB);
        spHingeDynAB->setLimit(-SIMD_HALF_PI * 0.5f, SIMD_HALF_PI * 0.5f);
        // add constraint to world
        m_dynamicsWorld->addConstraint(spHingeDynAB, true);
        // draw constraint frames and limits for debugging
        spHingeDynAB->setDbgDrawSize(btScalar(5.f));
    }
#endif
    
#if ENABLE_ALL_DEMOS
    {  // 6DOF connected to the world, with motor
        btTransform tr;
        tr.setIdentity();
        tr.setOrigin(btVector3(btScalar(10.), btScalar(-15.), btScalar(0.)));
        
        std::pair<Ogre::SceneNode*, btRigidBody*> pBody =
        createRigidBody(1.0, tr,
                        shape);
        bulletBody.push_back(pBody.first);
        pBody.second->setActivationState(DISABLE_DEACTIVATION);
        
        btTransform frameB;
        frameB.setIdentity();
        btGeneric6DofConstraint* pGen6Dof = new btGeneric6DofConstraint(*pBody.second,
                                                                        frameB, false);
        m_dynamicsWorld->addConstraint(pGen6Dof);
        pGen6Dof->setDbgDrawSize(btScalar(5.f));
        pGen6Dof->setAngularLowerLimit(btVector3(0, 0, 0));
        pGen6Dof->setAngularUpperLimit(btVector3(0, 0, 0));
        pGen6Dof->setLinearLowerLimit(btVector3(-10., 0, 0));
        pGen6Dof->setLinearUpperLimit(btVector3(10., 0, 0));
        pGen6Dof->getTranslationalLimitMotor()->m_enableMotor[0] = true;
        pGen6Dof->getTranslationalLimitMotor()->m_targetVelocity[0] = 5.0f;
        pGen6Dof->getTranslationalLimitMotor()->m_maxMotorForce[0] = 6.0f;
    }
#endif
}

void MyGameState::createScene01(void)
{
    createPhysicalWorld();
    
    Ogre::SceneManager *sceneManager = mGraphicsSystem->getSceneManager();
    BulletConverter converter(mGraphicsSystem);
    {
        ///create a few basic rigid bodies
        btCollisionShape* groundShape = m_collisionShapes[0];
        converter.createCollisionShapeGraphicsObject(groundShape, "ground1");
        
        Ogre::Item* m_item = sceneManager->createItem("ground1",
                                                      Ogre::ResourceGroupManager::
                                                      AUTODETECT_RESOURCE_GROUP_NAME,
                                                      Ogre::SCENE_DYNAMIC );
        bulletBody[0]->attachObject(m_item);
        
        m_item->setDatablock( "Marble" );
        //Change the addressing mode of the roughness map to wrap via code.
        //Detail maps default to wrap, but the rest to clamp.
        assert( dynamic_cast<Ogre::HlmsPbsDatablock*>( m_item->getSubItem(0)->getDatablock() ) );
        Ogre::HlmsPbsDatablock *datablock
        = static_cast<Ogre::HlmsPbsDatablock*>(
                                               m_item->getSubItem(0)->getDatablock() );
        //Make a hard copy of the sampler block
        Ogre::HlmsSamplerblock samplerblock( *datablock->getSamplerblock( Ogre::PBSM_ROUGHNESS ) );
        samplerblock.mU = Ogre::TAM_WRAP;
        samplerblock.mV = Ogre::TAM_WRAP;
        samplerblock.mW = Ogre::TAM_WRAP;
        //Set the new samplerblock. The Hlms system will
        //automatically create the API block if necessary
        datablock->setSamplerblock( Ogre::PBSM_ROUGHNESS, samplerblock );
    }
    
    ///create a few basic rigid bodies
    converter.createCollisionShapeGraphicsObject(m_collisionShapes[1], "box");
    converter.createCollisionShapeGraphicsObject(m_collisionShapes[2], "door");
    
    {
        Ogre::HlmsManager *hlmsManager = mGraphicsSystem->getRoot()->getHlmsManager();
        assert( dynamic_cast<Ogre::HlmsPbs*>( hlmsManager->getHlms( Ogre::HLMS_PBS ) ) );
        Ogre::HlmsPbs *hlmsPbs = static_cast<Ogre::HlmsPbs*>( hlmsManager->getHlms(Ogre::HLMS_PBS) );
        for (int idx = 1; idx < 22; idx++) {
            Ogre::Item* m_item;
            if (idx == 7) {
                m_item = sceneManager->createItem("door",
                                                  Ogre::ResourceGroupManager::
                                                  AUTODETECT_RESOURCE_GROUP_NAME,
                                                  Ogre::SCENE_DYNAMIC );
            }else{
                m_item = sceneManager->createItem("box",
                                                  Ogre::ResourceGroupManager::
                                                  AUTODETECT_RESOURCE_GROUP_NAME,
                                                  Ogre::SCENE_DYNAMIC );
            }
            bulletBody[idx]->attachObject(m_item);
            m_item->setVisibilityFlags( 0x000000001 );
            
            Ogre::String datablockName = "Test" + Ogre::StringConverter::toString( idx);
            Ogre::HlmsPbsDatablock *datablock
            = static_cast<Ogre::HlmsPbsDatablock*>(
                                                   hlmsPbs->createDatablock(datablockName,
                                                                            datablockName,
                                                                            Ogre::HlmsMacroblock(),
                                                                            Ogre::HlmsBlendblock(),
                                                                            Ogre::HlmsParamVec() ) );
            
            datablock->setDiffuse( Ogre::Vector3( 0.0f, 1.0f, 0.0f ) );
            datablock->setRoughness(0.02f);
            datablock->setFresnel( Ogre::Vector3( 1 ), false );
            datablock->setTransparency( 0.9 );
            m_item->setDatablock( datablock );
        }
    }
    
    Ogre::SceneNode *rootNode = sceneManager->getRootSceneNode();
    
    mDebugNode = sceneManager->getRootSceneNode( Ogre::SCENE_DYNAMIC )->
    createChildSceneNode( Ogre::SCENE_DYNAMIC );
    manual = sceneManager->createManualObject();
    manual->setVisibilityFlags( 0x000000002 );
    mDebugNode->attachObject( manual );
    
    Ogre::HlmsManager *hlmsManager = mGraphicsSystem->getRoot()->getHlmsManager();
    assert( dynamic_cast<Ogre::HlmsUnlit*>( hlmsManager->getHlms( Ogre::HLMS_UNLIT ) ) );
    Ogre::HlmsUnlit *hlmsUnlit = static_cast<Ogre::HlmsUnlit*>( hlmsManager->getHlms(Ogre::HLMS_UNLIT) );
    
    debug = new OgreDebugDrawer(manual, hlmsUnlit);
    m_dynamicsWorld->setDebugDrawer(debug);
    
    Ogre::Light *light = sceneManager->createLight();
    Ogre::SceneNode *lightNode = rootNode->createChildSceneNode();
    lightNode->attachObject( light );
    light->setPowerScale( 1.0f );
    light->setType( Ogre::Light::LT_DIRECTIONAL );
    light->setDirection( Ogre::Vector3( -1, -1, -1 ).normalisedCopy() );
    
    mLightNodes[0] = lightNode;
    
    sceneManager->setAmbientLight( Ogre::ColourValue( 0.3f, 0.5f, 0.7f ) * 0.1f * 0.75f,
                                  Ogre::ColourValue( 0.6f, 0.45f, 0.3f ) * 0.065f * 0.75f,
                                  -light->getDirection() + Ogre::Vector3::UNIT_Y * 0.2f );
    
    light = sceneManager->createLight();
    lightNode = rootNode->createChildSceneNode();
    lightNode->attachObject( light );
    light->setDiffuseColour( 0.8f, 0.4f, 0.2f ); //Warm
    light->setSpecularColour( 0.8f, 0.4f, 0.2f );
    light->setPowerScale( Ogre::Math::PI );
    light->setType( Ogre::Light::LT_SPOTLIGHT );
    lightNode->setPosition( -10.0f, 10.0f, 10.0f );
    light->setDirection( Ogre::Vector3( 1, -1, -1 ).normalisedCopy() );
    light->setAttenuationBasedOnRadius( 10.0f, 0.01f );
    
    mLightNodes[1] = lightNode;
    
    light = sceneManager->createLight();
    lightNode = rootNode->createChildSceneNode();
    lightNode->attachObject( light );
    light->setDiffuseColour( 0.2f, 0.4f, 0.8f ); //Cold
    light->setSpecularColour( 0.2f, 0.4f, 0.8f );
    light->setPowerScale( Ogre::Math::PI );
    light->setType( Ogre::Light::LT_SPOTLIGHT );
    lightNode->setPosition( 10.0f, 10.0f, -10.0f );
    light->setDirection( Ogre::Vector3( -1, -1, 1 ).normalisedCopy() );
    light->setAttenuationBasedOnRadius( 10.0f, 0.01f );
    
    mLightNodes[2] = lightNode;
    
    mCameraController = new CameraController( mGraphicsSystem, false );
    
    TutorialGameState::createScene01();
}
//-----------------------------------------------------------------------------------
void MyGameState::update( float timeSinceLast )
{
    stepSimulation(timeSinceLast);
    
    manual->clear();
    m_dynamicsWorld->debugDrawWorld();
    
    TutorialGameState::update( timeSinceLast );
}

void MyGameState::generateDebugText( float timeSinceLast, Ogre::String &outText )
{
    Ogre::uint32 visibilityMask = mGraphicsSystem->getSceneManager()->getVisibilityMask();
    
    TutorialGameState::generateDebugText( timeSinceLast, outText );
    outText += "\nPress F2 to show/hide animated objects. ";
    outText += (visibilityMask & 0x000000001) ? "[On]" : "[Off]";
    outText += "\nPress F3 to show/hide Decal's debug visualization. ";
    outText += (visibilityMask & 0x000000002) ? "[On]" : "[Off]";
}

//-----------------------------------------------------------------------------------
void MyGameState::keyReleased( const SDL_KeyboardEvent &arg )
{
    if( (arg.keysym.mod & ~(KMOD_NUM|KMOD_CAPS)) != 0 )
    {
        TutorialGameState::keyReleased( arg );
        return;
    }
    
    if( arg.keysym.sym == SDLK_F2 )
    {
        Ogre::uint32 visibilityMask = mGraphicsSystem->getSceneManager()->getVisibilityMask();
        bool showMovingObjects = (visibilityMask & 0x000000001);
        showMovingObjects = !showMovingObjects;
        visibilityMask &= ~0x000000001;
        visibilityMask |= (Ogre::uint32)showMovingObjects;
        mGraphicsSystem->getSceneManager()->setVisibilityMask( visibilityMask );
    }
    else if( arg.keysym.sym == SDLK_F3 )
    {
        Ogre::uint32 visibilityMask = mGraphicsSystem->getSceneManager()->getVisibilityMask();
        bool showMovingObjects = (visibilityMask & 0x000000002);
        showMovingObjects = !showMovingObjects;
        visibilityMask &= ~0x000000002;
        visibilityMask |= (Ogre::uint32)showMovingObjects << 1;
        mGraphicsSystem->getSceneManager()->setVisibilityMask( visibilityMask );
    }
    else
    {
        TutorialGameState::keyReleased( arg );
    }
}

void MyGameState::destroyScene(void){
    int i;

    //removed/delete constraints
    for (i = m_dynamicsWorld->getNumConstraints() - 1; i >= 0; i--)
    {
        btTypedConstraint* constraint = m_dynamicsWorld->getConstraint(i);
        m_dynamicsWorld->removeConstraint(constraint);
        delete constraint;
    }
    m_ctc = NULL;

    //remove the rigidbodies from the dynamics world and delete them
    for (i = m_dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
    {
        btCollisionObject* obj = m_dynamicsWorld->getCollisionObjectArray()[i];
        btRigidBody* body = btRigidBody::upcast(obj);
        if (body && body->getMotionState())
        {
            delete body->getMotionState();
        }
        m_dynamicsWorld->removeCollisionObject(obj);
        delete obj;
    }

    //delete collision shapes
    for (int j = 0; j < m_collisionShapes.size(); j++)
    {
        btCollisionShape* shape = m_collisionShapes[j];
        delete shape;
    }

    m_collisionShapes.clear();

    //delete dynamics world
    delete m_dynamicsWorld;
    m_dynamicsWorld = 0;

    //delete solver
    delete m_solver;
    m_solver = 0;

    //delete broadphase
    delete m_broadphase;
    m_broadphase = 0;

    //delete dispatcher
    delete m_dispatcher;

    delete m_collisionConfiguration;
}
}
