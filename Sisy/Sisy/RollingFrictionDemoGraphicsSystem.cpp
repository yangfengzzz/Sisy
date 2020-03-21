//
//  RollingFrictionDemoGraphicsSystem.cpp
//  Sisy
//
//  Created by 杨丰 on 2020/3/22.
//  Copyright © 2020 杨丰. All rights reserved.
//


#include "GraphicsSystem.h"
#include "RollingFrictionDemoState.hpp"

#include "OgreSceneManager.h"
#include "OgreCamera.h"
#include "OgreRoot.h"
#include "OgreWindow.h"
#include "OgreConfigFile.h"
#include "../../src.gfx/Compositor/OgreCompositorManager2.h"
#include "OgreTextureFilters.h"
#include "OgreTextureGpuManager.h"

//Declares WinMain / main
#include "MainEntryPointHelper.h"
#include "System/MainEntryPoints.h"

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
INT WINAPI WinMainApp( HINSTANCE hInst, HINSTANCE hPrevInstance, LPSTR strCmdLine, INT nCmdShow )
#else
int mainApp( int argc, const char *argv[] )
#endif
{
    return jet::MainEntryPoints::mainAppSingleThreaded( DEMO_MAIN_ENTRY_PARAMS );
}

namespace jet
{
    class MyGraphicsSystem : public jet::GraphicsSystem
    {
        virtual Ogre::CompositorWorkspace* setupCompositor()
        {
            Ogre::CompositorManager2 *compositorManager = mRoot->getCompositorManager2();
            return compositorManager->addWorkspace( mSceneManager, mRenderWindow->getTexture(),
                                                    mCamera, "PbsMaterialsWorkspace", true );
        }

        virtual void setupResources(void)
        {
            jet::GraphicsSystem::setupResources();

            Ogre::ConfigFile cf;
            cf.load(mResourcePath + "resources2.cfg");

            Ogre::String dataFolder = cf.getSetting( "DoNotUseAsResource", "Hlms", "" );

            if( dataFolder.empty() )
                dataFolder = "./";
            else if( *(dataFolder.end() - 1) != '/' )
                dataFolder += "/";

            dataFolder += "2.0/scripts/materials/PbsMaterials";

            addResourceLocation( dataFolder, "FileSystem", "General" );
        }

    public:
        MyGraphicsSystem( jet::GameState *gameState ) :
            jet::GraphicsSystem( gameState )
        {
        }
    };

    void jet::MainEntryPoints::createSystems( jet::GameState **outGraphicsGameState,
                                             jet::GraphicsSystem **outGraphicsSystem,
                                             jet::GameState **outLogicGameState,
                                             jet::LogicSystem **outLogicSystem )
    {
        MyGameState *gfxGameState = new MyGameState(
        ""
        "\n"
        "LEGAL: Uses Saint Peter's Basilica (C) by Emil Persson under CC Attrib 3.0 Unported\n"
        "See Samples/Media/materials/textures/Cubemaps/License.txt for more information." );

        jet::GraphicsSystem *graphicsSystem = new MyGraphicsSystem( gfxGameState );

        gfxGameState->_notifyGraphicsSystem( graphicsSystem );

        *outGraphicsGameState = gfxGameState;
        *outGraphicsSystem = graphicsSystem;
    }

    void jet::MainEntryPoints::destroySystems( GameState *graphicsGameState,
                                              GraphicsSystem *graphicsSystem,
                                              GameState *logicGameState,
                                              LogicSystem *logicSystem )
    {
        delete graphicsSystem;
        delete graphicsGameState;
    }

    const char* jet::MainEntryPoints::getWindowTitle(void)
    {
        return "Screen Space Decals";
    }
}
