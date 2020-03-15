////
////  MeshSerializer_test.cpp
////  GUnitTest
////
////  Created by 杨丰 on 2019/12/1.
////  Copyright © 2019 杨丰. All rights reserved.
////
//
//#include "../src.gfx/OgrePrerequisites.h"
//#include "../src.gfx/OgreDistanceLodStrategy.h"
//#include "../src.gfx/OgreLodStrategyManager.h"
//#include "../src.gfx/OgreDefaultHardwareBufferManager.h"
//#include "../src.gfx/OgreMesh.h"
//#include "../src.gfx/OgreMeshManager.h"
//#include "../src.gfx/OgreSkeleton.h"
//#include "../src.gfx/OgreOldSkeletonManager.h"
//#include "../src.gfx/OgreMeshSerializer.h"
//#include "../src.gfx/OgreFileSystemLayer.h"
//#include "../src.gfx/OgreArchiveManager.h"
//#include "../src.gfx/OgreFileSystem.h"
//#include "../src.gfx/OgreConfigFile.h"
//#include "../src.gfx/OgreMaterialManager.h"
//
//#include "../external/gtest/include/gtest/gtest.h"
//
//using namespace Ogre;
//using namespace Ogre::v1;
//
//class MeshSerializerTests : public testing::Test {
//public:
//    void getResourceFullPath(const ResourcePtr& resource, String& outPath)
//    {
//        ResourceGroupManager& resourceGroupMgr = ResourceGroupManager::getSingleton();
//        String group = resource->getGroup();
//        String name = resource->getName();
//        FileInfo* info = NULL;
//        FileInfoListPtr locPtr = resourceGroupMgr.listResourceFileInfo(group);
//        FileInfoList::iterator it, itEnd;
//        it = locPtr->begin();
//        itEnd = locPtr->end();
//        for (; it != itEnd; it++) {
//            if (stricmp(name.c_str(), it->filename.c_str()) == 0) {
//                info = &*it;
//                break;
//            }
//        }
//        if(!info) {
//            outPath = name;
//            return;
//        }
//        outPath = info->archive->getName();
//        if (outPath[outPath .size()-1] != '/' && outPath[outPath .size()-1] != '\\') {
//            outPath += '/';
//        }
//        outPath += info->path;
//        if (outPath[outPath .size()-1] != '/' && outPath[outPath .size()-1] != '\\') {
//            outPath += '/';
//        }
//        outPath += info->filename;
//
//        assert(info->archive->getType() == "FileSystem");
//    }
//
//    bool copyFile(const String& srcPath, const String& dstPath)
//    {
//        std::ifstream src(srcPath.c_str(), std::ios::binary);
//        if (!src.is_open()) {
//            return false;
//        }
//        std::ofstream dst(dstPath.c_str(), std::ios::binary);
//        if (!dst.is_open()) {
//            return false;
//        }
//
//        dst << src.rdbuf();
//        return true;
//    }
//
//public:
//    void SetUp() override{
//        mErrorFactor = 0.05;
//
//        mFSLayer = OGRE_NEW_T(Ogre::FileSystemLayer, Ogre::MEMCATEGORY_GENERAL)(OGRE_VERSION_NAME);
//
//        OGRE_NEW ResourceGroupManager();
//        OGRE_NEW LodStrategyManager();
//        OGRE_NEW DefaultHardwareBufferManager();
//        OGRE_NEW v1::MeshManager();
//        OGRE_NEW OldSkeletonManager();
//        ArchiveManager* archiveMgr = OGRE_NEW ArchiveManager();
//        archiveMgr->addArchiveFactory(OGRE_NEW FileSystemArchiveFactory());
//
//        MaterialManager* matMgr = OGRE_NEW MaterialManager();
//        matMgr->initialise();
//
//        // Load resource paths from config file
//        ConfigFile cf;
//        String resourcesPath;
//
//#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE || OGRE_PLATFORM == OGRE_PLATFORM_WIN32
//        resourcesPath = mFSLayer->getConfigFilePath("resources.cfg");
//#else
//        resourcesPath = mFSLayer->getConfigFilePath("bin/resources.cfg");
//#endif
//
//        // Go through all sections & settings in the file
//        cf.load(resourcesPath);
//        ConfigFile::SectionIterator seci = cf.getSectionIterator();
//
//        String secName, typeName, archName;
//        while (seci.hasMoreElements()) {
//            secName = seci.peekNextKey();
//            ConfigFile::SettingsMultiMap* settings = seci.getNext();
//            ConfigFile::SettingsMultiMap::iterator i;
//            for (i = settings->begin(); i != settings->end(); ++i) {
//                typeName = i->first;
//                archName = i->second;
//                if (typeName == "FileSystem") {
//                    ResourceGroupManager::getSingleton().addResourceLocation(
//                                                                             archName, typeName, secName);
//                }
//            }
//        }
//
//        mMesh = v1::MeshManager::getSingleton().load("knot.mesh", ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME);
//
//        getResourceFullPath(mMesh, mMeshFullPath);
//        if (!copyFile(mMeshFullPath + ".bak", mMeshFullPath)) {
//            // If there is no backup, create one.
//            copyFile(mMeshFullPath, mMeshFullPath + ".bak");
//        }
//        mSkeletonFullPath = "";
//        mSkeleton = std::static_pointer_cast<Skeleton>( OldSkeletonManager::getSingleton().load("jaiqua.skeleton", "Popular"));
//        getResourceFullPath(mSkeleton, mSkeletonFullPath);
//        if (!copyFile(mSkeletonFullPath + ".bak", mSkeletonFullPath)) {
//            // If there is no backup, create one.
//            copyFile(mSkeletonFullPath, mSkeletonFullPath + ".bak");
//        }
//
//        mMesh->reload();
//
//        //#ifdef OGRE_BUILD_COMPONENT_MESHLODGENERATOR
//        //        {
//        //            MeshLodGenerator().generateAutoconfiguredLodLevels(mMesh);
//        //        }
//        //#endif /* ifdef OGRE_BUILD_COMPONENT_MESHLODGENERATOR */
//
//        mOrigMesh = mMesh->clone(mMesh->getName() + ".orig.mesh", mMesh->getGroup());
//    }
//
//    void TearDown() override{
//        // Copy back original file.
//        if (!mMeshFullPath.empty()) {
//            copyFile(mMeshFullPath + ".bak", mMeshFullPath);
//        }
//        if (!mSkeletonFullPath.empty()) {
//            copyFile(mSkeletonFullPath + ".bak", mSkeletonFullPath);
//        }
//        if (mMesh != nullptr) {
//            mMesh->unload();
//            mMesh.reset();
//        }
//        if (mOrigMesh != nullptr) {
//            mOrigMesh->unload();
//            mOrigMesh.reset();
//        }
//        if (mSkeleton != nullptr) {
//            mSkeleton->unload();
//            mSkeleton.reset();
//        }
//
//        OGRE_DELETE v1::MeshManager::getSingletonPtr();
//        OGRE_DELETE OldSkeletonManager::getSingletonPtr();
//        OGRE_DELETE DefaultHardwareBufferManager::getSingletonPtr();
//        OGRE_DELETE ArchiveManager::getSingletonPtr();
//        OGRE_DELETE MaterialManager::getSingletonPtr();
//        OGRE_DELETE LodStrategyManager::getSingletonPtr();
//        OGRE_DELETE ResourceGroupManager::getSingletonPtr();
//        OGRE_DELETE_T(mFSLayer, FileSystemLayer, Ogre::MEMCATEGORY_GENERAL);
//    }
//
//public:
//    v1::MeshPtr mMesh;
//    v1::MeshPtr mOrigMesh;
//    String mMeshFullPath;
//    String mSkeletonFullPath;
//    SkeletonPtr mSkeleton;
//    Real mErrorFactor;
//    FileSystemLayer* mFSLayer;
//
//public:
//    bool isEqual(Real a, Real b)
//    {
//        Real absoluteError = std::abs(a * mErrorFactor);
//        return ((a - absoluteError) <= b) && ((a + absoluteError) >= b);
//    }
//
//    bool isEqual(const Vector3& a, const Vector3& b)
//    {
//        return isEqual(a.x, b.x) && isEqual(a.y, b.y) && isEqual(a.z, b.z);
//    }
//
//    bool isLodMixed(const v1::Mesh* pMesh)
//    {
//        if (!pMesh->hasManualLodLevel()) {
//            return false;
//        }
//
//        unsigned short numLods = pMesh->getNumLodLevels();
//        for (unsigned short i = 1; i < numLods; ++i) {
//            if (!pMesh->_isManualLodLevel(i)) {
//                return true;
//            }
//        }
//
//        return false;
//    }
//    //--------------------------------------------------------------------------
//    template<typename T>
//    bool isContainerClone(T& a, T& b)
//    {
//        return a.size() == b.size() && std::equal(a.begin(), a.end(), b.begin());
//    }
//    template<typename K, typename V>
//    bool isHashMapClone(const unordered_map<K, V>& a, const unordered_map<K, V>& b)
//    {
//        // if you recreate a HashMap with same elements, then iteration order may differ!
//        // So isContainerClone is not always working on HashMap.
//        if (a.size() != b.size()) {
//            return false;
//        }
//        typename unordered_map<K, V>::const_iterator it, itFind, itEnd;
//        it = a.begin();
//        itEnd = a.end();
//        for (; it != itEnd; it++) {
//            itFind = b.find(it->first);
//            if (itFind == b.end() || itFind->second != it->second) {
//                return false;
//            }
//        }
//        return true;
//    }
//    //--------------------------------------------------------------------------
//    void assertVertexDataClone(VertexData* a, VertexData* b,
//                               MeshVersion version = MESH_VERSION_LATEST)
//    {
//        ASSERT_EQ((a == NULL), (b == NULL));
//        if (a) {
//            // compare bindings
//            {
//                const VertexBufferBinding::VertexBufferBindingMap& aBindings = a->vertexBufferBinding->getBindings();
//                const VertexBufferBinding::VertexBufferBindingMap& bBindings = b->vertexBufferBinding->getBindings();
//                ASSERT_EQ(aBindings.size(), bBindings.size());
//                typedef VertexBufferBinding::VertexBufferBindingMap::const_iterator bindingIterator;
//                bindingIterator aIt = aBindings.begin();
//                bindingIterator aEndIt = aBindings.end();
//                bindingIterator bIt = bBindings.begin();
//                for (; aIt != aEndIt; aIt++, bIt++) {
//                    ASSERT_EQ(aIt->first, bIt->first);
//                    ASSERT_EQ((aIt->second.get() == NULL), (bIt->second.get() == NULL));
//                    if (a) {
//                        ASSERT_EQ(aIt->second->getManager(), bIt->second->getManager());
//                        ASSERT_EQ(aIt->second->getNumVertices(), bIt->second->getNumVertices());
//                    }
//                }
//            }
//
//            {
//                const VertexDeclaration::VertexElementList& aElements = a->vertexDeclaration->getElements();
//                const VertexDeclaration::VertexElementList& bElements = a->vertexDeclaration->getElements();
//                ASSERT_EQ(aElements.size(), bElements.size());
//                typedef VertexDeclaration::VertexElementList::const_iterator bindingIterator;
//                bindingIterator aIt = aElements.begin();
//                bindingIterator aEndIt = aElements.end();
//                bindingIterator bIt;
//                for (; aIt != aEndIt; aIt++) {
//                    bIt = std::find(bElements.begin(), bElements.end(), *aIt);
//                    ASSERT_EQ(bIt != bElements.end(), true);
//
//    #ifndef OGRE_TEST_XMLSERIALIZER
//                    const VertexElement& aElem = *aIt;
//                    const VertexElement& bElem = *bIt;
//                    HardwareVertexBufferSharedPtr abuf = a->vertexBufferBinding->getBuffer(aElem.getSource());
//                    HardwareVertexBufferSharedPtr bbuf = b->vertexBufferBinding->getBuffer(bElem.getSource());
//                    unsigned char* avertex = static_cast<unsigned char*>(abuf->lock(HardwareBuffer::HBL_READ_ONLY));
//                    unsigned char* bvertex = static_cast<unsigned char*>(bbuf->lock(HardwareBuffer::HBL_READ_ONLY));
//                    size_t avSize = abuf->getVertexSize();
//                    size_t bvSize = bbuf->getVertexSize();
//                    size_t elemSize = VertexElement::getTypeSize(aElem.getType());
//                    unsigned char* avEnd = avertex + a->vertexCount * avSize;
//                    bool error = false;
//                    for (; avertex < avEnd; avertex += avSize, bvertex += bvSize) {
//                        float* afloat, * bfloat;
//                        aElem.baseVertexPointerToElement(avertex, &afloat);
//                        bElem.baseVertexPointerToElement(bvertex, &bfloat);
//                        error |= (memcmp(afloat, bfloat, elemSize) != 0);
//                    }
//                    abuf->unlock();
//                    bbuf->unlock();
//                    ASSERT_EQ(!error, true) << "Content of vertex buffer differs!";
//    #endif /* ifndef OGRE_TEST_XMLSERIALIZER */
//                }
//            }
//
//            ASSERT_EQ(a->vertexStart, b->vertexStart);
//            ASSERT_EQ(a->vertexCount, b->vertexCount);
//            ASSERT_EQ(a->hwAnimDataItemsUsed, b->hwAnimDataItemsUsed);
//
//            // Compare hwAnimationData
//            {
//                const VertexData::HardwareAnimationDataList& aAnimData = a->hwAnimationDataList;
//                const VertexData::HardwareAnimationDataList& bAnimData = b->hwAnimationDataList;
//                ASSERT_EQ(aAnimData.size(), bAnimData.size());
//                typedef VertexData::HardwareAnimationDataList::const_iterator bindingIterator;
//                bindingIterator aIt = aAnimData.begin();
//                bindingIterator aEndIt = aAnimData.end();
//                bindingIterator bIt = bAnimData.begin();
//                for (; aIt != aEndIt; aIt++, bIt++) {
//                    ASSERT_EQ(aIt->parametric, bIt->parametric);
//                    ASSERT_EQ(aIt->targetBufferIndex, bIt->targetBufferIndex);
//                }
//            }
//        }
//    }
//
//    void assertEdgeDataClone(EdgeData* a, EdgeData* b,
//                             MeshVersion version = MESH_VERSION_LATEST)
//    {
//        ASSERT_EQ((a == NULL), (b == NULL));
//        if (a) {
//            ASSERT_EQ(a->isClosed, b->isClosed);
//            ASSERT_EQ(isContainerClone(a->triangleFaceNormals, b->triangleFaceNormals), true);
//            ASSERT_EQ(isContainerClone(a->triangleLightFacings, b->triangleLightFacings), true);
//            // TODO: Compare triangles and edgeGroups in more detail.
//            ASSERT_EQ(a->triangles.size(), b->triangles.size());
//            ASSERT_EQ(a->edgeGroups.size(), b->edgeGroups.size());
//        }
//    }
//
//    void assertIndexDataClone(IndexData* a, IndexData* b,
//                              MeshVersion version = MESH_VERSION_LATEST)
//    {
//        ASSERT_EQ((a == NULL), (b == NULL));
//        if (a) {
//            ASSERT_EQ(a->indexCount, b->indexCount);
//            // CPPUNIT_ASSERT(a->indexStart == b->indexStart);
//            ASSERT_EQ((a->indexBuffer.get() == NULL), (b->indexBuffer.get() == NULL));
//            if (a->indexBuffer != nullptr) {
//                ASSERT_EQ(a->indexBuffer->getManager(), b->indexBuffer->getManager());
//                // CPPUNIT_ASSERT(a->indexBuffer->getNumIndexes() == b->indexBuffer->getNumIndexes());
//                ASSERT_EQ(a->indexBuffer->getIndexSize(), b->indexBuffer->getIndexSize());
//                ASSERT_EQ(a->indexBuffer->getType(), b->indexBuffer->getType());
//
//                char* abuf = (char*) a->indexBuffer->lock(HardwareBuffer::HBL_READ_ONLY);
//                char* bbuf = (char*) b->indexBuffer->lock(HardwareBuffer::HBL_READ_ONLY);
//                size_t size = a->indexBuffer->getIndexSize();
//                char* astart = abuf + a->indexStart * size;
//                char* bstart = bbuf + b->indexStart * size;
//                ASSERT_EQ(memcmp(astart, bstart, a->indexCount * size), 0);
//                a->indexBuffer->unlock();
//                b->indexBuffer->unlock();
//            }
//        }
//    }
//
//    void assertLodUsageClone(const MeshLodUsage& a, const MeshLodUsage& b,
//                             MeshVersion version = MESH_VERSION_LATEST)
//    {
//        ASSERT_EQ(a.manualName, b.manualName);
//        ASSERT_EQ(isEqual(a.userValue, b.userValue), true);
//        ASSERT_EQ(isEqual(a.value, b.value), true);
//    }
//
//    void assertMeshClone(v1::Mesh* a, v1::Mesh* b,
//                         MeshVersion version = MESH_VERSION_LATEST)
//    {
//        // TODO: Compare skeleton
//        // TODO: Compare animations
//        // TODO: Compare pose animations
//
//        // CPPUNIT_ASSERT(a->getGroup() == b->getGroup());
//        // CPPUNIT_ASSERT(a->getName() == b->getName());
//
//#ifndef OGRE_TEST_XMLSERIALIZER
//        // XML serializer fails on these!
//        ASSERT_EQ(isEqual(a->getBoundingSphereRadius(), b->getBoundingSphereRadius()), true);
//        ASSERT_EQ(isEqual(a->getBounds().getMinimum(), b->getBounds().getMinimum()), true);
//        ASSERT_EQ(isEqual(a->getBounds().getMaximum(), b->getBounds().getMaximum()), true);
//#else
//        StringStream str;
//        Real val1 = a->getBoundingSphereRadius();
//        Real val2 = b->getBoundingSphereRadius();
//        Real diff = (val1 > val2) ? (val1 / val2) : (val2 / val1);
//        if (diff > 1.1) {
//            str << "bound sphere diff: " << diff << std::endl;
//        }
//        val1 = a->getBounds().getMinimum().length();
//        val2 = b->getBounds().getMinimum().length();
//        diff = (val1 > val2) ? (val1 / val2) : (val2 / val1);
//        if (diff > 1.1) {
//            str << "bound min diff: " << diff << std::endl;
//        }
//        val1 = a->getBounds().getMaximum().length();
//        val2 = b->getBounds().getMaximum().length();
//        diff = (val1 > val2) ? (val1 / val2) : (val2 / val1);
//        if (diff > 1.1) {
//            str << "bound max diff: " << diff << std::endl;
//        }
//        if (!str.str().empty()) {
//            StringStream str2;
//            str2 << std::endl << "Mesh name: " << b->getName() << std::endl;
//            str2 << str.str();
//            std::cout << str2.str();
//            // OutputDebugStringA(str2.str().c_str());
//        }
//#endif /* ifndef OGRE_TEST_XMLSERIALIZER */
//
//        // AutobuildEdgeLists is not saved to mesh file. You need to set it after loading a mesh!
//        // CPPUNIT_ASSERT(a->getAutoBuildEdgeLists() == b->getAutoBuildEdgeLists());
//        ASSERT_EQ(isHashMapClone(a->getSubMeshNameMap(), b->getSubMeshNameMap()), true);
//
//        assertVertexDataClone(a->sharedVertexData, b->sharedVertexData);
//        ASSERT_EQ(a->getCreator(), b->getCreator());
//        ASSERT_EQ(a->getIndexBufferUsage(), b->getIndexBufferUsage());
//        ASSERT_EQ(a->getSharedVertexDataAnimationIncludesNormals(), b->getSharedVertexDataAnimationIncludesNormals());
//        ASSERT_EQ(a->getSharedVertexDataAnimationType(), b->getSharedVertexDataAnimationType());
//        ASSERT_EQ(a->getVertexBufferUsage(), b->getVertexBufferUsage());
//        ASSERT_EQ(a->hasVertexAnimation(), b->hasVertexAnimation());
//
//#ifndef OGRE_TEST_XMLSERIALIZER
//        ASSERT_EQ(a->isEdgeListBuilt(), b->isEdgeListBuilt()); // <== OgreXMLSerializer is doing post processing to generate edgelists!
//#endif // !OGRE_TEST_XMLSERIALIZER
//
//        if ((a->getNumLodLevels() > 1 || b->getNumLodLevels() > 1) &&
//            ((version < MESH_VERSION_1_8 || (!isLodMixed(a) && !isLodMixed(b))) && // mixed lod only supported in v1.10+
//             (version < MESH_VERSION_1_4 || (a->getLodStrategyName() != DistanceLodStrategy::getSingletonPtr()->getName() &&
//                                             b->getLodStrategyName() != DistanceLodStrategy::getSingletonPtr()->getName())))) { // Lod Strategy only supported in v1.41+
//            ASSERT_EQ(a->getNumLodLevels(), b->getNumLodLevels());
//            ASSERT_EQ(a->hasManualLodLevel(), b->hasManualLodLevel());
//            ASSERT_EQ(a->getLodStrategyName(), b->getLodStrategyName());
//
//            int numLods = a->getNumLodLevels();
//            for (int i = 0; i < numLods; i++) {
//                if (version != MESH_VERSION_1_0 && a->getAutoBuildEdgeLists() == b->getAutoBuildEdgeLists()) {
//                    assertEdgeDataClone(a->getEdgeList(i), b->getEdgeList(i));
//                } else if (a->getLodLevel(i).edgeData != NULL && b->getLodLevel(i).edgeData != NULL) {
//                    assertEdgeDataClone(a->getLodLevel(i).edgeData, b->getLodLevel(i).edgeData);
//                }
//                assertLodUsageClone(a->getLodLevel(i), b->getLodLevel(i));
//            }
//        }
//
//        ASSERT_EQ(a->getNumSubMeshes(), b->getNumSubMeshes());
//        int numLods = std::min(a->getNumLodLevels(), b->getNumLodLevels());
//        int numSubmeshes = a->getNumSubMeshes();
//        for (int i = 0; i < numSubmeshes; i++) {
//            v1::SubMesh* aSubmesh = a->getSubMesh(i);
//            v1::SubMesh* bSubmesh = b->getSubMesh(i);
//
//            ASSERT_EQ(aSubmesh->getMaterialName(), bSubmesh->getMaterialName());
//            ASSERT_EQ(aSubmesh->isMatInitialised(), bSubmesh->isMatInitialised());
//            ASSERT_EQ(aSubmesh->useSharedVertices, bSubmesh->useSharedVertices);
//            ASSERT_EQ(aSubmesh->getVertexAnimationIncludesNormals(), bSubmesh->getVertexAnimationIncludesNormals());
//            ASSERT_EQ(aSubmesh->getVertexAnimationType(), bSubmesh->getVertexAnimationType());
//            ASSERT_EQ(aSubmesh->getTextureAliasCount(), bSubmesh->getTextureAliasCount());
//            ASSERT_EQ(isContainerClone(aSubmesh->blendIndexToBoneIndexMap, bSubmesh->blendIndexToBoneIndexMap), true);
//            // TODO: Compare getBoneAssignments and getTextureAliases
//            for (int n = 0; n < numLods; n++) {
//                if (a->_isManualLodLevel(n)) {
//                    continue;
//                }
//                RenderOperation aop, bop;
//                aSubmesh->_getRenderOperation(aop, n);
//                bSubmesh->_getRenderOperation(bop, n);
//                assertIndexDataClone(aop.indexData, bop.indexData);
//                assertVertexDataClone(aop.vertexData, bop.vertexData);
//                ASSERT_EQ(aop.operationType, bop.operationType);
//                ASSERT_EQ(aop.useIndexes, bop.useIndexes);
//            }
//        }
//    }
//};
//
//TEST_F(MeshSerializerTests, testMesh_clone){
//    v1::MeshPtr cloneMesh = mMesh->clone(mMesh->getName() + ".clone.mesh", mMesh->getGroup());
//    assertMeshClone(mMesh.get(), cloneMesh.get());
//}
