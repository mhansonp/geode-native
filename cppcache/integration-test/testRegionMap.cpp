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

#include <iostream>

#include "fw_helper.hpp"

using namespace apache::geode::client;

#define ROOT_NAME "testRegionMap"

#include "CacheHelper.hpp"

/**
 * @brief Test putting and getting entries without LRU enabled.
 */
BEGIN_TEST(TestRegionLRULastTen)
#if 1
  CacheHelper& cacheHelper = CacheHelper::getHelper();
  std::shared_ptr<Region> regionPtr;
  cacheHelper.createLRURegion(fwtest_Name, regionPtr);
  std::cout << regionPtr->getFullPath() << std::endl;
  // put more than 10 items... verify limit is held.
  uint32_t i;
  for (i = 0; i < 10; i++) {
    char buf[100];
    sprintf(buf, "%d", i);
    auto key = CacheableKey::create(buf);
    sprintf(buf, "value of %d", i);
    auto valuePtr = cacheHelper.createCacheable(buf);
    regionPtr->put(key, valuePtr);
    std::vector<std::shared_ptr<CacheableKey>> vecKeys = regionPtr->keys();
    ASSERT(vecKeys.size() == (i + 1), "expected more entries");
  }
  for (i = 10; i < 20; i++) {
    char buf[100];
    sprintf(buf, "%d", i);
    auto key = CacheableKey::create(buf);
    sprintf(buf, "value of %d", i);
    auto valuePtr = cacheHelper.createCacheable(buf);
    regionPtr->put(key, valuePtr);
    std::vector<std::shared_ptr<CacheableKey>> vecKeys = regionPtr->keys();
    cacheHelper.showKeys(vecKeys);
    ASSERT(vecKeys.size() == (10), "expected 10 entries");
  }
  std::vector<std::shared_ptr<CacheableKey>> vecKeys = regionPtr->keys();
  ASSERT(vecKeys.size() == 10, "expected 10 entries");
  // verify it is the last 10 keys..
  int expected = 0;
  int total = 0;
  for (int k = 10; k < 20; k++) {
    expected += k;
    auto key = std::dynamic_pointer_cast<CacheableString>(vecKeys.back());
    vecKeys.pop_back();
    total += atoi(key->asChar());
  }
  ASSERT(vecKeys.empty(), "expected no more than 10 keys.");
  ASSERT(expected == total, "checksum mismatch.");
#endif
END_TEST(TestRegionLRULastTen)

BEGIN_TEST(TestRegionNoLRU)
#if 1
  CacheHelper& cacheHelper = CacheHelper::getHelper();
  std::shared_ptr<Region> regionPtr;
  cacheHelper.createPlainRegion(fwtest_Name, regionPtr);
  // put more than 10 items... verify limit is held.
  uint32_t i;
  for (i = 0; i < 20; i++) {
    char buf[100];
    sprintf(buf, "%d", i);
    auto key = CacheableKey::create(buf);
    sprintf(buf, "value of %d", i);
    auto valuePtr = cacheHelper.createCacheable(buf);
    regionPtr->put(key, valuePtr);
    std::vector<std::shared_ptr<CacheableKey>> vecKeys = regionPtr->keys();
    cacheHelper.showKeys(vecKeys);
    ASSERT(vecKeys.size() == (i + 1), "unexpected entries count");
  }

#endif

END_TEST(TestRegionNoLRU)

BEGIN_TEST(TestRegionLRULocal)
#if 1
  CacheHelper& cacheHelper = CacheHelper::getHelper();
  std::shared_ptr<Region> regionPtr;
  cacheHelper.createLRURegion(fwtest_Name, regionPtr);
  std::cout << regionPtr->getFullPath() << std::endl;
  // put more than 10 items... verify limit is held.
  uint32_t i;
  /** @TODO make this local scope and re-increase the iterations... would also
   * like to time it. */
  for (i = 0; i < 1000; i++) {
    char buf[100];
    sprintf(buf, "%d", i);
    auto key = CacheableKey::create(buf);
    sprintf(buf, "value of %d", i);
    auto valuePtr = cacheHelper.createCacheable(buf);
    regionPtr->put(key, valuePtr);
    std::vector<std::shared_ptr<CacheableKey>> vecKeys = regionPtr->keys();
    ASSERT(vecKeys.size() == (i < 10 ? i + 1 : 10), "expected more entries");
  }

#endif
END_TEST(TestRegionLRULocal)

BEGIN_TEST(TestRecentlyUsedBit)
  // Put twenty items in region. LRU is set to 10.
  // So 10 through 19 should be in region (started at 0)
  // get 15 so it is marked recently used.
  // put 9 more...  check that 15 was skipped for eviction.
  // put 1 more...  15 should then have been evicted.
  CacheHelper& cacheHelper = CacheHelper::getHelper();
  std::shared_ptr<Region> regionPtr;
  cacheHelper.createLRURegion(fwtest_Name, regionPtr);
  std::cout << regionPtr->getFullPath() << std::endl;
  // put more than 10 items... verify limit is held.
  uint32_t i;
  char buf[100];
  for (i = 0; i < 20; i++) {
    sprintf(buf, "%d", i);
    auto key = CacheableKey::create(buf);
    sprintf(buf, "value of %d", i);
    auto valuePtr = cacheHelper.createCacheable(buf);
    regionPtr->put(key, valuePtr);
  }
  sprintf(buf, "%d", 15);
  std::shared_ptr<CacheableString> value2Ptr;
  auto key2 = CacheableKey::create(buf);
  value2Ptr = std::dynamic_pointer_cast<CacheableString>(regionPtr->get(key2));
  ASSERT(value2Ptr != nullptr, "expected to find key 15 in cache.");
  for (i = 20; i < 35; i++) {
    sprintf(buf, "%d", i);
    auto key = CacheableKey::create(buf);
    auto valuePtr = cacheHelper.createCacheable(buf);
    regionPtr->put(key, valuePtr);
    std::vector<std::shared_ptr<CacheableKey>> vecKeys = regionPtr->keys();
    cacheHelper.showKeys(vecKeys);
    ASSERT(vecKeys.size() == 10, "expected more entries");
  }
  ASSERT(regionPtr->containsKey(key2), "expected to find key 15 in cache.");
  {
    sprintf(buf, "%d", 35);
    auto key = CacheableKey::create(buf);
    auto valuePtr = cacheHelper.createCacheable(buf);
    regionPtr->put(key, valuePtr);
    std::vector<std::shared_ptr<CacheableKey>> vecKeys = regionPtr->keys();
    cacheHelper.showKeys(vecKeys);
    ASSERT(vecKeys.size() == 10, "expected more entries");
  }
  ASSERT(regionPtr->containsKey(key2) == false, "15 should have been evicted.");

END_TEST(TestRecentlyUsedBit)

BEGIN_TEST(TestEmptiedMap)
  CacheHelper& cacheHelper = CacheHelper::getHelper();
  std::shared_ptr<Region> regionPtr;
  cacheHelper.createLRURegion(fwtest_Name, regionPtr);
  std::cout << regionPtr->getFullPath() << std::endl;
  // put more than 10 items... verify limit is held.
  uint32_t i;
  for (i = 0; i < 10; i++) {
    char buf[100];
    sprintf(buf, "%d", i);
    auto key = CacheableKey::create(buf);
    sprintf(buf, "value of %d", i);
    auto valuePtr = cacheHelper.createCacheable(buf);
    regionPtr->put(key, valuePtr);
    std::vector<std::shared_ptr<CacheableKey>> vecKeys = regionPtr->keys();
    ASSERT(vecKeys.size() == (i + 1), "expected more entries");
  }
  for (i = 0; i < 10; i++) {
    char buf[100];
    sprintf(buf, "%d", i);
    auto key = CacheableKey::create(buf);
    regionPtr->destroy(key);
    std::cout << "removed key "
              << std::dynamic_pointer_cast<CacheableString>(key)->asChar()
              << std::endl;
  }
  std::vector<std::shared_ptr<CacheableKey>> vecKeys = regionPtr->keys();
  ASSERT(vecKeys.size() == 0, "expected more entries");
  for (i = 20; i < 40; i++) {
    char buf[100];
    sprintf(buf, "%d", i);
    auto key = CacheableKey::create(buf);
    sprintf(buf, "value of %d", i);
    auto valuePtr = cacheHelper.createCacheable(buf);
    regionPtr->put(key, valuePtr);
  }
  vecKeys = regionPtr->keys();
  ASSERT(vecKeys.size() == 10, "expected more entries");

END_TEST(TestEmptiedMap)
