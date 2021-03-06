#pragma once

#ifndef GEODE_CACHEXMLCREATION_H_
#define GEODE_CACHEXMLCREATION_H_

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

#include <geode/geode_globals.hpp>
#include <geode/ExceptionTypes.hpp>
#include "RegionXmlCreation.hpp"
#include "PoolXmlCreation.hpp"
#include <vector>

namespace apache {
namespace geode {
namespace client {

class Cache;
/**
 * Represents a {@link Cache} that is created declaratively.
 *
 * @since 1.0
 */

class CPPCACHE_EXPORT CacheXmlCreation {
 public:
  /**
   * Creates a new <code>CacheXmlCreation</code> with no root region
   */
  CacheXmlCreation();

  /**
   * Adds a root region to the cache
   */
  void addRootRegion(std::shared_ptr<RegionXmlCreation> root);

  /** Adds a pool to the cache */
  void addPool(std::shared_ptr<PoolXmlCreation> pool);

  /**
   * Fills in the contents of a {@link Cache} based on this creation
   * object's state.
   *
   * @param  cache
   *         The cache which is to be populated
   * @throws OutOfMemoryException if the memory allocation failed
   * @throws NotConnectedException if the cache is not connected
   * @throws InvalidArgumentException if the attributePtr is nullptr.
   *         or if RegionAttributes is null or if regionName is null,
   *         the empty   string, or contains a '/'
   * @throws RegionExistsException
   * @throws CacheClosedException if the cache is closed
   *         when the region is created.
   * @throws UnknownException otherwise
   *
   */
  void create(Cache* cache);

  void setPdxIgnoreUnreadField(bool ignore);

  void setPdxReadSerialized(bool val);

  bool getPdxIgnoreUnreadField() { return m_pdxIgnoreUnreadFields; }

  bool getPdxReadSerialized(bool val) { return m_readPdxSerialized; }

  ~CacheXmlCreation() = default;

 private:
  /** This cache's roots */
  std::vector<std::shared_ptr<RegionXmlCreation>> rootRegions;

  /** This cache's pools */
  std::vector<std::shared_ptr<PoolXmlCreation>> pools;

  Cache* m_cache;
  bool m_pdxIgnoreUnreadFields;
  bool m_readPdxSerialized;
};
}  // namespace client
}  // namespace geode
}  // namespace apache

#endif  // GEODE_CACHEXMLCREATION_H_
