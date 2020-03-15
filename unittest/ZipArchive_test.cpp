//
//  ZipArchive_test.cpp
//  GUnitTest
//
//  Created by 杨丰 on 2019/12/1.
//  Copyright © 2019 杨丰. All rights reserved.
//

#include "../src.gfx/OgreCommon.h"
#include "../src.gfx/OgreZip.h"
#include "../src.gfx/macUtils.h"
#include "../external/gtest/include/gtest/gtest.h"

using namespace Ogre;

class ZipArchiveTests : public testing::Test {
public:
    void SetUp() override{
        mTestPath = "../../../../../../Tests/misc/ArchiveTest.zip";
    }
    
    String mTestPath;
};

TEST_F(ZipArchiveTests, testListNonRecursive){
    ZipArchive* arch = OGRE_NEW ZipArchive(mTestPath, "Zip");
    try {
        arch->load();
    } catch (Ogre::Exception e) {
        // If it starts in build/bin/debug
        OGRE_DELETE arch;
        arch = OGRE_NEW ZipArchive("../../../" + mTestPath, "Zip");
        arch->load();
    }
    StringVectorPtr vec = arch->list(false);
    
    EXPECT_EQ((size_t)2, vec->size());
    EXPECT_EQ(String("rootfile.txt"), vec->at(0));
    EXPECT_EQ(String("rootfile2.txt"), vec->at(1));
    
    OGRE_DELETE arch;
}

TEST_F(ZipArchiveTests, testListRecursive){
    ZipArchive* arch = OGRE_NEW ZipArchive(mTestPath, "Zip");
    try {
        arch->load();
    } catch (Ogre::Exception e) {
        // If it starts in build/bin/debug
        OGRE_DELETE arch;
        arch = OGRE_NEW ZipArchive("../../../" + mTestPath, "Zip");
        arch->load();
    }
    StringVectorPtr vec = arch->list(true);
    
    EXPECT_EQ((size_t)6, vec->size());
    EXPECT_EQ(String("file.material"), vec->at(0));
    EXPECT_EQ(String("file2.material"), vec->at(1));
    EXPECT_EQ(String("file3.material"), vec->at(2));
    EXPECT_EQ(String("file4.material"), vec->at(3));
    EXPECT_EQ(String("rootfile.txt"), vec->at(4));
    EXPECT_EQ(String("rootfile2.txt"), vec->at(5));
    
    OGRE_DELETE arch;
}

TEST_F(ZipArchiveTests, testListFileInfoNonRecursive){
    ZipArchive* arch = OGRE_NEW ZipArchive(mTestPath, "Zip");
    try {
        arch->load();
    } catch (Ogre::Exception e) {
        // If it starts in build/bin/debug
        OGRE_DELETE arch;
        arch = OGRE_NEW ZipArchive("../../../" + mTestPath, "Zip");
        arch->load();
    }
    FileInfoListPtr vec = arch->listFileInfo(false);
    
    EXPECT_EQ((size_t)2, vec->size());
    FileInfo& fi1 = vec->at(0);
    EXPECT_EQ(String("rootfile.txt"), fi1.filename);
    EXPECT_EQ(BLANKSTRING, fi1.path);
    EXPECT_EQ((size_t)40, fi1.compressedSize);
    EXPECT_EQ((size_t)130, fi1.uncompressedSize);
    
    FileInfo& fi2 = vec->at(1);
    EXPECT_EQ(String("rootfile2.txt"), fi2.filename);
    EXPECT_EQ(BLANKSTRING, fi2.path);
    EXPECT_EQ((size_t)45, fi2.compressedSize);
    EXPECT_EQ((size_t)156, fi2.uncompressedSize);
    
    OGRE_DELETE arch;
}

TEST_F(ZipArchiveTests, testListFileInfoRecursive){
    ZipArchive* arch = OGRE_NEW ZipArchive(mTestPath, "Zip");
    try {
        arch->load();
    } catch (Ogre::Exception e) {
        // If it starts in build/bin/debug
        OGRE_DELETE arch;
        arch = OGRE_NEW ZipArchive("../../../" + mTestPath, "Zip");
        arch->load();
    }
    FileInfoListPtr vec = arch->listFileInfo(true);
    
    EXPECT_EQ((size_t)6, vec->size());
    FileInfo& fi3 = vec->at(0);
    EXPECT_EQ(String("file.material"), fi3.filename);
    EXPECT_EQ(String("level1/materials/scripts/"), fi3.path);
    EXPECT_EQ((size_t)0, fi3.compressedSize);
    EXPECT_EQ((size_t)0, fi3.uncompressedSize);
    
    FileInfo& fi4 = vec->at(1);
    EXPECT_EQ(String("file2.material"), fi4.filename);
    EXPECT_EQ(String("level1/materials/scripts/"), fi4.path);
    EXPECT_EQ((size_t)0, fi4.compressedSize);
    EXPECT_EQ((size_t)0, fi4.uncompressedSize);
    
    FileInfo& fi5 = vec->at(2);
    EXPECT_EQ(String("file3.material"), fi5.filename);
    EXPECT_EQ(String("level2/materials/scripts/"), fi5.path);
    EXPECT_EQ((size_t)0, fi5.compressedSize);
    EXPECT_EQ((size_t)0, fi5.uncompressedSize);
    
    FileInfo& fi6 = vec->at(3);
    EXPECT_EQ(String("file4.material"), fi6.filename);
    EXPECT_EQ(String("level2/materials/scripts/"), fi6.path);
    EXPECT_EQ((size_t)0, fi6.compressedSize);
    EXPECT_EQ((size_t)0, fi6.uncompressedSize);
    
    FileInfo& fi1 = vec->at(4);
    EXPECT_EQ(String("rootfile.txt"), fi1.filename);
    EXPECT_EQ(BLANKSTRING, fi1.path);
    EXPECT_EQ((size_t)40, fi1.compressedSize);
    EXPECT_EQ((size_t)130, fi1.uncompressedSize);
    
    FileInfo& fi2 = vec->at(5);
    EXPECT_EQ(String("rootfile2.txt"), fi2.filename);
    EXPECT_EQ(BLANKSTRING, fi2.path);
    EXPECT_EQ((size_t)45, fi2.compressedSize);
    EXPECT_EQ((size_t)156, fi2.uncompressedSize);
    
    OGRE_DELETE arch;
}

TEST_F(ZipArchiveTests, testFindNonRecursive){
    ZipArchive* arch = OGRE_NEW ZipArchive(mTestPath, "Zip");
    try {
        arch->load();
    } catch (Ogre::Exception e) {
        // If it starts in build/bin/debug
        OGRE_DELETE arch;
        arch = OGRE_NEW ZipArchive("../../../" + mTestPath, "Zip");
        arch->load();
    }
    
    StringVectorPtr vec = arch->find("*.txt", false);
    
    EXPECT_EQ((size_t)2, vec->size());
    EXPECT_EQ(String("rootfile.txt"), vec->at(0));
    EXPECT_EQ(String("rootfile2.txt"), vec->at(1));
    
    OGRE_DELETE arch;
}

TEST_F(ZipArchiveTests, testFindRecursive){
    ZipArchive* arch = OGRE_NEW ZipArchive(mTestPath, "Zip");
    try {
        arch->load();
    } catch (Ogre::Exception e) {
        // If it starts in build/bin/debug
        OGRE_DELETE arch;
        arch = OGRE_NEW ZipArchive("../../../" + mTestPath, "Zip");
        arch->load();
    }
    
    StringVectorPtr vec = arch->find("*.material", true);
    
    EXPECT_EQ((size_t)4, vec->size());
    EXPECT_EQ(String("file.material"), vec->at(0));
    EXPECT_EQ(String("file2.material"), vec->at(1));
    EXPECT_EQ(String("file3.material"), vec->at(2));
    EXPECT_EQ(String("file4.material"), vec->at(3));
    
    OGRE_DELETE arch;
}

TEST_F(ZipArchiveTests, testFindFileInfoNonRecursive){
    ZipArchive* arch = OGRE_NEW ZipArchive(mTestPath, "Zip");
    try {
        arch->load();
    } catch (Ogre::Exception e) {
        // If it starts in build/bin/debug
        OGRE_DELETE arch;
        arch = OGRE_NEW ZipArchive("../../../" + mTestPath, "Zip");
        arch->load();
    }
    FileInfoListPtr vec = arch->findFileInfo("*.txt", false);
    
    EXPECT_EQ((size_t)2, vec->size());
    FileInfo& fi1 = vec->at(0);
    EXPECT_EQ(String("rootfile.txt"), fi1.filename);
    EXPECT_EQ(BLANKSTRING, fi1.path);
    EXPECT_EQ((size_t)40, fi1.compressedSize);
    EXPECT_EQ((size_t)130, fi1.uncompressedSize);
    
    FileInfo& fi2 = vec->at(1);
    EXPECT_EQ(String("rootfile2.txt"), fi2.filename);
    EXPECT_EQ(BLANKSTRING, fi2.path);
    EXPECT_EQ((size_t)45, fi2.compressedSize);
    EXPECT_EQ((size_t)156, fi2.uncompressedSize);
    
    OGRE_DELETE arch;
}

TEST_F(ZipArchiveTests, testFindFileInfoRecursive){
    ZipArchive* arch = OGRE_NEW ZipArchive(mTestPath, "Zip");
    try {
        arch->load();
    } catch (Ogre::Exception e) {
        // If it starts in build/bin/debug
        OGRE_DELETE arch;
        arch = OGRE_NEW ZipArchive("../../../" + mTestPath, "Zip");
        arch->load();
    }
    FileInfoListPtr vec = arch->findFileInfo("*.material", true);
    
    EXPECT_EQ((size_t)4, vec->size());
    
    FileInfo& fi3 = vec->at(0);
    EXPECT_EQ(String("file.material"), fi3.filename);
    EXPECT_EQ(String("level1/materials/scripts/"), fi3.path);
    EXPECT_EQ((size_t)0, fi3.compressedSize);
    EXPECT_EQ((size_t)0, fi3.uncompressedSize);
    
    FileInfo& fi4 = vec->at(1);
    EXPECT_EQ(String("file2.material"), fi4.filename);
    EXPECT_EQ(String("level1/materials/scripts/"), fi4.path);
    EXPECT_EQ((size_t)0, fi4.compressedSize);
    EXPECT_EQ((size_t)0, fi4.uncompressedSize);
    
    FileInfo& fi5 = vec->at(2);
    EXPECT_EQ(String("file3.material"), fi5.filename);
    EXPECT_EQ(String("level2/materials/scripts/"), fi5.path);
    EXPECT_EQ((size_t)0, fi5.compressedSize);
    EXPECT_EQ((size_t)0, fi5.uncompressedSize);
    
    FileInfo& fi6 = vec->at(3);
    EXPECT_EQ(String("file4.material"), fi6.filename);
    EXPECT_EQ(String("level2/materials/scripts/"), fi6.path);
    EXPECT_EQ((size_t)0, fi6.compressedSize);
    EXPECT_EQ((size_t)0, fi6.uncompressedSize);
    
    OGRE_DELETE arch;
}

TEST_F(ZipArchiveTests, testFileRead){
    ZipArchive* arch = OGRE_NEW ZipArchive(mTestPath, "Zip");
    try {
        arch->load();
    } catch (Ogre::Exception e) {
        // If it starts in build/bin/debug
        OGRE_DELETE arch;
        arch = OGRE_NEW ZipArchive("../../../" + mTestPath, "Zip");
        arch->load();
    }
    
    DataStreamPtr stream = arch->open("rootfile.txt");
    EXPECT_EQ(String("this is line 1 in file 1"), stream->getLine());
    EXPECT_EQ(String("this is line 2 in file 1"), stream->getLine());
    EXPECT_EQ(String("this is line 3 in file 1"), stream->getLine());
    EXPECT_EQ(String("this is line 4 in file 1"), stream->getLine());
    EXPECT_EQ(String("this is line 5 in file 1"), stream->getLine());
    ASSERT_EQ(stream->eof(), true);
    
    OGRE_DELETE arch;
}

TEST_F(ZipArchiveTests, testReadInterleave){
    // Test overlapping reads from same archive
    ZipArchive* arch = OGRE_NEW ZipArchive(mTestPath, "Zip");
    try {
        arch->load();
    } catch (Ogre::Exception e) {
        // If it starts in build/bin/debug
        OGRE_DELETE arch;
        arch = OGRE_NEW ZipArchive("../../../" + mTestPath, "Zip");
        arch->load();
    }
    
    // File 1
    DataStreamPtr stream1 = arch->open("rootfile.txt");
    EXPECT_EQ(String("this is line 1 in file 1"), stream1->getLine());
    EXPECT_EQ(String("this is line 2 in file 1"), stream1->getLine());
    
    // File 2
    DataStreamPtr stream2 = arch->open("rootfile2.txt");
    EXPECT_EQ(String("this is line 1 in file 2"), stream2->getLine());
    EXPECT_EQ(String("this is line 2 in file 2"), stream2->getLine());
    EXPECT_EQ(String("this is line 3 in file 2"), stream2->getLine());
    
    // File 1
    EXPECT_EQ(String("this is line 3 in file 1"), stream1->getLine());
    EXPECT_EQ(String("this is line 4 in file 1"), stream1->getLine());
    EXPECT_EQ(String("this is line 5 in file 1"), stream1->getLine());
    ASSERT_EQ(stream1->eof(), true);
    
    // File 2
    EXPECT_EQ(String("this is line 4 in file 2"), stream2->getLine());
    EXPECT_EQ(String("this is line 5 in file 2"), stream2->getLine());
    EXPECT_EQ(String("this is line 6 in file 2"), stream2->getLine());
    ASSERT_EQ(stream2->eof(), true);
    
    OGRE_DELETE arch;
}
