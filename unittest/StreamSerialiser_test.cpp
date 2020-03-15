////
////  StreamSerialiser_test.cpp
////  GUnitTest
////
////  Created by 杨丰 on 2019/12/1.
////  Copyright © 2019 杨丰. All rights reserved.
////
//
//#include "../src.gfx/OgreVector3.h"
//#include "../src.gfx/OgreFileSystem.h"
//#include "../src.gfx/OgreStreamSerialiser.h"
//#include "../external/gtest/include/gtest/gtest.h"
//
//using namespace Ogre;
//
//TEST(StreamSerialiserTests, testWriteBasic){
//    FileSystemArchive arch("./", "FileSystem", false);
//    arch.load();
//
//    String fileName = "testSerialiser.dat";
//    Vector3 aTestVector(0.3, 15.2, -12.0);
//    String aTestString = "Some text here";
//    int aTestValue = 99;
//    uint32 chunkID = StreamSerialiser::makeIdentifier("TEST");
//
//    // write the data
//    {
//        DataStreamPtr stream = arch.create(fileName);
//
//        StreamSerialiser serialiser(stream);
//
//        serialiser.writeChunkBegin(chunkID);
//
//        serialiser.write(&aTestVector);
//        serialiser.write(&aTestString);
//        serialiser.write(&aTestValue);
//        serialiser.writeChunkEnd(chunkID);
//    }
//
//    // read it back
//    {
//        DataStreamPtr stream = arch.open(fileName);
//
//        StreamSerialiser serialiser(stream);
//
//        const StreamSerialiser::Chunk* c = serialiser.readChunkBegin();
//
//        EXPECT_EQ(chunkID, c->id);
//        EXPECT_EQ(sizeof(Vector3) + sizeof(int) + aTestString.size() + 4, (size_t)c->length);
//
//        Vector3 inVector;
//        String inString;
//        int inValue;
//        
//        serialiser.read(&inVector);
//        serialiser.read(&inString);
//        serialiser.read(&inValue);
//        serialiser.readChunkEnd(chunkID);
//
//        EXPECT_EQ(aTestVector, inVector);
//        EXPECT_EQ(aTestString, inString);
//        EXPECT_EQ(aTestValue, inValue);
//    }
//
//    arch.remove(fileName);
//
//    ASSERT_EQ(!arch.exists(fileName), true);
//}
