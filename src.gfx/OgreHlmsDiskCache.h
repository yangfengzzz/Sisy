/*
 -----------------------------------------------------------------------------
 This source file is part of OGRE
 (Object-oriented Graphics Rendering Engine)
 For the latest info, see http://www.ogre3d.org/
 
 Copyright (c) 2000-2018 Torus Knot Software Ltd
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 -----------------------------------------------------------------------------
 */
#ifndef _OgreHlmsDiskCache_H_
#define _OgreHlmsDiskCache_H_

#include "OgreHlms.h"
#include "OgreHlmsDatablock.h"
#include "OgreHeaderPrefix.h"

namespace Ogre{
/** \addtogroup Core
 *  @{
 */
/** \addtogroup Resources
 *  @{
 */

/** @class HlmsDiskCache
 
 This class allows saving the current state of an Hlms to disk: both its compiled shaders
 with source and the generated PSOs.
 
 This allows cutting down stalls by compiling at loading time.
 
 This cache is not meant to be used with HLMS_COMPUTE or HLMS_LOW_LEVEL.
 
 The cache saves several layers:
 1. The preprocessed shader
 2. The properties used to generate the shader, in case the template becomes out of date
 3. The generated PSO pairs.
 
 The layer 1 can only be used for the same platform & RenderSystem it was built for (e.g. a
 cache built for Windows+D3D11 can only be used there). However the rest of the layers work
 on any platform on any RenderSystem.
 
 The cache cannot be used if it was generated with a different value of
 OGRE_DEBUG_STR_SIZE (i.e. see IdString) unless the current value of OGRE_DEBUG_STR_SIZE is 0.
 
 In practice this means caches generated by Debug builds can be used in Release, but caches
 generated by Release builds cannot be used in Debug, unless OGRE_IDSTRING_ALWAYS_READABLE
 was set for the Release build.
 
 Decrease loading times further by also enabling the microcode cache.
 See http://wiki.ogre3d.org/Ogre+2.1+FAQ#Starting_my_app_takes_forever_particularly_Direct3D11_
 
 @remarks
 IMPORTANT: If you're using the microcode cache, it should be loaded BEFORE the disk cache.
 And setSaveMicrocodesToCache should be called BEFORE loading the disk cache.
 
 What's the difference between the Microcode cache and HlmsDiskCache?
 
 A PSO has multiple stages since it's compiled in an Hlms:
 
 @code
 Hlms Shader Templates       These are the shader templates (e.g. PixelShader_ps.glsl)
 |
 v
 Property & Piece info       These are the properties set by the Hlms to describe information
 |                  about the mesh, the material and the pass (e.g. does it have
 |                  normals? does it use normal mapping? Is this a shadow mapping pass?)
 |                  This info is used to run the Hlms parser on the templates to
 |                  produce the preprocessed shaders for the next step.
 |                  HlmsDiskCache stores it in
 |                  HlmsDiskCache::Cache::sourceCode::mergedCache
 |                  This information is API & platform agnostic. In certain cases, the
 |                  cache may still be used even if the templates have changed.
 |
 v
 Preprocessed Shader        These are the templates processed by the Hlms. There is up to 1
 |                  for each shader stage. The produced output is valid shader code
 |                  but specific to each RenderSystem API (OpenGL, D3D11, Metal)
 |                  While parsing the templates is fast, it's not free; specially in
 |                  debug builds.
 |                  HlmsDiskCache stores this in
 |                  HlmsDiskCache::Cache::sourceCode::sourceFile
 |
 v
 Compiled Microcode         The is a binary blob produced by the shader compiler
 |                  (e.g fxc in D3D11). *This is cached by the Microcode cache*.
 |                  If the microcode cache is not active, the HlmsDiskCache will
 |                  rebuild it from scratch to move the performance hit (stalls) from
 |                  realtime into loading time.
 |
 V
 Pipeline State Object       This is a huge amalgamation of all the information required to
 (PSO)                draw a triangle on screen. See HlmsPso for what's in it.
 The driver will internally merge the compiled microcode and PSO info
 and translate it into an ISA (Instruction Set Architecture) which is
 specific to the GPU & Driver the user currently has installed;
 and store the ISA into the PSO.
 Under Vulkan & D3D12 this ISA can be saved to disk.
 However for the rest of the APIs, HlmsDiskCache saves all the info
 required to rebuild the PSO/ISA from scratch again.
 HlmsDiskCache stores this info in HlmsDiskCache::Cache::pso
 Depending on the API and Driver, building the PSO can be very fast
 or take significant time.
 
 Note that due to a technical issue, this information is currently
 being saved to disk but the PSO is not rebuilt (i.e. the information
 is not used). Because of it, certain platforms may still experience
 some stalls at runtime, due to the driver translating the Microcode
 to the internal ISA.
 @endcode
 */
class _OgreExport HlmsDiskCache : public HlmsAlloc
{
public:
    struct SourceCode
    {
        Hlms::RenderableCache   mergedCache;
        String                  sourceFile[NumShaderTypes];
        
        SourceCode();
        SourceCode( const Hlms::ShaderCodeCache &shaderCodeCache );
    };
    
    typedef vector<SourceCode>::type SourceCodeVec;
    
    struct Pso
    {
        Hlms::RenderableCache   renderableCache;
        HlmsPropertyVec         passProperties;
        HlmsPso                 pso;
        HlmsMacroblock          macroblock;
        HlmsBlendblock          blendblock;
        
        Pso();
        Pso( const Hlms::RenderableCache &srcRenderableCache,
            const Hlms::PassCache &srcPassCache,
            const HlmsCache *srcPsoCache );
    };
    
    typedef vector<Pso>::type PsoVec;
    
    struct Cache
    {
        uint64          templateHash[2]; //128 bit hash
        uint8           type;           /// See HlmsTypes
        SourceCodeVec   sourceCode;
        PsoVec          pso;
    };
    
    bool        mTemplatesOutOfDate;
    Cache       mCache;
    HlmsManager *mHlmsManager;
    String      mShaderProfile;
    uint16      mDebugStrSize;
    
    void save( DataStreamPtr &dataStream, const IdString &hashedString );
    void save( DataStreamPtr &dataStream, const String &string );
    void save( DataStreamPtr &dataStream, const HlmsPropertyVec &properties );
    void save( DataStreamPtr &dataStream, const Hlms::RenderableCache &renderableCache );
    
    void load( DataStreamPtr &dataStream, IdString &hashedString );
    void load( DataStreamPtr &dataStream, String &string );
    void load( DataStreamPtr &dataStream, HlmsPropertyVec &properties );
    void load( DataStreamPtr &dataStream, Hlms::RenderableCache &renderableCache );
    
public:
    HlmsDiskCache( HlmsManager *hlmsManager );
    ~HlmsDiskCache();
    
    void clearCache(void);
    
    void copyFrom( Hlms *hlms );
    void applyTo( Hlms *hlms );
    
    void saveTo( DataStreamPtr &dataStream );
    void loadFrom( DataStreamPtr &dataStream );
};

/** @} */
/** @} */

}

#include "OgreHeaderSuffix.h"

#endif
