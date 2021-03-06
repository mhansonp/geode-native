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

#include "geode/TypeRegistry.hpp"
#include "CacheRegionHelper.hpp"
#include "CacheImpl.hpp"

/**
  TypeRegistry is the public facing wrapper for the serialization registry.
**/

TypeRegistry::TypeRegistry(const Cache &cache) : m_cache(cache) {}

void TypeRegistry::registerType(TypeFactoryMethod creationFunction) {
  CacheRegionHelper::getCacheImpl(&m_cache)
      ->getSerializationRegistry()
      ->addType(creationFunction);
}

void TypeRegistry::registerPdxType(TypeFactoryMethodPdx creationFunction) {
  CacheRegionHelper::getCacheImpl(&m_cache)
      ->getSerializationRegistry()
      ->addPdxType(creationFunction);
}

void TypeRegistry::registerPdxSerializer(
    std::shared_ptr<PdxSerializer> pdxSerializer) {
  CacheRegionHelper::getCacheImpl(&m_cache)
      ->getSerializationRegistry()
      ->setPdxSerializer(pdxSerializer);
}
