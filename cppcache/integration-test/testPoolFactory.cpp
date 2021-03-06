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
#define ROOT_NAME "testPoolFactory"

#include "fw_helper.hpp"
#include <CacheRegionHelper.hpp>
#include <geode/PoolFactory.hpp>
#include <geode/CacheFactory.hpp>
#include <geode/Cache.hpp>
#include <geode/PoolManager.hpp>
#include "CacheImpl.hpp"
using namespace apache::geode::client;

BEGIN_TEST(POOLFACTORY)
{
  auto cacheFactory = CacheFactory::createCacheFactory();
  ASSERT(cacheFactory != nullptr, "CacheFactory was a nullptr");
  auto cachePtr = cacheFactory->create();
  ASSERT(cachePtr != nullptr, "CachePtr was a nullptr");
  auto poolFactory = cachePtr->getPoolManager().createFactory();
  ASSERT(poolFactory != nullptr, "poolFactory was a nullptr");
  auto& testPoolFactory = poolFactory->setSubscriptionEnabled(true);
  bool test = poolFactory.get() == std::addressof<PoolFactory>(testPoolFactory);
  ASSERT(test, "testPoolFactory and poolFactory not equal");
 }
END_TEST(POOLFACTORY)
