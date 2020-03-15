//
//  UseCustomCapabilities_test.cpp
//  GUnitTest
//
//  Created by 杨丰 on 2019/12/1.
//  Copyright © 2019 杨丰. All rights reserved.
//

#include "../src.gfx/OgreRoot.h"
#include "../src.gfx/OgreRenderSystem.h"
#include "../src.gfx/OgreRenderSystemCapabilities.h"
#include "../src.gfx/OgreResourceGroupManager.h"
#include "../src.gfx/OgreMaterialManager.h"
#include "../src.gfx/OgreGpuProgramManager.h"
#include "../src.gfx/OgreHighLevelGpuProgramManager.h"
#include "../external/gtest/include/gtest/gtest.h"

using namespace Ogre;

class UseCustomCapabilitiesTests : public testing::Test {
public:
    void SetUp() override{
        if(HighLevelGpuProgramManager::getSingletonPtr())
            OGRE_DELETE HighLevelGpuProgramManager::getSingletonPtr();
        if(GpuProgramManager::getSingletonPtr())
            OGRE_DELETE GpuProgramManager::getSingletonPtr();
        if(MaterialManager::getSingletonPtr())
            OGRE_DELETE MaterialManager::getSingletonPtr();
        if(ResourceGroupManager::getSingletonPtr())
            OGRE_DELETE ResourceGroupManager::getSingletonPtr();
    }
    
    void checkCaps(const RenderSystemCapabilities* caps)
    {
        ASSERT_EQ(caps->hasCapability(RSC_AUTOMIPMAP), true);
        ASSERT_EQ(caps->hasCapability(RSC_BLENDING), true);
        ASSERT_EQ(caps->hasCapability(RSC_ANISOTROPY), true);
        ASSERT_EQ(caps->hasCapability(RSC_DOT3), true);
        ASSERT_EQ(caps->hasCapability(RSC_CUBEMAPPING), true);
        ASSERT_EQ(caps->hasCapability(RSC_HWSTENCIL), true);
        
        ASSERT_EQ(caps->hasCapability(RSC_VBO), true);
        ASSERT_EQ(caps->hasCapability(RSC_VERTEX_PROGRAM), true);
        ASSERT_EQ(caps->hasCapability(RSC_FRAGMENT_PROGRAM), true);
        ASSERT_EQ(caps->hasCapability(RSC_TWO_SIDED_STENCIL), true);
        ASSERT_EQ(caps->hasCapability(RSC_STENCIL_WRAP), true);
        
        ASSERT_EQ(caps->hasCapability(RSC_HWOCCLUSION), true);
        ASSERT_EQ(caps->hasCapability(RSC_USER_CLIP_PLANES), true);
        ASSERT_EQ(caps->hasCapability(RSC_VERTEX_FORMAT_UBYTE4), true);
        ASSERT_EQ(caps->hasCapability(RSC_INFINITE_FAR_PLANE), true);
        ASSERT_EQ(caps->hasCapability(RSC_HWRENDER_TO_TEXTURE), true);
        ASSERT_EQ(caps->hasCapability(RSC_TEXTURE_FLOAT), true);
        
        ASSERT_EQ(caps->hasCapability(RSC_NON_POWER_OF_2_TEXTURES), false);
        ASSERT_EQ(caps->hasCapability(RSC_TEXTURE_3D), true);
        ASSERT_EQ(caps->hasCapability(RSC_POINT_SPRITES), true);
        ASSERT_EQ(caps->hasCapability(RSC_POINT_EXTENDED_PARAMETERS), true);
        ASSERT_EQ(caps->hasCapability(RSC_VERTEX_TEXTURE_FETCH), false);
        ASSERT_EQ(caps->hasCapability(RSC_MIPMAP_LOD_BIAS), true);
        
        ASSERT_EQ(caps->hasCapability(RSC_TEXTURE_COMPRESSION), true);
        ASSERT_EQ(caps->hasCapability(RSC_TEXTURE_COMPRESSION_DXT), true);
        ASSERT_EQ(caps->hasCapability(RSC_TEXTURE_COMPRESSION_VTC), false);
        ASSERT_EQ(caps->hasCapability(RSC_TEXTURE_COMPRESSION_PVRTC), false);
        ASSERT_EQ(caps->hasCapability(RSC_TEXTURE_COMPRESSION_BC4_BC5), false);
        ASSERT_EQ(caps->hasCapability(RSC_TEXTURE_COMPRESSION_BC6H_BC7), false);
        ASSERT_EQ(caps->hasCapability(RSC_FBO), true);
        ASSERT_EQ(caps->hasCapability(RSC_FBO_ARB), false);
        
        ASSERT_EQ(caps->hasCapability(RSC_FBO_ATI), false);
        ASSERT_EQ(caps->hasCapability(RSC_PBUFFER), false);
        ASSERT_EQ(caps->hasCapability(RSC_PERSTAGECONSTANT), false);
        ASSERT_EQ(caps->hasCapability(RSC_VAO), false);
        ASSERT_EQ(caps->hasCapability(RSC_SEPARATE_SHADER_OBJECTS), false);
        
        ASSERT_EQ(caps->isShaderProfileSupported("arbfp1"), true);
        ASSERT_EQ(caps->isShaderProfileSupported("arbvp1"), true);
        ASSERT_EQ(caps->isShaderProfileSupported("glsl"), true);
        ASSERT_EQ(caps->isShaderProfileSupported("ps_1_1"), true);
        ASSERT_EQ(caps->isShaderProfileSupported("ps_1_2"), true);
        ASSERT_EQ(caps->isShaderProfileSupported("ps_1_3"), true);
        ASSERT_EQ(caps->isShaderProfileSupported("ps_1_4"), true);
        
        ASSERT_EQ(caps->getMaxPointSize(), (Real)1024);
        ASSERT_EQ(caps->getNonPOW2TexturesLimited(), false);
        ASSERT_EQ(caps->getVertexTextureUnitsShared(), true);
        ASSERT_EQ(caps->getNumWorldMatrices(), (Ogre::ushort)0);
        ASSERT_EQ(caps->getNumTextureUnits(), (Ogre::ushort)16);
        ASSERT_EQ(caps->getStencilBufferBitDepth(), (Ogre::ushort)8);
        ASSERT_EQ(caps->getNumVertexBlendMatrices(), (Ogre::ushort)0);
        ASSERT_EQ(caps->getNumMultiRenderTargets(), (Ogre::ushort)4);
        
        ASSERT_EQ(caps->getVertexProgramConstantFloatCount(), (Ogre::ushort)256);
        ASSERT_EQ(caps->getVertexProgramConstantIntCount(), (Ogre::ushort)0);
        ASSERT_EQ(caps->getVertexProgramConstantBoolCount(), (Ogre::ushort)0);
        
        ASSERT_EQ(caps->getFragmentProgramConstantFloatCount(), (Ogre::ushort)64);
        ASSERT_EQ(caps->getFragmentProgramConstantIntCount(), (Ogre::ushort)0);
        ASSERT_EQ(caps->getFragmentProgramConstantBoolCount(), (Ogre::ushort)0);
        
        ASSERT_EQ(caps->getNumVertexTextureUnits(), (Ogre::ushort)0);
        ASSERT_EQ(caps->isShaderProfileSupported("arbvp1"), true);
        ASSERT_EQ(caps->isShaderProfileSupported("arbfp1"), true);
    }
    
public:
    void setUpGLRenderSystemOptions(RenderSystem* rs)
    {
        ConfigOptionMap options = rs->getConfigOptions();
        // set default options
        // this should work on every semi-normal system
        rs->setConfigOption(String("Colour Depth"), String("32"));
        rs->setConfigOption(String("FSAA"), String("0"));
        rs->setConfigOption(String("Full Screen"), String("No"));
        rs->setConfigOption(String("VSync"), String("No"));
        rs->setConfigOption(String("Video Mode"), String("800 x 600"));
        
        // use the best RTT
        ConfigOption optionRTT = options["RTT Preferred Mode"];
        
        if(find(optionRTT.possibleValues.begin(), optionRTT.possibleValues.end(), "FBO") != optionRTT.possibleValues.end())
        {
            rs->setConfigOption(String("RTT Preferred Mode"), String("FBO"));
        }
        else if(find(optionRTT.possibleValues.begin(), optionRTT.possibleValues.end(), "PBuffer") != optionRTT.possibleValues.end())
        {
            rs->setConfigOption(String("RTT Preferred Mode"), String("PBuffer"));
        }
        else
            rs->setConfigOption(String("RTT Preferred Mode"), String("Copy"));
    }
    
    void setUpD3D9RenderSystemOptions(RenderSystem* rs)
    {
        ConfigOptionMap options = rs->getConfigOptions();
        // set default options
        // this should work on every semi-normal system
        rs->setConfigOption(String("Anti aliasing"), String("None"));
        rs->setConfigOption(String("Full Screen"), String("No"));
        rs->setConfigOption(String("VSync"), String("No"));
        rs->setConfigOption(String("Video Mode"), String("800 x 600 @ 32-bit colour"));
        
        // pick first available device
        ConfigOption optionDevice = options["Rendering Device"];
        
        rs->setConfigOption(optionDevice.name, optionDevice.currentValue);
    }
};

TEST_F(UseCustomCapabilitiesTests, testCustomCapabilitiesMetal){
    Root* root = OGRE_NEW Root("../../../../../../Tests/plugins.cfg");

    RenderSystem* rs = root->getRenderSystemByName("Metal Rendering Subsystem");
    if(rs == 0)
    {
        std::cout<<"This test is irrelevant because Metal RenderSystem is not available\n";
    }
    else
    {
        try {
            setUpGLRenderSystemOptions(rs);
            root->setRenderSystem(rs);

            root->initialise(true, "OGRE testCustomCapabilitiesMetal Window",
                             "../../../../../../Tests/Media/CustomCapabilities/customCapabilitiesTest.cfg");

            const RenderSystemCapabilities* caps = rs->getCapabilities();

            checkCaps(caps);
        }
        // clean up root, in case of error, and let cppunit to handle the exception
        catch(...)
        {
        }
    }
    OGRE_DELETE root;
}

TEST_F(UseCustomCapabilitiesTests, testCustomCapabilitiesD3D9){
    Root* root = OGRE_NEW Root("../../../../../../Tests/plugins.cfg");
    
    RenderSystem* rs = root->getRenderSystemByName("Direct3D9 Rendering Subsystem");
    if(rs == 0)
    {
        std::cout<< "This test is irrelevant because D3D9 RenderSystem is not available\n";
    }
    else
    {
        try {
            setUpD3D9RenderSystemOptions(rs);
            root->setRenderSystem(rs);
            root->initialise(true, "OGRE testCustomCapabilitiesD3D9 Window",
                             "../../../../../../Tests/Media/CustomCapabilities/customCapabilitiesTest.cfg");

            const RenderSystemCapabilities* caps = rs->getCapabilities();

            checkCaps(caps);
        }
        // clean up root, in case of error, and let cppunit to handle the exception
        catch(...)
        {
        }
    }

    OGRE_DELETE root;
}
