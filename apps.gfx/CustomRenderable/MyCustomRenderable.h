
#ifndef _Demo_MyCustomRenderable_H_
#define _Demo_MyCustomRenderable_H_

#include "OgreMovableObject.h"
#include "OgreRenderable.h"

namespace Ogre
{
    class MyCustomRenderable : public MovableObject, public Renderable
    {
        void createBuffers(void);

    public:
        MyCustomRenderable( IdType id, ObjectMemoryManager *objectMemoryManager,
                            SceneManager* manager, uint8 renderQueueId );
        virtual ~MyCustomRenderable();

        //Overrides from MovableObject
        virtual const String& getMovableType(void) const override;

        //Overrides from Renderable
        virtual const LightList& getLights(void) const override;
        virtual void getRenderOperation( v1::RenderOperation& op, bool casterPass ) override;
        virtual void getWorldTransforms( Matrix4* xform ) const override;
        virtual bool getCastsShadows(void) const override;
    };
}

#endif
