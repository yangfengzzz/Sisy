
#include "MyGameState.h"
#include "GraphicsSystem.h"

#include "OgreSceneManager.h"
#include "OgreItem.h"

#include "OgreTextAreaOverlayElement.h"
#include "BulletGUIHelper.h"
using namespace Demo;

#define ARRAY_SIZE_Y 5
#define ARRAY_SIZE_X 5
#define ARRAY_SIZE_Z 5

namespace Demo
{
    MyGameState::MyGameState( const Ogre::String &helpDescription ) :
    TutorialGameState( helpDescription )
    {
    }
    //-----------------------------------------------------------------------------------
    void MyGameState::createScene01(void)
    {
        createEmptyDynamicsWorld();

        ///create a few basic rigid bodies
        btBoxShape* groundShape = createBoxShape(btVector3(btScalar(50.), btScalar(50.), btScalar(50.)));

        //groundShape->initializePolyhedralFeatures();
        //btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(0,1,0),50);

        m_collisionShapes.push_back(groundShape);

        btTransform groundTransform;
        groundTransform.setIdentity();
        groundTransform.setOrigin(btVector3(0, -50, 0));
        
        {
            btScalar mass(0.);
            createRigidBody(mass, groundTransform, groundShape, btVector4(0, 0, 1, 1));
        }

        {
            //create a few dynamic rigidbodies
            // Re-using the same collision is better for memory usage and performance

            btBoxShape* colShape = createBoxShape(btVector3(.1, .1, .1));

            //btCollisionShape* colShape = new btSphereShape(btScalar(1.));
            m_collisionShapes.push_back(colShape);

            /// Create Dynamic Objects
            btTransform startTransform;
            startTransform.setIdentity();

            btScalar mass(1.f);

            //rigidbody is dynamic if and only if mass is non zero, otherwise static
            bool isDynamic = (mass != 0.f);

            btVector3 localInertia(0, 0, 0);
            if (isDynamic)
                colShape->calculateLocalInertia(mass, localInertia);

            for (int k = 0; k < ARRAY_SIZE_Y; k++)
            {
                for (int i = 0; i < ARRAY_SIZE_X; i++)
                {
                    for (int j = 0; j < ARRAY_SIZE_Z; j++)
                    {
                        startTransform.setOrigin(btVector3(
                            btScalar(0.2 * i),
                            btScalar(2 + .2 * k),
                            btScalar(0.2 * j)));

                        createRigidBody(mass, startTransform, colShape);
                    }
                }
            }
        }

        mBulletHelper->autogenerateGraphicsObjects(m_dynamicsWorld);
        
        TutorialGameState::createScene01();
    }
    //-----------------------------------------------------------------------------------
    void MyGameState::update( float timeSinceLast )
    {
        stepSimulation(timeSinceLast);
        
        TutorialGameState::update( timeSinceLast );
    }
}
