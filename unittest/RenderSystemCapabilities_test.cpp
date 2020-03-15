//
//  RenderSystemCapabilities_test.cpp
//  GUnitTest
//
//  Created by 杨丰 on 2019/12/1.
//  Copyright © 2019 杨丰. All rights reserved.
//

#include "../src.gfx/OgreLogManager.h"
#include "../src.gfx/FileStreamDataStream.hpp"
#include "../src.gfx/OgreFileSystem.h"
#include "../src.gfx/OgreArchiveManager.h"
#include "../src.gfx/OgreRenderSystemCapabilities.h"
#include "../src.gfx/OgreRenderSystemCapabilitiesSerializer.h"
#include "../src.gfx/OgreRenderSystemCapabilitiesManager.h"
#include "../external/gtest/include/gtest/gtest.h"

using namespace Ogre;

class RenderSystemCapabilitiesTests : public testing::Test {
public:
    void SetUp() override{
        mLogManager = OGRE_NEW LogManager;
        
        // We need to be able to create FileSystem archives to load .rendercaps
        mFileSystemArchiveFactory = OGRE_NEW FileSystemArchiveFactory();
        
        mArchiveManager = OGRE_NEW ArchiveManager();
        ArchiveManager::getSingleton().addArchiveFactory(mFileSystemArchiveFactory);
        
        mRenderSystemCapabilitiesManager = OGRE_NEW RenderSystemCapabilitiesManager();
        
        // Actual parsing happens here. The following test methods confirm parse results only.
        mRenderSystemCapabilitiesManager->parseCapabilitiesFromArchive("../../../../../../Tests/Media/CustomCapabilities",
                                                                       "FileSystem", true);
    }
    
    void TearDown() override{
        OGRE_DELETE mLogManager;
        OGRE_DELETE mRenderSystemCapabilitiesManager;
        OGRE_DELETE mArchiveManager;
        OGRE_DELETE mFileSystemArchiveFactory;
    }
    
    LogManager* mLogManager;
    // For serializing .rendercaps we need RSCManager
    RenderSystemCapabilitiesManager* mRenderSystemCapabilitiesManager;
    // Need these for loading .rendercaps from the file system
    ArchiveManager* mArchiveManager;
    FileSystemArchiveFactory* mFileSystemArchiveFactory;
};

TEST_F(RenderSystemCapabilitiesTests, testIsShaderProfileSupported){
    // create a new RSC
    RenderSystemCapabilities rsc;
    
    // check that no shader profile is supported
    ASSERT_EQ(!rsc.isShaderProfileSupported("vs_1"), true);
    ASSERT_EQ(!rsc.isShaderProfileSupported("ps_1_1"), true);
    ASSERT_EQ(!rsc.isShaderProfileSupported("fp1"), true);
    
    rsc.addShaderProfile("vs_1");
    rsc.addShaderProfile("fp1");
    
    // check that the added shader profiles are supported
    ASSERT_EQ(rsc.isShaderProfileSupported("vs_1"), true);
    ASSERT_EQ(rsc.isShaderProfileSupported("fp1"), true);
    
    
    // check that non added profile is not supported
    ASSERT_EQ(!rsc.isShaderProfileSupported("ps_1_1"), true);
    
    
    // check that empty string is not supported
    ASSERT_EQ(!rsc.isShaderProfileSupported(""), true);
}

TEST_F(RenderSystemCapabilitiesTests, testHasCapability){
    RenderSystemCapabilities rsc;
    
    // check that no caps (from 2 categories) are supported
    ASSERT_EQ(!rsc.hasCapability(RSC_AUTOMIPMAP), true);
    ASSERT_EQ(!rsc.hasCapability(RSC_BLENDING), true);
    ASSERT_EQ(!rsc.hasCapability(RSC_FRAGMENT_PROGRAM), true);
    ASSERT_EQ(!rsc.hasCapability(RSC_TWO_SIDED_STENCIL), true);
    ASSERT_EQ(!rsc.hasCapability(RSC_MIPMAP_LOD_BIAS), true);
    ASSERT_EQ(!rsc.hasCapability(RSC_TEXTURE_COMPRESSION), true);
    ASSERT_EQ(!rsc.hasCapability(RSC_TEXTURE_COMPRESSION_VTC), true);
    ASSERT_EQ(!rsc.hasCapability(RSC_FBO_ATI), true);
    ASSERT_EQ(!rsc.hasCapability(RSC_PBUFFER), true);
    
    // add support for few caps from each category
    rsc.setCapability(RSC_AUTOMIPMAP);
    rsc.setCapability(RSC_FRAGMENT_PROGRAM);
    rsc.setCapability(RSC_TEXTURE_COMPRESSION);
    rsc.setCapability(RSC_FBO_ATI);
    
    // check that the newly set caps are supported
    ASSERT_EQ(rsc.hasCapability(RSC_AUTOMIPMAP), true);
    ASSERT_EQ(rsc.hasCapability(RSC_FRAGMENT_PROGRAM), true);
    ASSERT_EQ(rsc.hasCapability(RSC_TEXTURE_COMPRESSION), true);
    ASSERT_EQ(rsc.hasCapability(RSC_FBO_ATI), true);
    
    // check that the non-set caps are NOT supported
    ASSERT_EQ(!rsc.hasCapability(RSC_BLENDING), true);
    ASSERT_EQ(!rsc.hasCapability(RSC_TWO_SIDED_STENCIL), true);
    ASSERT_EQ(!rsc.hasCapability(RSC_MIPMAP_LOD_BIAS), true);
    ASSERT_EQ(!rsc.hasCapability(RSC_TEXTURE_COMPRESSION_VTC), true);
    ASSERT_EQ(!rsc.hasCapability(RSC_PBUFFER), true);
}

TEST_F(RenderSystemCapabilitiesTests, testSerializeBlank){
    RenderSystemCapabilitiesManager* rscManager = RenderSystemCapabilitiesManager::getSingletonPtr();
    
    RenderSystemCapabilities* rsc = rscManager->loadParsedCapabilities("TestCaps Blank");
    
    // if we have a non-NULL it's good enough
    ASSERT_EQ(rsc != 0, true);
}

TEST_F(RenderSystemCapabilitiesTests, testSerializeEnumCapability){
    RenderSystemCapabilitiesManager* rscManager = RenderSystemCapabilitiesManager::getSingletonPtr();
    
    RenderSystemCapabilities* rsc = rscManager->loadParsedCapabilities("TestCaps enum Capabilities");
    
    // confirm that RSC was loaded
    ASSERT_EQ(rsc != 0, true);
    
    // confirm that the contents are the same as in .rendercaps file
    ASSERT_EQ(rsc->hasCapability(RSC_AUTOMIPMAP), true);
    ASSERT_EQ(rsc->hasCapability(RSC_FBO_ARB), true);
}

TEST_F(RenderSystemCapabilitiesTests, testSerializeStringCapability){
    RenderSystemCapabilitiesManager* rscManager = RenderSystemCapabilitiesManager::getSingletonPtr();
    
    RenderSystemCapabilities* rsc = rscManager->loadParsedCapabilities("TestCaps set String");
    
    // confirm that RSC was loaded
    ASSERT_EQ(rsc != 0, true);
    
    ASSERT_EQ(rsc->isShaderProfileSupported("vs99"), true);
}

TEST_F(RenderSystemCapabilitiesTests, testSerializeBoolCapability){
    RenderSystemCapabilitiesManager* rscManager = RenderSystemCapabilitiesManager::getSingletonPtr();
    
    RenderSystemCapabilities* rscTrue = rscManager->loadParsedCapabilities("TestCaps set bool (true)");
    RenderSystemCapabilities* rscFalse = rscManager->loadParsedCapabilities("TestCaps set bool (false)");
    
    // confirm that RSC was loaded
    ASSERT_EQ(rscTrue != 0, true);
    ASSERT_EQ(rscFalse != 0, true);
    
    ASSERT_EQ(rscTrue->getVertexTextureUnitsShared(), true);
    ASSERT_EQ(rscFalse->getVertexTextureUnitsShared(), false);
}

TEST_F(RenderSystemCapabilitiesTests, testSerializeIntCapability){
    RenderSystemCapabilitiesManager* rscManager = RenderSystemCapabilitiesManager::getSingletonPtr();
    
    RenderSystemCapabilities* rsc = rscManager->loadParsedCapabilities("TestCaps set int");
    
    // confirm that RSC was loaded
    ASSERT_EQ(rsc != 0, true);
    
    // TODO: why no get?
    ASSERT_EQ(rsc->getNumMultiRenderTargets(), 99);
}

TEST_F(RenderSystemCapabilitiesTests, testSerializeRealCapability){
    RenderSystemCapabilitiesManager* rscManager = RenderSystemCapabilitiesManager::getSingletonPtr();
    
    RenderSystemCapabilities* rsc = rscManager->loadParsedCapabilities("TestCaps set Real");
    
    // confirm that RSC was loaded
    ASSERT_EQ(rsc != 0, true);
    
    ASSERT_EQ(rsc->getMaxPointSize(), 99.5);
}

TEST_F(RenderSystemCapabilitiesTests, testSerializeShaderCapability){
    RenderSystemCapabilitiesManager* rscManager = RenderSystemCapabilitiesManager::getSingletonPtr();
    
    RenderSystemCapabilities* rsc = rscManager->loadParsedCapabilities("TestCaps addShaderProfile");
    
    // confirm that RSC was loaded
    ASSERT_EQ(rsc != 0, true);
    
    ASSERT_EQ(rsc->isShaderProfileSupported("vp1"), true);
    ASSERT_EQ(rsc->isShaderProfileSupported("vs_1_1"), true);
    ASSERT_EQ(rsc->isShaderProfileSupported("ps_99"), true);
}

TEST_F(RenderSystemCapabilitiesTests, testWriteSimpleCapabilities){
    String name = "simple caps";
    String filename = "simpleCapsTest.rendercaps";
    
    // set up caps of every type
    RenderSystemCapabilitiesSerializer serializer;
    RenderSystemCapabilities caps;
    caps.setCapability(RSC_AUTOMIPMAP);
    caps.setMaxPointSize(10.5);
    caps.addShaderProfile("vs999");
    caps.addShaderProfile("sp999");
    caps.setVertexTextureUnitsShared(true);
    caps.setNumWorldMatrices(777);
    
    // write them to file
    serializer.writeScript(&caps, name, filename);
    
    // read them back
    std::ifstream capsfile(filename.c_str());
    char buff[255];
    
    capsfile.getline(buff, 255);
    ASSERT_EQ(String("render_system_capabilities \"") + name + "\"", String(buff));
    
    capsfile.getline(buff, 255);
    ASSERT_EQ(String("{"), String(buff));
    
    // scan every line and find the set capabilities it them
    std::vector <String> lines;
    while(capsfile.good())
    {
        capsfile.getline(buff, 255);
        lines.push_back(String(buff));
    }
    
    // check that the file is closed nicely
    String closeBracket = *(lines.end() - 2);
    ASSERT_EQ(String("}"), closeBracket);
    ASSERT_EQ(String(""), lines.back());
    
    // check that all the set caps are there
    ASSERT_EQ(find(lines.begin(), lines.end(), "\tautomipmap true") != lines.end(), true);
    ASSERT_EQ(find(lines.begin(), lines.end(), "\tmax_point_size 10.5") != lines.end(), true);
    ASSERT_EQ(find(lines.begin(), lines.end(), "\tshader_profile sp999") != lines.end(), true);
    ASSERT_EQ(find(lines.begin(), lines.end(), "\tvertex_texture_units_shared true") != lines.end(), true);
    ASSERT_EQ(find(lines.begin(), lines.end(), "\tnum_world_matrices 777") != lines.end(), true);
}

TEST_F(RenderSystemCapabilitiesTests, testWriteAllFalseCapabilities){
    String name = "all false caps";
    String filename = "allFalseCapsTest.rendercaps";
    
    // set up caps of every type
    RenderSystemCapabilitiesSerializer serializer;
    RenderSystemCapabilities caps;
    
    // all caps are false by default
    caps.setVertexTextureUnitsShared(false);
    
    // write them to file
    serializer.writeScript(&caps, name, filename);
    
    // read them back
    std::ifstream capsfile(filename.c_str());
    char buff[255];
    
    capsfile.getline(buff, 255);
    ASSERT_EQ(String("render_system_capabilities \"") + name + "\"", String(buff));
    
    capsfile.getline(buff, 255);
    ASSERT_EQ(String("{"), String(buff));
    
    // scan every line and find the set capabilities it them
    std::vector <String> lines;
    while(capsfile.good())
    {
        capsfile.getline(buff, 255);
        lines.push_back(String(buff));
    }
    
    // check that the file is closed nicely
    String closeBracket = *(lines.end() - 2);
    ASSERT_EQ(String("}"), closeBracket);
    ASSERT_EQ(String(""), lines.back());
    
    // confirm every caps
    ASSERT_EQ(find(lines.begin(), lines.end(), "\tautomipmap false") != lines.end(), true);
    ASSERT_EQ(find(lines.begin(), lines.end(), "\tblending false") != lines.end(), true);
    ASSERT_EQ(find(lines.begin(), lines.end(), "\tanisotropy false") != lines.end(), true);
    ASSERT_EQ(find(lines.begin(), lines.end(), "\tdot3 false") != lines.end(), true);
    ASSERT_EQ(find(lines.begin(), lines.end(), "\tcubemapping false") != lines.end(), true);
    ASSERT_EQ(find(lines.begin(), lines.end(), "\thwstencil false") != lines.end(), true);
    
    ASSERT_EQ(find(lines.begin(), lines.end(), "\tvbo false") != lines.end(), true);
    ASSERT_EQ(find(lines.begin(), lines.end(), "\tvertex_program false") != lines.end(), true);
    ASSERT_EQ(find(lines.begin(), lines.end(), "\tfragment_program false") != lines.end(), true);
    ASSERT_EQ(find(lines.begin(), lines.end(), "\ttwo_sided_stencil false") != lines.end(), true);
    ASSERT_EQ(find(lines.begin(), lines.end(), "\tstencil_wrap false") != lines.end(), true);
    
    ASSERT_EQ(find(lines.begin(), lines.end(), "\thwocclusion false") != lines.end(), true);
    ASSERT_EQ(find(lines.begin(), lines.end(), "\tuser_clip_planes false") != lines.end(), true);
    ASSERT_EQ(find(lines.begin(), lines.end(), "\tvertex_format_ubyte4 false") != lines.end(), true);
    ASSERT_EQ(find(lines.begin(), lines.end(), "\tinfinite_far_plane false") != lines.end(), true);
    ASSERT_EQ(find(lines.begin(), lines.end(), "\thwrender_to_texture false") != lines.end(), true);
    ASSERT_EQ(find(lines.begin(), lines.end(), "\ttexture_float false") != lines.end(), true);
    
    ASSERT_EQ(find(lines.begin(), lines.end(), "\tnon_power_of_2_textures false") != lines.end(), true);
    ASSERT_EQ(find(lines.begin(), lines.end(), "\ttexture_3d false") != lines.end(), true);
    ASSERT_EQ(find(lines.begin(), lines.end(), "\tpoint_sprites false") != lines.end(), true);
    ASSERT_EQ(find(lines.begin(), lines.end(), "\tpoint_extended_parameters false") != lines.end(), true);
    ASSERT_EQ(find(lines.begin(), lines.end(), "\tvertex_texture_fetch false") != lines.end(), true);
    ASSERT_EQ(find(lines.begin(), lines.end(), "\tmipmap_lod_bias false") != lines.end(), true);
    
    ASSERT_EQ(find(lines.begin(), lines.end(), "\ttexture_compression false") != lines.end(), true);
    ASSERT_EQ(find(lines.begin(), lines.end(), "\ttexture_compression_dxt false") != lines.end(), true);
    ASSERT_EQ(find(lines.begin(), lines.end(), "\ttexture_compression_vtc false") != lines.end(), true);
    ASSERT_EQ(find(lines.begin(), lines.end(), "\ttexture_compression_pvrtc false") != lines.end(), true);
    ASSERT_EQ(find(lines.begin(), lines.end(), "\ttexture_compression_bc4_bc5 false") != lines.end(), true);
    ASSERT_EQ(find(lines.begin(), lines.end(), "\ttexture_compression_bc6h_bc7 false") != lines.end(), true);
    ASSERT_EQ(find(lines.begin(), lines.end(), "\tfbo false") != lines.end(), true);
    ASSERT_EQ(find(lines.begin(), lines.end(), "\tfbo_arb false") != lines.end(), true);
    
    ASSERT_EQ(find(lines.begin(), lines.end(), "\tfbo_ati false") != lines.end(), true);
    ASSERT_EQ(find(lines.begin(), lines.end(), "\tpbuffer false") != lines.end(), true);
    ASSERT_EQ(find(lines.begin(), lines.end(), "\tperstageconstant false") != lines.end(), true);
    ASSERT_EQ(find(lines.begin(), lines.end(), "\tseparate_shader_objects false") != lines.end(), true);
    ASSERT_EQ(find(lines.begin(), lines.end(), "\tvao false") != lines.end(), true);
    
    // bool caps
    ASSERT_EQ(find(lines.begin(), lines.end(), "\tvertex_texture_units_shared false") != lines.end(), true);
}

TEST_F(RenderSystemCapabilitiesTests, testWriteAllTrueCapabilities){
    String name = "all false caps";
    String filename = "allFalseCapsTest.rendercaps";
    
    // set up caps of every type
    RenderSystemCapabilitiesSerializer serializer;
    RenderSystemCapabilities caps;
    
    // set all caps
    caps.setVertexTextureUnitsShared(true);
    
    caps.setCapability(RSC_AUTOMIPMAP);
    caps.setCapability(RSC_BLENDING);
    caps.setCapability(RSC_ANISOTROPY);
    caps.setCapability(RSC_DOT3);
    caps.setCapability(RSC_CUBEMAPPING);
    caps.setCapability(RSC_HWSTENCIL);
    
    caps.setCapability(RSC_VBO);
    caps.setCapability(RSC_VERTEX_PROGRAM);
    caps.setCapability(RSC_FRAGMENT_PROGRAM);
    caps.setCapability(RSC_TWO_SIDED_STENCIL);
    caps.setCapability(RSC_STENCIL_WRAP);
    
    caps.setCapability(RSC_HWOCCLUSION);
    caps.setCapability(RSC_USER_CLIP_PLANES);
    caps.setCapability(RSC_VERTEX_FORMAT_UBYTE4);
    caps.setCapability(RSC_INFINITE_FAR_PLANE);
    caps.setCapability(RSC_HWRENDER_TO_TEXTURE);
    caps.setCapability(RSC_TEXTURE_FLOAT);
    
    caps.setCapability(RSC_NON_POWER_OF_2_TEXTURES);
    caps.setCapability(RSC_TEXTURE_3D);
    caps.setCapability(RSC_POINT_SPRITES);
    caps.setCapability(RSC_POINT_EXTENDED_PARAMETERS);
    caps.setCapability(RSC_VERTEX_TEXTURE_FETCH);
    caps.setCapability(RSC_MIPMAP_LOD_BIAS);
    
    caps.setCapability(RSC_TEXTURE_COMPRESSION);
    caps.setCapability(RSC_TEXTURE_COMPRESSION_DXT);
    caps.setCapability(RSC_TEXTURE_COMPRESSION_VTC);
    caps.setCapability(RSC_TEXTURE_COMPRESSION_PVRTC);
    caps.setCapability(RSC_TEXTURE_COMPRESSION_BC4_BC5);
    caps.setCapability(RSC_TEXTURE_COMPRESSION_BC6H_BC7);
    caps.setCapability(RSC_FBO);
    caps.setCapability(RSC_FBO_ARB);
    
    caps.setCapability(RSC_FBO_ATI);
    caps.setCapability(RSC_PBUFFER);
    caps.setCapability(RSC_PERSTAGECONSTANT);
    caps.setCapability(RSC_SEPARATE_SHADER_OBJECTS);
    caps.setCapability(RSC_VAO);
    
    // write them to file
    serializer.writeScript(&caps, name, filename);
    
    // read them back
    std::ifstream capsfile(filename.c_str());
    char buff[255];
    
    capsfile.getline(buff, 255);
    ASSERT_EQ(String("render_system_capabilities \"") + name + "\"", String(buff));
    
    capsfile.getline(buff, 255);
    ASSERT_EQ(String("{"), String(buff));
    
    // scan every line and find the set capabilities it them
    std::vector <String> lines;
    while(capsfile.good())
    {
        capsfile.getline(buff, 255);
        lines.push_back(String(buff));
    }
    
    // check that the file is closed nicely
    String closeBracket = *(lines.end() - 2);
    ASSERT_EQ(String("}"), closeBracket);
    ASSERT_EQ(String(""), lines.back());
    
    // confirm all caps
    ASSERT_EQ(find(lines.begin(), lines.end(), "\tautomipmap true") != lines.end(), true);
    ASSERT_EQ(find(lines.begin(), lines.end(), "\tblending true") != lines.end(), true);
    ASSERT_EQ(find(lines.begin(), lines.end(), "\tanisotropy true") != lines.end(), true);
    ASSERT_EQ(find(lines.begin(), lines.end(), "\tdot3 true") != lines.end(), true);
    ASSERT_EQ(find(lines.begin(), lines.end(), "\tcubemapping true") != lines.end(), true);
    ASSERT_EQ(find(lines.begin(), lines.end(), "\thwstencil true") != lines.end(), true);
    
    ASSERT_EQ(find(lines.begin(), lines.end(), "\tvbo true") != lines.end(), true);
    ASSERT_EQ(find(lines.begin(), lines.end(), "\tvertex_program true") != lines.end(), true);
    ASSERT_EQ(find(lines.begin(), lines.end(), "\tfragment_program true") != lines.end(), true);
    ASSERT_EQ(find(lines.begin(), lines.end(), "\ttwo_sided_stencil true") != lines.end(), true);
    ASSERT_EQ(find(lines.begin(), lines.end(), "\tstencil_wrap true") != lines.end(), true);
    
    ASSERT_EQ(find(lines.begin(), lines.end(), "\thwocclusion true") != lines.end(), true);
    ASSERT_EQ(find(lines.begin(), lines.end(), "\tuser_clip_planes true") != lines.end(), true);
    ASSERT_EQ(find(lines.begin(), lines.end(), "\tvertex_format_ubyte4 true") != lines.end(), true);
    ASSERT_EQ(find(lines.begin(), lines.end(), "\tinfinite_far_plane true") != lines.end(), true);
    ASSERT_EQ(find(lines.begin(), lines.end(), "\thwrender_to_texture true") != lines.end(), true);
    ASSERT_EQ(find(lines.begin(), lines.end(), "\ttexture_float true") != lines.end(), true);
    
    ASSERT_EQ(find(lines.begin(), lines.end(), "\tnon_power_of_2_textures true") != lines.end(), true);
    ASSERT_EQ(find(lines.begin(), lines.end(), "\ttexture_3d true") != lines.end(), true);
    ASSERT_EQ(find(lines.begin(), lines.end(), "\tpoint_sprites true") != lines.end(), true);
    ASSERT_EQ(find(lines.begin(), lines.end(), "\tpoint_extended_parameters true") != lines.end(), true);
    ASSERT_EQ(find(lines.begin(), lines.end(), "\tvertex_texture_fetch true") != lines.end(), true);
    ASSERT_EQ(find(lines.begin(), lines.end(), "\tmipmap_lod_bias true") != lines.end(), true);
    
    ASSERT_EQ(find(lines.begin(), lines.end(), "\ttexture_compression true") != lines.end(), true);
    ASSERT_EQ(find(lines.begin(), lines.end(), "\ttexture_compression_dxt true") != lines.end(), true);
    ASSERT_EQ(find(lines.begin(), lines.end(), "\ttexture_compression_vtc true") != lines.end(), true);
    ASSERT_EQ(find(lines.begin(), lines.end(), "\ttexture_compression_pvrtc true") != lines.end(), true);
    ASSERT_EQ(find(lines.begin(), lines.end(), "\ttexture_compression_bc4_bc5 true") != lines.end(), true);
    ASSERT_EQ(find(lines.begin(), lines.end(), "\ttexture_compression_bc6h_bc7 true") != lines.end(), true);
    ASSERT_EQ(find(lines.begin(), lines.end(), "\tfbo true") != lines.end(), true);
    ASSERT_EQ(find(lines.begin(), lines.end(), "\tfbo_arb true") != lines.end(), true);
    
    ASSERT_EQ(find(lines.begin(), lines.end(), "\tfbo_ati true") != lines.end(), true);
    ASSERT_EQ(find(lines.begin(), lines.end(), "\tpbuffer true") != lines.end(), true);
    ASSERT_EQ(find(lines.begin(), lines.end(), "\tperstageconstant true") != lines.end(), true);
    ASSERT_EQ(find(lines.begin(), lines.end(), "\tseparate_shader_objects true") != lines.end(), true);
    ASSERT_EQ(find(lines.begin(), lines.end(), "\tvao true") != lines.end(), true);
    
    // bool caps
    ASSERT_EQ(find(lines.begin(), lines.end(), "\tvertex_texture_units_shared true") != lines.end(), true);
}

TEST_F(RenderSystemCapabilitiesTests, testWriteAndReadComplexCapabilities){
    String name = "complex caps";
    String filename = "complexCapsTest.rendercaps";

    // set up caps of every type
    RenderSystemCapabilitiesSerializer serializer;
    RenderSystemCapabilities caps;

    // set all caps
    caps.setVertexTextureUnitsShared(true);

    caps.setCapability(RSC_AUTOMIPMAP);
    caps.setCapability(RSC_DOT3);
    caps.setCapability(RSC_CUBEMAPPING);
    caps.setCapability(RSC_HWSTENCIL);
    caps.setCapability(RSC_VBO);
    caps.setCapability(RSC_FRAGMENT_PROGRAM);
    caps.setCapability(RSC_TWO_SIDED_STENCIL);
    caps.setCapability(RSC_HWOCCLUSION);
    caps.setCapability(RSC_VERTEX_FORMAT_UBYTE4);
    caps.setCapability(RSC_HWRENDER_TO_TEXTURE);
    caps.setCapability(RSC_TEXTURE_FLOAT);
    caps.setCapability(RSC_NON_POWER_OF_2_TEXTURES);
    caps.setCapability(RSC_TEXTURE_3D);
    caps.setCapability(RSC_POINT_EXTENDED_PARAMETERS);
    caps.setCapability(RSC_MIPMAP_LOD_BIAS);
    caps.setCapability(RSC_TEXTURE_COMPRESSION);
    caps.setCapability(RSC_TEXTURE_COMPRESSION_DXT);
    caps.setCapability(RSC_TEXTURE_COMPRESSION_VTC);
    caps.setCapability(RSC_TEXTURE_COMPRESSION_PVRTC);
    caps.setCapability(RSC_TEXTURE_COMPRESSION_BC4_BC5);
    caps.setCapability(RSC_TEXTURE_COMPRESSION_BC6H_BC7);
    caps.setCapability(RSC_PERSTAGECONSTANT);
    caps.setCapability(RSC_SEPARATE_SHADER_OBJECTS);
    caps.setCapability(RSC_VAO);

    caps.setNumWorldMatrices(11);
    caps.setNumTextureUnits(22);
    caps.setStencilBufferBitDepth(20001);
    caps.setNumVertexBlendMatrices(33);
    caps.setNumMultiRenderTargets(23);

    caps.addShaderProfile("99foo100");

    // try out stranger names
    caps.addShaderProfile("..f(_)specialsymbolextravaganza!@#$%^&*_but_no_spaces");

    caps.setVertexProgramConstantFloatCount(1111);
    caps.setVertexProgramConstantIntCount(2222);
    caps.setVertexProgramConstantBoolCount(3333);

    caps.setFragmentProgramConstantFloatCount(4444);
    caps.setFragmentProgramConstantIntCount(5555);
    caps.setFragmentProgramConstantBoolCount(64000);

    caps.setMaxPointSize(123.75);
    caps.setNonPOW2TexturesLimited(true);
    caps.setVertexTextureUnitsShared(true);

    DriverVersion driverversion;
    driverversion.major = 11;
    driverversion.minor = 13;
    driverversion.release = 17;
    driverversion.build = 0;

    caps.setDriverVersion(driverversion);
    caps.setDeviceName("Dummy Device");
    caps.setRenderSystemName("Dummy RenderSystem");

    // write them to file
    serializer.writeScript(&caps, name, filename);

    FileStreamDataStream* fdatastream = new FileStreamDataStream(filename,
            OGRE_NEW_T(std::ifstream, MEMCATEGORY_GENERAL)(filename.c_str()));

    DataStreamPtr dataStreamPtr(fdatastream);

    // parsing does not return a raw RSC, but adds it to the Manager
    serializer.parseScript(dataStreamPtr);

    RenderSystemCapabilitiesManager* rscManager = RenderSystemCapabilitiesManager::getSingletonPtr();

    RenderSystemCapabilities* rsc = rscManager->loadParsedCapabilities(name);
    // confirm that RSC was loaded
    ASSERT_EQ(rsc != 0, true);

    // create a reference, so that were are working with two refs
    RenderSystemCapabilities& caps2 = *rsc;

    ASSERT_EQ(caps.hasCapability(RSC_AUTOMIPMAP), caps2.hasCapability(RSC_AUTOMIPMAP));
    ASSERT_EQ(caps.hasCapability(RSC_BLENDING), caps2.hasCapability(RSC_BLENDING));
    ASSERT_EQ(caps.hasCapability(RSC_ANISOTROPY), caps2.hasCapability(RSC_ANISOTROPY));
    ASSERT_EQ(caps.hasCapability(RSC_DOT3), caps2.hasCapability(RSC_DOT3));
    ASSERT_EQ(caps.hasCapability(RSC_CUBEMAPPING), caps2.hasCapability(RSC_CUBEMAPPING));
    ASSERT_EQ(caps.hasCapability(RSC_HWSTENCIL), caps2.hasCapability(RSC_HWSTENCIL));

    ASSERT_EQ(caps.hasCapability(RSC_VBO), caps2.hasCapability(RSC_VBO));
    ASSERT_EQ(caps.hasCapability(RSC_VERTEX_PROGRAM), caps2.hasCapability(RSC_VERTEX_PROGRAM));
    ASSERT_EQ(caps.hasCapability(RSC_FRAGMENT_PROGRAM), caps2.hasCapability(RSC_FRAGMENT_PROGRAM));
    ASSERT_EQ(caps.hasCapability(RSC_TWO_SIDED_STENCIL), caps2.hasCapability(RSC_TWO_SIDED_STENCIL));
    ASSERT_EQ(caps.hasCapability(RSC_STENCIL_WRAP), caps2.hasCapability(RSC_STENCIL_WRAP));

    ASSERT_EQ(caps.hasCapability(RSC_HWOCCLUSION), caps2.hasCapability(RSC_HWOCCLUSION));
    ASSERT_EQ(caps.hasCapability(RSC_USER_CLIP_PLANES), caps2.hasCapability(RSC_USER_CLIP_PLANES));
    ASSERT_EQ(caps.hasCapability(RSC_VERTEX_FORMAT_UBYTE4), caps2.hasCapability(RSC_VERTEX_FORMAT_UBYTE4));
    ASSERT_EQ(caps.hasCapability(RSC_INFINITE_FAR_PLANE), caps2.hasCapability(RSC_INFINITE_FAR_PLANE));
    ASSERT_EQ(caps.hasCapability(RSC_HWRENDER_TO_TEXTURE), caps2.hasCapability(RSC_HWRENDER_TO_TEXTURE));
    ASSERT_EQ(caps.hasCapability(RSC_TEXTURE_FLOAT), caps2.hasCapability(RSC_TEXTURE_FLOAT));

    ASSERT_EQ(caps.hasCapability(RSC_NON_POWER_OF_2_TEXTURES), caps2.hasCapability(RSC_NON_POWER_OF_2_TEXTURES));
    ASSERT_EQ(caps.hasCapability(RSC_TEXTURE_3D), caps2.hasCapability(RSC_TEXTURE_3D));
    ASSERT_EQ(caps.hasCapability(RSC_POINT_SPRITES), caps2.hasCapability(RSC_POINT_SPRITES));
    ASSERT_EQ(caps.hasCapability(RSC_POINT_EXTENDED_PARAMETERS), caps2.hasCapability(RSC_POINT_EXTENDED_PARAMETERS));
    ASSERT_EQ(caps.hasCapability(RSC_VERTEX_TEXTURE_FETCH), caps2.hasCapability(RSC_VERTEX_TEXTURE_FETCH));
    ASSERT_EQ(caps.hasCapability(RSC_MIPMAP_LOD_BIAS), caps2.hasCapability(RSC_MIPMAP_LOD_BIAS));

    ASSERT_EQ(caps.hasCapability(RSC_TEXTURE_COMPRESSION), caps2.hasCapability(RSC_TEXTURE_COMPRESSION));
    ASSERT_EQ(caps.hasCapability(RSC_TEXTURE_COMPRESSION_DXT), caps2.hasCapability(RSC_TEXTURE_COMPRESSION_DXT));
    ASSERT_EQ(caps.hasCapability(RSC_TEXTURE_COMPRESSION_VTC), caps2.hasCapability(RSC_TEXTURE_COMPRESSION_VTC));
    ASSERT_EQ(caps.hasCapability(RSC_TEXTURE_COMPRESSION_PVRTC), caps2.hasCapability(RSC_TEXTURE_COMPRESSION_PVRTC));
    ASSERT_EQ(caps.hasCapability(RSC_TEXTURE_COMPRESSION_BC4_BC5), caps2.hasCapability(RSC_TEXTURE_COMPRESSION_BC4_BC5));
    ASSERT_EQ(caps.hasCapability(RSC_TEXTURE_COMPRESSION_BC6H_BC7), caps2.hasCapability(RSC_TEXTURE_COMPRESSION_BC6H_BC7));
    ASSERT_EQ(caps.hasCapability(RSC_FBO), caps2.hasCapability(RSC_FBO));
    ASSERT_EQ(caps.hasCapability(RSC_FBO_ARB), caps2.hasCapability(RSC_FBO_ARB));

    ASSERT_EQ(caps.hasCapability(RSC_FBO_ATI), caps2.hasCapability(RSC_FBO_ATI));
    ASSERT_EQ(caps.hasCapability(RSC_PBUFFER), caps2.hasCapability(RSC_PBUFFER));
    ASSERT_EQ(caps.hasCapability(RSC_PERSTAGECONSTANT), caps2.hasCapability(RSC_PERSTAGECONSTANT));
    ASSERT_EQ(caps.hasCapability(RSC_SEPARATE_SHADER_OBJECTS), caps2.hasCapability(RSC_SEPARATE_SHADER_OBJECTS));
    ASSERT_EQ(caps.hasCapability(RSC_VAO), caps2.hasCapability(RSC_VAO));

    ASSERT_EQ(caps.getNumWorldMatrices(), caps2.getNumWorldMatrices());
    ASSERT_EQ(caps.getNumTextureUnits(), caps2.getNumTextureUnits());
    ASSERT_EQ(caps.getStencilBufferBitDepth(), caps2.getStencilBufferBitDepth());
    ASSERT_EQ(caps.getNumVertexBlendMatrices(), caps2.getNumVertexBlendMatrices());
    ASSERT_EQ(caps.getNumMultiRenderTargets(), caps2.getNumMultiRenderTargets());

    ASSERT_EQ(caps.getVertexProgramConstantFloatCount(), caps2.getVertexProgramConstantFloatCount());
    ASSERT_EQ(caps.getVertexProgramConstantIntCount(), caps2.getVertexProgramConstantIntCount());
    ASSERT_EQ(caps.getVertexProgramConstantBoolCount(), caps2.getVertexProgramConstantBoolCount());

    ASSERT_EQ(caps.getFragmentProgramConstantFloatCount(), caps2.getFragmentProgramConstantFloatCount());
    ASSERT_EQ(caps.getFragmentProgramConstantIntCount(), caps2.getFragmentProgramConstantIntCount());
    ASSERT_EQ(caps.getFragmentProgramConstantBoolCount(), caps2.getFragmentProgramConstantBoolCount());

    ASSERT_EQ(caps.getMaxPointSize(), caps2.getMaxPointSize());
    ASSERT_EQ(caps.getNonPOW2TexturesLimited(), caps2.getNonPOW2TexturesLimited());
    ASSERT_EQ(caps.getVertexTextureUnitsShared(), caps2.getVertexTextureUnitsShared());
    
    // test versions
    ASSERT_EQ(caps.getDriverVersion().major, caps2.getDriverVersion().major);
    ASSERT_EQ(caps.getDriverVersion().minor, caps2.getDriverVersion().minor);
    ASSERT_EQ(caps.getDriverVersion().release, caps2.getDriverVersion().release);
    ASSERT_EQ(0, caps2.getDriverVersion().build);

    dataStreamPtr.reset();
}
