#ifndef Bullet_GUI_HELPER_H
#define Bullet_GUI_HELPER_H
//#include "CommonGUIHelperInterface.h"
#include "OgreString.h"
class btCollisionShape;
class btTransform;
class btRigidBody;
class btVector3;
class btCollisionObject;
class btDiscreteDynamicsWorld;
#include "btAlignedObjectArray.h"
#include "InstanceGraphicsShape.h"

namespace Demo{
typedef void (*VisualizerFlagCallback)(int flag, bool enable);

class GraphicsSystem;
struct BulletGuiHelper
{
    struct BulletGuiHelperInternalData* m_data;
    
    BulletGuiHelper(GraphicsSystem *graphicsSystem );
    
    virtual ~BulletGuiHelper();
    
    static int idx;
    
public:
    std::pair<Ogre::MeshPtr, Ogre::VertexBufferPacked*>
    createDynamicMesh(const float* vertices, int numvertices,
                      const Ogre::uint16* indices, int numIndices, int idx);
    
    Ogre::IndexBufferPacked*
    createIndexBuffer(const Ogre::uint16* indices, int numIndices);
    
    virtual int registerGraphicsShape(const float* vertices, int numvertices,
                                      const Ogre::uint16* indices, int numIndices,
                                      const btVector3& pos);
    
    virtual void createCollisionShapeGraphicsObject(btCollisionObject* body);
    
    void computeSoftBodyVertices(btCollisionShape* collisionShape,
                                 btAlignedObjectArray<GLInstanceVertex>& gfxVertices,
                                 btAlignedObjectArray<Ogre::uint16>& indices);
    
    virtual void autogenerateGraphicsObjects(btDiscreteDynamicsWorld* rbWorld);

public:
//    virtual void createRigidBodyGraphicsObject(btRigidBody* body, const btVector3& color);
//
//    virtual void createCollisionObjectGraphicsObject(btCollisionObject* body, const btVector3& color);
//
//    virtual int registerTexture(const unsigned char* texels, int width, int height);
//
//    virtual int registerGraphicsInstance(int shapeIndex, const float* position,
//                                         const float* quaternion, const float* color, const float* scaling);
//    virtual void removeAllGraphicsInstances();
//    virtual void removeGraphicsInstance(int graphicsUid);
//    virtual void changeInstanceFlags(int instanceUid, int flags);
//    virtual void changeRGBAColor(int instanceUid, const double rgbaColor[4]);
//    virtual void changeSpecularColor(int instanceUid, const double specularColor[3]);
//    virtual void changeTexture(int textureUniqueId, const unsigned char* rgbTexels, int width, int height);
//    virtual void removeTexture(int textureUid);
//    virtual int getShapeIndexFromInstance(int instanceUid);
//    virtual void replaceTexture(int shapeIndex, int textureUid);
//    virtual void updateShape(int shapeIndex, float* vertices);
//
//    virtual void syncPhysicsToGraphics(const btDiscreteDynamicsWorld* rbWorld);
//
//    virtual void render(const btDiscreteDynamicsWorld* rbWorld);
//
//    virtual void createPhysicsDebugDrawer(btDiscreteDynamicsWorld* rbWorld);
//
//    virtual struct Common2dCanvasInterface* get2dCanvasInterface();
//
//    virtual CommonParameterInterface* getParameterInterface();
//
//    virtual struct CommonGraphicsApp* getAppInterface();
//
//    virtual void setUpAxis(int axis);
//
//    virtual void resetCamera(float camDist, float yaw, float pitch, float camPosX, float camPosY, float camPosZ);
//    virtual bool getCameraInfo(int* width, int* height,
//                               float viewMatrix[16], float projectionMatrix[16], float camUp[3],
//                               float camForward[3], float hor[3], float vert[3],
//                               float* yaw, float* pitch, float* camDist, float cameraTarget[3]) const;
//
//    virtual void copyCameraImageData(const float viewMatrix[16], const float projectionMatrix[16],
//                                     unsigned char* pixelsRGBA, int rgbaBufferSizeInPixels,
//                                     float* depthBuffer, int depthBufferSizeInPixels,
//                                     int* segmentationMaskBuffer, int segmentationMaskBufferSizeInPixels,
//                                     int startPixelIndex, int destinationWidth,
//                                     int destinationHeight, int* numPixelsCopied);
//
//    virtual void setProjectiveTextureMatrices(const float viewMatrix[16], const float projectionMatrix[16]);
//    virtual void setProjectiveTexture(bool useProjectiveTexture);
//
//    virtual int addUserDebugLine(const double debugLineFromXYZ[3], const double debugLineToXYZ[3],
//                                 const double debugLineColorRGB[3], double lineWidth,
//                                 double lifeTime, int trackingVisualShapeIndex, int replaceItemUid)
//    {
//        return -1;
//    }
//    virtual int addUserDebugParameter(const char* txt, double rangeMin, double rangeMax, double startValue)
//    {
//        return -1;
//    }
//
//    virtual void removeUserDebugItem(int debugItemUniqueId)
//    {
//    }
//    virtual void removeAllUserDebugItems()
//    {
//    }
//
//    void renderInternalGl2(int pass, const btDiscreteDynamicsWorld* dynamicsWorld);
//
//    void setVRMode(bool vrMode);
//
//    void setVisualizerFlag(int flag, int enable);
//
//    virtual void setVisualizerFlagCallback(VisualizerFlagCallback callback);
//
//    virtual void dumpFramesToVideo(const char* mp4FileName);
//
//    int createCheckeredTexture(int r, int g, int b);
};

}

#endif  //OPENGL_GUI_HELPER_H
