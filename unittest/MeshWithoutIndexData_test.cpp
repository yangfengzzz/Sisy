//
//  MeshWithoutIndexData_test.cpp
//  GUnitTest
//
//  Created by 杨丰 on 2019/12/1.
//  Copyright © 2019 杨丰. All rights reserved.
//

#include "../src.gfx/OgreVertexIndexData.h"
#include "../src.gfx/Math/Array/OgreObjectMemoryManager.h"
#include "../src.gfx/OgreManualObject.h"
#include "../src.gfx/OgrePass.h"
#include "../src.gfx/OgreTechnique.h"
#include "../src.gfx/OgreMaterialManager.h"
#include "../src.gfx/OgreMaterialSerializer.h"
#include "../src.gfx/OgreLodStrategyManager.h"
#include "../src.gfx/OgreDefaultHardwareBufferManager.h"
#include "../src.gfx/OgreArchiveManager.h"
#include "../src.gfx/OgreFileSystem.h"
#include "../src.gfx/OgreMeshSerializer.h"
#include "../src.gfx/OgreMeshManager.h"
#include "../external/gtest/include/gtest/gtest.h"

using namespace Ogre;

class MeshWithoutIndexDataTests : public testing::Test {
public:
    void SetUp() override{
        OGRE_NEW ResourceGroupManager();
        OGRE_NEW LodStrategyManager();
        mBufMgr = OGRE_NEW v1::DefaultHardwareBufferManager();
        mMeshMgr = OGRE_NEW v1::MeshManager();
        mArchiveMgr = OGRE_NEW ArchiveManager();
        mArchiveMgr->addArchiveFactory(OGRE_NEW FileSystemArchiveFactory());
        
        MaterialManager* matMgr = OGRE_NEW MaterialManager();
        matMgr->initialise();
    }
    
    void TearDown() override{
        OGRE_DELETE MaterialManager::getSingletonPtr();
        OGRE_DELETE mArchiveMgr;
        OGRE_DELETE mMeshMgr;
        OGRE_DELETE mBufMgr;
        OGRE_DELETE LodStrategyManager::getSingletonPtr();
        OGRE_DELETE ResourceGroupManager::getSingletonPtr();
    }
    
    v1::HardwareBufferManager* mBufMgr;
    v1::MeshManager* mMeshMgr;
    ArchiveManager* mArchiveMgr;
    
public:
    void createMeshWithMaterial(String fileName)
    {
        String matFileNameSuffix = ".material";
        String matName1 = "red";
        String matFileName1 = matName1 + matFileNameSuffix;
        MaterialPtr matPtr = MaterialManager::getSingleton().create(matName1, "General");
        Pass* pass = matPtr->getTechnique(0)->getPass(0);
        pass->setDiffuse(1.0, 0.1, 0.1, 0);

        String matName2 = "green";
        String matFileName2 = matName2 + matFileNameSuffix;
        matPtr = MaterialManager::getSingleton().create(matName2, "General");
        pass = matPtr->getTechnique(0)->getPass(0);
        pass->setDiffuse(0.1, 1.0, 0.1, 0);

        String matName3 = "blue";
        String matFileName3 = matName3 + matFileNameSuffix;
        matPtr = MaterialManager::getSingleton().create(matName3, "General");
        pass = matPtr->getTechnique(0)->getPass(0);
        pass->setDiffuse(0.1, 0.1, 1.0, 0);

        String matName4 = "yellow";
        String matFileName4 = matName4 + matFileNameSuffix;
        matPtr = MaterialManager::getSingleton().create(matName4, "General");
        pass = matPtr->getTechnique(0)->getPass(0);
        pass->setDiffuse(1.0, 1.0, 0.1, 0);

        v1::ManualObject* manObj = OGRE_NEW v1::ManualObject(0, new ObjectMemoryManager(), nullptr);
        manObj->setName("mesh");
        manObj->begin(matName1, OT_TRIANGLE_LIST);
        manObj->position(0, 50, 0);
        manObj->position(50, 50, 0);
        manObj->position(0, 100, 0);
        manObj->triangle(0, 1, 2);
        manObj->position(50, 100, 0);
        manObj->position(0, 100, 0);
        manObj->position(50, 50, 0);
        manObj->triangle(3, 4, 5);
        manObj->end();
        manObj->begin(matName2, OT_LINE_LIST);
        manObj->position(0, 100, 0);
        manObj->position(-50, 50, 0);
        manObj->position(-50, 0, 0);
        manObj->position(-50, 50, 0);
        manObj->position(-100, 0, 0);
        manObj->position(-50, 0, 0);
        manObj->end();
        manObj->begin(matName3, OT_LINE_STRIP);
        manObj->position(50, 100, 0);
        manObj->position(100, 50, 0);
        manObj->position(100, 0, 0);
        manObj->position(150, 0, 0);
        manObj->end();
        manObj->begin(matName4, OT_POINT_LIST);
        manObj->position(50, 0, 0);
        manObj->position(0, 0, 0);
        manObj->end();
        manObj->convertToMesh(fileName);
        OGRE_DELETE manObj;
    }
};

//TEST_F(MeshWithoutIndexDataTests, testCreateSimpleLine){
//    v1::ManualObject* line = OGRE_NEW v1::ManualObject(0, new ObjectMemoryManager(), nullptr);
//    line->setName("line");
//    line->begin("BaseWhiteNoLighting", OT_LINE_LIST);
//    line->position(0, 50, 0);
//    line->position(50, 100, 0);
//    line->end();
//    String fileName = "line.mesh";
//    v1::MeshPtr lineMesh = line->convertToMesh(fileName);
//    OGRE_DELETE line;
//
//    ASSERT_EQ(lineMesh->getNumSubMeshes(), 1);
//    ASSERT_EQ(lineMesh->getSubMesh(0)->indexData->indexCount, 0);
//    v1::RenderOperation rop;
//    lineMesh->getSubMesh(0)->_getRenderOperation(rop);
//    ASSERT_EQ(rop.useIndexes, false);
//    ASSERT_EQ(lineMesh->getSubMesh(0)->vertexData->vertexCount, 2);
//
//    v1::MeshSerializer meshWriter;
//    meshWriter.exportMesh(lineMesh.get(), fileName);
//
//    mMeshMgr->remove(fileName);
//
//    ResourceGroupManager::getSingleton().addResourceLocation(".", "FileSystem");
//    v1::MeshPtr loadedLine = mMeshMgr->load(fileName, "General");
//
//    remove(fileName.c_str());
//
//    ASSERT_EQ(loadedLine->getNumSubMeshes(), 1);
//    ASSERT_EQ(loadedLine->getSubMesh(0)->indexData->indexCount, 0);
//    loadedLine->getSubMesh(0)->_getRenderOperation(rop);
//    ASSERT_EQ(rop.useIndexes, false);
//    ASSERT_EQ(lineMesh->getSubMesh(0)->vertexData->vertexCount, 2);
//
//    mMeshMgr->remove(fileName);
//}
//
//TEST_F(MeshWithoutIndexDataTests, testCreateLineList){
//    v1::ManualObject* lineList = OGRE_NEW v1::ManualObject(0, new ObjectMemoryManager(), nullptr);
//    lineList->setName("line");
//    lineList->begin("BaseWhiteNoLighting", OT_LINE_LIST);
//    lineList->position(0, 50, 0);
//    lineList->position(50, 100, 0);
//    lineList->position(50, 50, 0);
//    lineList->position(100, 100, 0);
//    lineList->position(0, 50, 0);
//    lineList->position(50, 50, 0);
//    lineList->end();
//    String fileName = "lineList.mesh";
//    v1::MeshPtr lineListMesh = lineList->convertToMesh(fileName);
//    OGRE_DELETE lineList;
//
//    ASSERT_EQ(lineListMesh->getNumSubMeshes(), 1);
//    ASSERT_EQ(lineListMesh->getSubMesh(0)->indexData->indexCount, 0);
//    v1::RenderOperation rop;
//    lineListMesh->getSubMesh(0)->_getRenderOperation(rop);
//    ASSERT_EQ(rop.useIndexes, false);
//    ASSERT_EQ(lineListMesh->getSubMesh(0)->vertexData->vertexCount, 6);
//
//    v1::MeshSerializer meshWriter;
//    meshWriter.exportMesh(lineListMesh.get(), fileName);
//
//    mMeshMgr->remove(fileName);
//
//    ResourceGroupManager::getSingleton().addResourceLocation(".", "FileSystem");
//    v1::MeshPtr loadedLineList = mMeshMgr->load(fileName, "General");
//
//    remove(fileName.c_str());
//
//    ASSERT_EQ(loadedLineList->getNumSubMeshes(), 1);
//    ASSERT_EQ(loadedLineList->getSubMesh(0)->indexData->indexCount, 0);
//    loadedLineList->getSubMesh(0)->_getRenderOperation(rop);
//    ASSERT_EQ(rop.useIndexes, false);
//    ASSERT_EQ(loadedLineList->getSubMesh(0)->vertexData->vertexCount, 6);
//
//    mMeshMgr->remove(fileName);
//}
//
//TEST_F(MeshWithoutIndexDataTests, testCreateLineStrip){
//    v1::ManualObject* lineStrip = OGRE_NEW v1::ManualObject(0, new ObjectMemoryManager(), nullptr);
//    lineStrip->setName("line");
//    lineStrip->begin("BaseWhiteNoLighting", OT_LINE_STRIP);
//    lineStrip->position(50, 100, 0);
//    lineStrip->position(0, 50, 0);
//    lineStrip->position(50, 50, 0);
//    lineStrip->position(100, 100, 0);
//    lineStrip->end();
//    String fileName = "lineStrip.mesh";
//    v1::MeshPtr lineStripMesh = lineStrip->convertToMesh(fileName);
//    OGRE_DELETE lineStrip;
//
//    ASSERT_EQ(lineStripMesh->getNumSubMeshes(), 1);
//    ASSERT_EQ(lineStripMesh->getSubMesh(0)->indexData->indexCount, 0);
//    v1::RenderOperation rop;
//    lineStripMesh->getSubMesh(0)->_getRenderOperation(rop);
//    ASSERT_EQ(rop.useIndexes, false);
//    ASSERT_EQ(lineStripMesh->getSubMesh(0)->vertexData->vertexCount, 4);
//
//    v1::MeshSerializer meshWriter;
//    meshWriter.exportMesh(lineStripMesh.get(), fileName);
//
//    mMeshMgr->remove(fileName);
//
//    ResourceGroupManager::getSingleton().addResourceLocation(".", "FileSystem");
//    v1::MeshPtr loadedLineStrip = mMeshMgr->load(fileName, "General");
//
//    remove(fileName.c_str());
//
//    ASSERT_EQ(loadedLineStrip->getNumSubMeshes(), 1);
//    ASSERT_EQ(loadedLineStrip->getSubMesh(0)->indexData->indexCount, 0);
//    loadedLineStrip->getSubMesh(0)->_getRenderOperation(rop);
//    ASSERT_EQ(rop.useIndexes, false);
//    ASSERT_EQ(loadedLineStrip->getSubMesh(0)->vertexData->vertexCount, 4);
//
//    mMeshMgr->remove(fileName);
//}
//
//TEST_F(MeshWithoutIndexDataTests, testCreatePointList){
//    v1::ManualObject* pointList = OGRE_NEW v1::ManualObject(0, new ObjectMemoryManager(), nullptr);
//    pointList->setName("line");
//    pointList->begin("BaseWhiteNoLighting", OT_POINT_LIST);
//    pointList->position(50, 100, 0);
//    pointList->position(0, 50, 0);
//    pointList->position(50, 50, 0);
//    pointList->position(100, 100, 0);
//    pointList->end();
//    String fileName = "pointList.mesh";
//    v1::MeshPtr pointListMesh = pointList->convertToMesh(fileName);
//    OGRE_DELETE pointList;
//
//    ASSERT_EQ(pointListMesh->getNumSubMeshes(), 1);
//    ASSERT_EQ(pointListMesh->getSubMesh(0)->indexData->indexCount, 0);
//    v1::RenderOperation rop;
//    pointListMesh->getSubMesh(0)->_getRenderOperation(rop);
//    ASSERT_EQ(rop.useIndexes, false);
//    ASSERT_EQ(pointListMesh->getSubMesh(0)->vertexData->vertexCount, 4);
//
//    v1::MeshSerializer meshWriter;
//    meshWriter.exportMesh(pointListMesh.get(), fileName);
//
//    mMeshMgr->remove(fileName);
//
//    ResourceGroupManager::getSingleton().addResourceLocation(".", "FileSystem");
//    v1::MeshPtr loadedPointList = mMeshMgr->load(fileName, "General");
//
//    remove(fileName.c_str());
//
//    ASSERT_EQ(loadedPointList->getNumSubMeshes(), 1);
//    ASSERT_EQ(loadedPointList->getSubMesh(0)->indexData->indexCount, 0);
//    loadedPointList->getSubMesh(0)->_getRenderOperation(rop);
//    ASSERT_EQ(rop.useIndexes, false);
//    ASSERT_EQ(loadedPointList->getSubMesh(0)->vertexData->vertexCount, 4);
//
//    mMeshMgr->remove(fileName);
//}
//
//TEST_F(MeshWithoutIndexDataTests, testCreateLineWithMaterial){
//    String matName = "lineMat";
//    MaterialPtr matPtr = MaterialManager::getSingleton().create(matName, "General");
//    Pass* pass = matPtr->getTechnique(0)->getPass(0);
//    pass->setDiffuse(1.0, 0.1, 0.1, 0);
//
//    v1::ManualObject* line = OGRE_NEW v1::ManualObject(0, new ObjectMemoryManager(), nullptr);
//    line->setName("line");
//    line->begin(matName, OT_LINE_LIST);
//    line->position(0, 50, 0);
//    line->position(50, 100, 0);
//    line->end();
//    String fileName = "lineWithMat.mesh";
//    v1::MeshPtr lineMesh = line->convertToMesh(fileName);
//    OGRE_DELETE line;
//
//    ASSERT_EQ(lineMesh->getNumSubMeshes(), 1);
//    ASSERT_EQ(lineMesh->getSubMesh(0)->indexData->indexCount, 0);
//    v1::RenderOperation rop;
//    lineMesh->getSubMesh(0)->_getRenderOperation(rop);
//    ASSERT_EQ(rop.useIndexes, false);
//    ASSERT_EQ(lineMesh->getSubMesh(0)->vertexData->vertexCount, 2);
//
//    v1::MeshSerializer meshWriter;
//    meshWriter.exportMesh(lineMesh.get(), fileName);
//    MaterialSerializer matWriter;
//    matWriter.exportMaterial(
//        MaterialManager::getSingleton().getByName(matName),
//        matName + ".material");
//
//    mMeshMgr->remove(fileName);
//
//    ResourceGroupManager::getSingleton().addResourceLocation(".", "FileSystem");
//    v1::MeshPtr loadedLine = mMeshMgr->load(fileName, "General");
//
//    remove(fileName.c_str());
//    remove((matName + ".material").c_str());
//
//    ASSERT_EQ(loadedLine->getNumSubMeshes(), 1);
//    ASSERT_EQ(loadedLine->getSubMesh(0)->indexData->indexCount, 0);
//    loadedLine->getSubMesh(0)->_getRenderOperation(rop);
//    ASSERT_EQ(rop.useIndexes, false);
//    ASSERT_EQ(lineMesh->getSubMesh(0)->vertexData->vertexCount, 2);
//
//    mMeshMgr->remove(fileName);
//}
//
//TEST_F(MeshWithoutIndexDataTests, testCreateMesh){
//    String fileName = "indexMix.mesh";
//    createMeshWithMaterial(fileName);
//    v1::MeshPtr mesh = std::static_pointer_cast<v1::Mesh>(mMeshMgr->getByName(fileName));
//
//    ASSERT_EQ(mesh->getNumSubMeshes(), 4);
//    v1::RenderOperation rop;
//    for (int i=0; i<4; ++i)
//    {
//        mesh->getSubMesh(i)->_getRenderOperation(rop);
//        // First submesh has indexes, the others does not.
//        ASSERT_EQ(rop.useIndexes, (i == 0));
//    }
//
//    v1::MeshSerializer meshWriter;
//    meshWriter.exportMesh(mesh.get(), fileName);
//
//    mMeshMgr->remove(fileName);
//
//    ResourceGroupManager::getSingleton().addResourceLocation(".", "FileSystem");
//    v1::MeshPtr loadedMesh = mMeshMgr->load(fileName, "General");
//
//    remove(fileName.c_str());
//
//    ASSERT_EQ(loadedMesh->getNumSubMeshes(), 4);
//
//    mMeshMgr->remove(fileName);
//}
//
//TEST_F(MeshWithoutIndexDataTests, testCloneMesh){
//    String originalName = "toClone.mesh";
//    createMeshWithMaterial(originalName);
//    v1::MeshPtr mesh = std::static_pointer_cast<v1::Mesh>( mMeshMgr->getByName(originalName));
//
//    String fileName = "clone.mesh";
//    v1::MeshPtr clone = mesh->clone(fileName);
//    ASSERT_EQ(mesh->getNumSubMeshes(), 4);
//
//    v1::MeshSerializer meshWriter;
//    meshWriter.exportMesh(mesh.get(), fileName);
//
//    mMeshMgr->remove(fileName);
//
//    ResourceGroupManager::getSingleton().addResourceLocation(".", "FileSystem");
//    v1::MeshPtr loadedMesh = mMeshMgr->load(fileName, "General");
//
//    remove(fileName.c_str());
//
//    ASSERT_EQ(loadedMesh->getNumSubMeshes(), 4);
//
//    mMeshMgr->remove(fileName);
//}
//
//TEST_F(MeshWithoutIndexDataTests, testEdgeList){
//    String fileName = "testEdgeList.mesh";
//    v1::ManualObject* line = OGRE_NEW v1::ManualObject(0, new ObjectMemoryManager(), nullptr);
//    line->setName("line");
//    line->begin("BaseWhiteNoLighting", OT_LINE_LIST);
//    line->position(0, 50, 0);
//    line->position(50, 100, 0);
//    line->end();
//    v1::MeshPtr mesh = line->convertToMesh(fileName);
//    OGRE_DELETE line;
//
//    // whole mesh must not contain index data, for this test
//    ASSERT_EQ(mesh->getNumSubMeshes(), 1);
//    ASSERT_EQ(mesh->getSubMesh(0)->indexData->indexCount, 0);
//
//    mesh->buildEdgeList();
//    v1::MeshSerializer meshWriter;
//    // if it does not crash here, test is passed
//    meshWriter.exportMesh(mesh.get(), fileName);
//
//    remove(fileName.c_str());
//
//    mMeshMgr->remove(fileName);
//}
//
//TEST_F(MeshWithoutIndexDataTests, testGenerateExtremes){
//    String fileName = "testGenerateExtremes.mesh";
//    createMeshWithMaterial(fileName);
//    v1::MeshPtr mesh = std::static_pointer_cast<v1::Mesh>( mMeshMgr->getByName(fileName));
//
//    const size_t NUM_EXTREMES = 4;
//    for (ushort i = 0; i < mesh->getNumSubMeshes(); ++i)
//    {
//        mesh->getSubMesh(i)->generateExtremes(NUM_EXTREMES);
//    }
//    for (ushort i = 0; i < mesh->getNumSubMeshes(); ++i)
//    {
//        v1::SubMesh* subMesh = mesh->getSubMesh(i);
//        // According to generateExtremes, extremes are built based upon the bounding box indices.
//        // But it also creates indices for all bounding boxes even if the mesh does not have any.
//        // So...there should always be some extremity points. The number of which may vary.
//        if (subMesh->indexData->indexCount > 0)
//        {
//            ASSERT_EQ(subMesh->extremityPoints.size(), NUM_EXTREMES);
//        }
//    }
//
//    mMeshMgr->remove(fileName);
//}
//
//TEST_F(MeshWithoutIndexDataTests, testBuildTangentVectors){
//    String fileName = "testBuildTangentVectors.mesh";
//    createMeshWithMaterial(fileName);
//    v1::MeshPtr mesh = std::static_pointer_cast<v1::Mesh>( mMeshMgr->getByName(fileName));
//
//    try
//    {
//        // Make sure correct exception is thrown
//        mesh->buildTangentVectors();
//        ASSERT_EQ(false, false) << "Expected InvalidParametersException!";
//    }
//    catch (const InvalidParametersException&)
//    {
//        // Ok
//    }
//
//    mMeshMgr->remove(fileName);
//}
