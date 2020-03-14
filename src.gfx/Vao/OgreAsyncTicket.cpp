/*
 -----------------------------------------------------------------------------
 This source file is part of OGRE
 (Object-oriented Graphics Rendering Engine)
 For the latest info, see http://www.ogre3d.org
 
 Copyright (c) 2000-2014 Torus Knot Software Ltd
 
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

#include "OgreStableHeaders.h"

#include "OgreAsyncTicket.h"

#include "OgreStagingBuffer.h"
#include "OgreProfiler.h"

namespace Ogre{
AsyncTicket::AsyncTicket( BufferPacked *creator, StagingBuffer *stagingBuffer,
                         size_t elementStart, size_t elementCount ) :
mHasBeenMapped( false ),
mCreator( creator ),
mStagingBuffer( stagingBuffer ),
mElementStart( elementStart ),
mElementCount( elementCount )
{
    mStagingBufferMapOffset = stagingBuffer->_asyncDownload(
                                                            creator,
                                                            elementStart * creator->getBytesPerElement(),
                                                            elementCount * creator->getBytesPerElement() );
}
//-----------------------------------------------------------------------------------
AsyncTicket::~AsyncTicket()
{
    if( mStagingBuffer->getMappingState() != MS_UNMAPPED )
    {
        unmap();
    }
    else if( !mHasBeenMapped )
    {
        mStagingBuffer->_cancelDownload( mStagingBufferMapOffset,
                                        mElementCount * mCreator->getBytesPerElement() );
    }
    
    mStagingBuffer->removeReferenceCount();
    mStagingBuffer = 0;
}
//-----------------------------------------------------------------------------------
const void* AsyncTicket::map(void)
{
    assert( !mHasBeenMapped );
    
    OgreProfileExhaustive( "AsyncTicket::map" );
    
    mHasBeenMapped = true;
    return mapImpl();
}
//-----------------------------------------------------------------------------------
void AsyncTicket::unmap(void)
{
    mStagingBuffer->unmap( 0, 0 );
}
//-----------------------------------------------------------------------------------
}
