/*
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define ROOT_NAME "testThinClientAfterRegionLive"

#include "fw_dunit.hpp"
#include "ThinClientHelper.hpp"
#include <string>
#define CLIENT1 s1p1
#define CLIENT2 s1p2
#define SERVER1 s2p1
#include <geode/CacheListener.hpp>
// CacheHelper* cacheHelper = nullptr;
static bool isLocator = false;
static bool isLocalServer = true;
static int numberOfLocators = 1;
static bool isRegionLive[4] = {false, false, false, false};
static bool isRegionDead[4] = {false, false, false, false};
const char* locatorsG =
    CacheHelper::getLocatorHostPort(isLocator, isLocalServer, numberOfLocators);
using namespace apache::geode::client;
using namespace test;
class DisconnectCacheListioner : public CacheListener {
  int m_index;

 public:
  explicit DisconnectCacheListioner(int index) { m_index = index; };

  void afterRegionDisconnected(const std::shared_ptr<Region>& region) {
    isRegionDead[m_index] = true;
    LOG("After Region Disconnected event received");
  }
  void afterRegionLive(const RegionEvent& event) {
    isRegionLive[m_index] = true;
    LOG("After region live received ");
  }
};

auto cptr1 = std::make_shared<DisconnectCacheListioner>(0);
auto cptr2 = std::make_shared<DisconnectCacheListioner>(1);
auto cptr3 = std::make_shared<DisconnectCacheListioner>(2);
auto cptr4 = std::make_shared<DisconnectCacheListioner>(3);

#include "LocatorHelper.hpp"

void createPooledRegionMine(bool callReadyForEventsAPI = false) {
  auto& poolManager = getHelper()->getCache()->getPoolManager();
  auto poolFacPtr = poolManager.createFactory();
  poolFacPtr->setSubscriptionEnabled(true);
  getHelper()->addServerLocatorEPs(locatorsG, poolFacPtr);
  if ((poolManager.find("__TEST_POOL1__")) ==
      nullptr) {  // Pool does not exist with the same name.
    auto pptr = poolFacPtr->create("__TEST_POOL1__");
  }
  SLEEP(10000);
  AttributesFactory af;
  af.setCachingEnabled(true);
  af.setLruEntriesLimit(0);
  af.setEntryIdleTimeout(ExpirationAction::DESTROY, std::chrono::seconds(0));
  af.setEntryTimeToLive(ExpirationAction::DESTROY, std::chrono::seconds(0));
  af.setRegionIdleTimeout(ExpirationAction::DESTROY, std::chrono::seconds(0));
  af.setRegionTimeToLive(ExpirationAction::DESTROY, std::chrono::seconds(0));
  af.setPoolName("__TEST_POOL1__");
  LOG("poolName = ");
  LOG("__TEST_POOL1__");
  af.setCacheListener(cptr1);
  std::shared_ptr<RegionAttributes> rattrsPtr1 = af.createRegionAttributes();
  af.setCacheListener(cptr2);
  std::shared_ptr<RegionAttributes> rattrsPtr2 = af.createRegionAttributes();
  af.setCacheListener(cptr3);
  std::shared_ptr<RegionAttributes> rattrsPtr3 = af.createRegionAttributes();
  af.setCacheListener(cptr4);
  std::shared_ptr<RegionAttributes> rattrsPtr4 = af.createRegionAttributes();
  CacheImpl* cacheImpl =
      CacheRegionHelper::getCacheImpl(getHelper()->cachePtr.get());
  std::shared_ptr<Region> region1;
  cacheImpl->createRegion(regionNames[0], rattrsPtr1, region1);
  std::shared_ptr<Region> region2;
  cacheImpl->createRegion(regionNames[1], rattrsPtr2, region2);
  auto subregion1 = region1->createSubregion(regionNames[0], rattrsPtr3);
  auto subregion2 = region2->createSubregion(regionNames[1], rattrsPtr4);
  if (callReadyForEventsAPI) {
    getHelper()->cachePtr->readyForEvents();
  }
}
DUNIT_TASK_DEFINITION(CLIENT1, SetupClient1_Pool_Locator)
  {
    initClient(true);
    createPooledRegionMine(false);
  }
END_TASK_DEFINITION

DUNIT_TASK_DEFINITION(CLIENT1, NotAutoReady_CallReadyForEvents)
  {
    std::shared_ptr<Properties> props(Properties::create());
    props->insert("auto-ready-for-events", "false");
    initClient(true, props);
    createPooledRegionMine(true);
  }
END_TASK_DEFINITION

DUNIT_TASK_DEFINITION(CLIENT1, NotAutoReady_DontCallReadyForEvents)
  {
    std::shared_ptr<Properties> props(Properties::create());
    props->insert("auto-ready-for-events", "false");
    initClient(true, props);
    createPooledRegionMine(false);
  }
END_TASK_DEFINITION

DUNIT_TASK_DEFINITION(CLIENT1, Verify)
  {
    SLEEP(10000);
    ASSERT(isRegionLive[0], "Client should have gotten region1 live event");
    isRegionLive[0] = false;
    ASSERT(isRegionLive[1], "Client should have gotten region2 live event");
    isRegionLive[1] = false;
    ASSERT(isRegionLive[2], "Client should have gotten subregion1 live event");
    isRegionLive[2] = false;
    ASSERT(isRegionLive[3], "Client should have gotten subregion2 live event");
    isRegionLive[3] = false;
  }
END_TASK_DEFINITION

DUNIT_TASK_DEFINITION(CLIENT1, VerifyNotLive)
  {
    SLEEP(10000);
    ASSERT(!isRegionLive[0],
           "Client should not have gotten region1 live event");
    ASSERT(!isRegionLive[1],
           "Client should not have gotten region2 live event");
    ASSERT(!isRegionLive[2],
           "Client should not have gotten subregion1 live event");
    ASSERT(!isRegionLive[3],
           "Client should not have gotten subregion2 live event");
  }
END_TASK_DEFINITION

DUNIT_TASK_DEFINITION(CLIENT1, VerifyDead)
  {
    SLEEP(10000);
    ASSERT(isRegionDead[0],
           "Client should have gotten region1 disconnected event");
    isRegionDead[0] = false;
    ASSERT(isRegionDead[1],
           "Client should have gotten region2 disconnected event");
    isRegionDead[1] = false;
    ASSERT(isRegionDead[2],
           "Client should have gotten subregion1 disconnected event");
    isRegionDead[2] = false;
    ASSERT(isRegionDead[3],
           "Client should have gotten subregion2 disconnected event");
    isRegionDead[3] = false;
  }
END_TASK_DEFINITION

DUNIT_TASK_DEFINITION(CLIENT1, VerifyNotDead)
  {
    SLEEP(10000);
    ASSERT(!isRegionDead[0],
           "Client should not have gotten region1 disconnected event");
    ASSERT(!isRegionDead[1],
           "Client should not have gotten region2 disconnected event");
    ASSERT(!isRegionDead[2],
           "Client should not have gotten subregion1 disconnected event");
    ASSERT(!isRegionDead[3],
           "Client should not have gotten subregion2 disconnected event");
  }
END_TASK_DEFINITION

DUNIT_TASK_DEFINITION(CLIENT1, populateServer)
  {
    SLEEP(10000);
    auto regPtr = getHelper()->getRegion(regionNames[0]);
    auto keyPtr = createKey("PXR");
    auto valPtr = CacheableString::create("PXR1");
    regPtr->create(keyPtr, valPtr);
  }
END_TASK_DEFINITION

DUNIT_TASK_DEFINITION(SERVER1, StopServer)
  {
    if (isLocalServer) CacheHelper::closeServer(1);
    LOG("SERVER stopped");
  }
END_TASK_DEFINITION

DUNIT_TASK_DEFINITION(CLIENT1, StopClient1)
  {
    cleanProc();
    LOG("CLIENT1 stopped");
  }
END_TASK_DEFINITION

DUNIT_MAIN
  {
    CALL_TASK(CreateLocator1);
    CALL_TASK(CreateServer1_With_Locator_XML);
    CALL_TASK(SetupClient1_Pool_Locator);
    CALL_TASK(populateServer);
    CALL_TASK(StopServer);
    CALL_TASK(VerifyDead);
    CALL_TASK(CreateServer1_With_Locator_XML);
    CALL_TASK(Verify);
    CALL_TASK(StopClient1);
    CALL_TASK(NotAutoReady_CallReadyForEvents);
    CALL_TASK(Verify);
    CALL_TASK(populateServer);
    CALL_TASK(StopServer);
    CALL_TASK(VerifyDead);
    CALL_TASK(CreateServer1_With_Locator_XML);
    CALL_TASK(Verify);
    CALL_TASK(StopClient1);
    CALL_TASK(NotAutoReady_DontCallReadyForEvents);
    CALL_TASK(VerifyNotLive);
    CALL_TASK(populateServer);
    CALL_TASK(StopServer);
    CALL_TASK(VerifyNotDead);
    CALL_TASK(CreateServer1_With_Locator_XML);
    CALL_TASK(VerifyNotLive);
    CALL_TASK(StopClient1);
    CALL_TASK(StopServer);
    CALL_TASK(CloseLocator1);
  }
END_MAIN
