//
//  FileSystemArchive_test.cpp
//  GUnitTest
//
//  Created by 杨丰 on 2019/12/1.
//  Copyright © 2019 杨丰. All rights reserved.
//

#include "../src.gfx/OgreCommon.h"
#include "../src.gfx/OgreStringVector.h"
#include "../src.gfx/OgreFileSystem.h"
#include "../external/gtest/include/gtest/gtest.h"

using namespace Ogre;

class FileSystemArchiveTests : public testing::Test {
public:
    void SetUp() override{
        mTestPath = "../../../../../../Tests/misc/ArchiveTest";
        mFileSizeRoot1 = 125;
        mFileSizeRoot2 = 150;
    }
    
    String mTestPath;
    size_t mFileSizeRoot1;
    size_t mFileSizeRoot2;
};

TEST_F(FileSystemArchiveTests, testListNonRecursive){
    try {
        FileSystemArchive arch(mTestPath, "FileSystem", true);
        arch.load();
        StringVectorPtr vec = arch.list(false);
        
        EXPECT_EQ((unsigned int)2, (unsigned int)vec->size());
        EXPECT_EQ(String("rootfile2.txt"), vec->at(0));
        EXPECT_EQ(String("rootfile.txt"), vec->at(1));
    }
    catch (Exception& e)
    {
        std::cout << e.getFullDescription();
    }
}

TEST_F(FileSystemArchiveTests, testListRecursive){
    FileSystemArchive arch(mTestPath, "FileSystem", true);
    arch.load();
    StringVectorPtr vec = arch.list(true);
    
    EXPECT_EQ((size_t)6, vec->size());
    EXPECT_EQ(String("rootfile2.txt"), vec->at(0));
    EXPECT_EQ(String("rootfile.txt"), vec->at(1));
    EXPECT_EQ(String("level2/materials/scripts/file3.material"), vec->at(2));
    EXPECT_EQ(String("level2/materials/scripts/file4.material"), vec->at(3));
    EXPECT_EQ(String("level1/materials/scripts/file.material"), vec->at(4));
    EXPECT_EQ(String("level1/materials/scripts/file2.material"), vec->at(5));
}

TEST_F(FileSystemArchiveTests, testListFileInfoNonRecursive){
    FileSystemArchive arch(mTestPath, "FileSystem", true);
    arch.load();
    FileInfoListPtr vec = arch.listFileInfo(false);
    
    // Only execute size checks, if the values have been set for the current platform
    if(mFileSizeRoot1 >0 && mFileSizeRoot2 > 0)
    {
        EXPECT_EQ((size_t)2, vec->size());
        FileInfo& fi1 = vec->at(1);
        EXPECT_EQ(String("rootfile.txt"), fi1.filename);
        EXPECT_EQ(String("rootfile.txt"), fi1.basename);
        EXPECT_EQ(BLANKSTRING, fi1.path);
        EXPECT_EQ((size_t)mFileSizeRoot1, fi1.compressedSize);
        EXPECT_EQ((size_t)mFileSizeRoot1, fi1.uncompressedSize);
        
        FileInfo& fi2 = vec->at(0);
        EXPECT_EQ(String("rootfile2.txt"), fi2.filename);
        EXPECT_EQ(String("rootfile2.txt"), fi2.basename);
        EXPECT_EQ(BLANKSTRING, fi2.path);
        EXPECT_EQ((size_t)mFileSizeRoot2, fi2.compressedSize);
        EXPECT_EQ((size_t)mFileSizeRoot2, fi2.uncompressedSize);
    }
}

TEST_F(FileSystemArchiveTests, testListFileInfoRecursive){
    FileSystemArchive arch(mTestPath, "FileSystem", true);
    arch.load();
    FileInfoListPtr vec = arch.listFileInfo(true);
    
    // Only execute size checks, if the values have been set for the current platform
    if(mFileSizeRoot1 >0 && mFileSizeRoot2 > 0)
    {
        EXPECT_EQ((size_t)6, vec->size());
        FileInfo& fi1 = vec->at(1);
        EXPECT_EQ(String("rootfile.txt"), fi1.filename);
        EXPECT_EQ(String("rootfile.txt"), fi1.basename);
        EXPECT_EQ(BLANKSTRING, fi1.path);
        EXPECT_EQ((size_t)mFileSizeRoot1, fi1.compressedSize);
        EXPECT_EQ((size_t)mFileSizeRoot1, fi1.uncompressedSize);
        
        FileInfo& fi2 = vec->at(0);
        EXPECT_EQ(String("rootfile2.txt"), fi2.filename);
        EXPECT_EQ(String("rootfile2.txt"), fi2.basename);
        EXPECT_EQ(BLANKSTRING, fi2.path);
        EXPECT_EQ((size_t)mFileSizeRoot2, fi2.compressedSize);
        EXPECT_EQ((size_t)mFileSizeRoot2, fi2.uncompressedSize);
    }
    
    FileInfo& fi3 = vec->at(4);
    EXPECT_EQ(String("level1/materials/scripts/file.material"), fi3.filename);
    EXPECT_EQ(String("file.material"), fi3.basename);
    EXPECT_EQ(String("level1/materials/scripts/"), fi3.path);
    EXPECT_EQ((size_t)0, fi3.compressedSize);
    EXPECT_EQ((size_t)0, fi3.uncompressedSize);
    
    FileInfo& fi4 = vec->at(5);
    EXPECT_EQ(String("level1/materials/scripts/file2.material"), fi4.filename);
    EXPECT_EQ(String("file2.material"), fi4.basename);
    EXPECT_EQ(String("level1/materials/scripts/"), fi4.path);
    EXPECT_EQ((size_t)0, fi4.compressedSize);
    EXPECT_EQ((size_t)0, fi4.uncompressedSize);
    
    FileInfo& fi5 = vec->at(2);
    EXPECT_EQ(String("level2/materials/scripts/file3.material"), fi5.filename);
    EXPECT_EQ(String("file3.material"), fi5.basename);
    EXPECT_EQ(String("level2/materials/scripts/"), fi5.path);
    EXPECT_EQ((size_t)0, fi5.compressedSize);
    EXPECT_EQ((size_t)0, fi5.uncompressedSize);
    
    FileInfo& fi6 = vec->at(3);
    EXPECT_EQ(String("level2/materials/scripts/file4.material"), fi6.filename);
    EXPECT_EQ(String("file4.material"), fi6.basename);
    EXPECT_EQ(String("level2/materials/scripts/"), fi6.path);
    EXPECT_EQ((size_t)0, fi6.compressedSize);
    EXPECT_EQ((size_t)0, fi6.uncompressedSize);
}

TEST_F(FileSystemArchiveTests, testFindNonRecursive){
    FileSystemArchive arch(mTestPath, "FileSystem", true);
    arch.load();
    StringVectorPtr vec = arch.find("*.txt", false);
    
    EXPECT_EQ((size_t)2, vec->size());
    EXPECT_EQ(String("rootfile.txt"), vec->at(1));
    EXPECT_EQ(String("rootfile2.txt"), vec->at(0));
}

TEST_F(FileSystemArchiveTests, testFindRecursive){
    FileSystemArchive arch(mTestPath, "FileSystem", true);
    arch.load();
    StringVectorPtr vec = arch.find("*.material", true);
    
    EXPECT_EQ((size_t)4, vec->size());
    EXPECT_EQ(String("level1/materials/scripts/file.material"), vec->at(2));
    EXPECT_EQ(String("level1/materials/scripts/file2.material"), vec->at(3));
    EXPECT_EQ(String("level2/materials/scripts/file3.material"), vec->at(0));
    EXPECT_EQ(String("level2/materials/scripts/file4.material"), vec->at(1));
}

TEST_F(FileSystemArchiveTests, testFindFileInfoNonRecursive){
    FileSystemArchive arch(mTestPath, "FileSystem", true);
    arch.load();
    FileInfoListPtr vec = arch.findFileInfo("*.txt", false);
    
    // Only execute size checks, if the values have been set for the current platform
    if(mFileSizeRoot1 >0 && mFileSizeRoot2 > 0)
    {
        EXPECT_EQ((size_t)2, vec->size());
        FileInfo& fi1 = vec->at(1);
        EXPECT_EQ(String("rootfile.txt"), fi1.filename);
        EXPECT_EQ(String("rootfile.txt"), fi1.basename);
        EXPECT_EQ(BLANKSTRING, fi1.path);
        EXPECT_EQ((size_t)mFileSizeRoot1, fi1.compressedSize);
        EXPECT_EQ((size_t)mFileSizeRoot1, fi1.uncompressedSize);
        
        FileInfo& fi2 = vec->at(0);
        EXPECT_EQ(String("rootfile2.txt"), fi2.filename);
        EXPECT_EQ(String("rootfile2.txt"), fi2.basename);
        EXPECT_EQ(BLANKSTRING, fi2.path);
        EXPECT_EQ((size_t)mFileSizeRoot2, fi2.compressedSize);
        EXPECT_EQ((size_t)mFileSizeRoot2, fi2.uncompressedSize);
    }
}

TEST_F(FileSystemArchiveTests, testFindFileInfoRecursive){
    FileSystemArchive arch(mTestPath, "FileSystem", true);
    arch.load();
    FileInfoListPtr vec = arch.findFileInfo("*.material", true);
    
    EXPECT_EQ((size_t)4, vec->size());
    
    FileInfo& fi3 = vec->at(2);
    EXPECT_EQ(String("level1/materials/scripts/file.material"), fi3.filename);
    EXPECT_EQ(String("file.material"), fi3.basename);
    EXPECT_EQ(String("level1/materials/scripts/"), fi3.path);
    EXPECT_EQ((size_t)0, fi3.compressedSize);
    EXPECT_EQ((size_t)0, fi3.uncompressedSize);
    
    FileInfo& fi4 = vec->at(3);
    EXPECT_EQ(String("level1/materials/scripts/file2.material"), fi4.filename);
    EXPECT_EQ(String("file2.material"), fi4.basename);
    EXPECT_EQ(String("level1/materials/scripts/"), fi4.path);
    EXPECT_EQ((size_t)0, fi4.compressedSize);
    EXPECT_EQ((size_t)0, fi4.uncompressedSize);
    
    FileInfo& fi5 = vec->at(0);
    EXPECT_EQ(String("level2/materials/scripts/file3.material"), fi5.filename);
    EXPECT_EQ(String("file3.material"), fi5.basename);
    EXPECT_EQ(String("level2/materials/scripts/"), fi5.path);
    EXPECT_EQ((size_t)0, fi5.compressedSize);
    EXPECT_EQ((size_t)0, fi5.uncompressedSize);
    
    FileInfo& fi6 = vec->at(1);
    EXPECT_EQ(String("level2/materials/scripts/file4.material"), fi6.filename);
    EXPECT_EQ(String("file4.material"), fi6.basename);
    EXPECT_EQ(String("level2/materials/scripts/"), fi6.path);
    EXPECT_EQ((size_t)0, fi6.compressedSize);
    EXPECT_EQ((size_t)0, fi6.uncompressedSize);
}

TEST_F(FileSystemArchiveTests, testFileRead){
    FileSystemArchive arch(mTestPath, "FileSystem", true);
    arch.load();
    
    DataStreamPtr stream = arch.open("rootfile.txt");
    EXPECT_EQ(String("this is line 1 in file 1"), stream->getLine());
    EXPECT_EQ(String("this is line 2 in file 1"), stream->getLine());
    EXPECT_EQ(String("this is line 3 in file 1"), stream->getLine());
    EXPECT_EQ(String("this is line 4 in file 1"), stream->getLine());
    EXPECT_EQ(String("this is line 5 in file 1"), stream->getLine());
    EXPECT_EQ(BLANKSTRING, stream->getLine()); // blank at end of file
    ASSERT_EQ(stream->eof(), true);
}

TEST_F(FileSystemArchiveTests, testReadInterleave){
    // Test overlapping reads from same archive
    FileSystemArchive arch(mTestPath, "FileSystem", true);
    arch.load();
    
    // File 1
    DataStreamPtr stream1 = arch.open("rootfile.txt");
    EXPECT_EQ(String("this is line 1 in file 1"), stream1->getLine());
    EXPECT_EQ(String("this is line 2 in file 1"), stream1->getLine());
    
    // File 2
    DataStreamPtr stream2 = arch.open("rootfile2.txt");
    EXPECT_EQ(String("this is line 1 in file 2"), stream2->getLine());
    EXPECT_EQ(String("this is line 2 in file 2"), stream2->getLine());
    EXPECT_EQ(String("this is line 3 in file 2"), stream2->getLine());
    
    // File 1
    EXPECT_EQ(String("this is line 3 in file 1"), stream1->getLine());
    EXPECT_EQ(String("this is line 4 in file 1"), stream1->getLine());
    EXPECT_EQ(String("this is line 5 in file 1"), stream1->getLine());
    EXPECT_EQ(BLANKSTRING, stream1->getLine()); // blank at end of file
    ASSERT_EQ(stream1->eof(), true);
    
    // File 2
    EXPECT_EQ(String("this is line 4 in file 2"), stream2->getLine());
    EXPECT_EQ(String("this is line 5 in file 2"), stream2->getLine());
    EXPECT_EQ(String("this is line 6 in file 2"), stream2->getLine());
    EXPECT_EQ(BLANKSTRING, stream2->getLine()); // blank at end of file
    ASSERT_EQ(stream2->eof(), true);
}

TEST_F(FileSystemArchiveTests, testCreateAndRemoveFile){
    FileSystemArchive arch("./", "FileSystem", false);
    arch.load();
    
    ASSERT_EQ(!arch.isReadOnly(), true);
    
    String fileName = "a_test_file.txt";
    DataStreamPtr stream = arch.create(fileName);
    
    String testString = "Some text here";
    size_t written = stream->write((void*)testString.c_str(), testString.size());
    EXPECT_EQ(testString.size(), written);
    
    stream->close();
    
    arch.remove(fileName);
    
    ASSERT_EQ(!arch.exists(fileName), true);
}
