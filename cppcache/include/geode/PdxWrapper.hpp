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

#pragma once

#ifndef GEODE_PDXWRAPPER_H_
#define GEODE_PDXWRAPPER_H_

#include "PdxSerializer.hpp"
#include "PdxSerializable.hpp"

namespace apache {
namespace geode {
namespace client {

class CPPCACHE_EXPORT PdxWrapper : public PdxSerializable {
  /**
   * The PdxWrapper class allows domain classes to be used in Region operations.
   * A user domain object should be wrapped in an instance of a PdxWrapper with
   * a PdxSerializer registered that can handle the user domain class.
   */

 public:
  /**
   * Constructor which takes the address of the user object to contain for PDX
   * serialization.
   * @param userObject the void pointer to an instance of a user object - NOTE:
   * PdxWrapper takes ownership.
   * @param className the fully qualified class name to map this user object to
   * the Java side.
   */
  PdxWrapper(void* userObject, const char* className,
             std::shared_ptr<PdxSerializer> pdxSerializerPtr);

  /**
   * Returns the pointer to the user object which is deserialized with a
   * PdxSerializer.
   * User code (such as in PdxSerializer) should cast it to a pointer of the
   * known user class.
   * @param detach if set to true will release ownership of the object and
   * future calls to getObject() return nullptr.
   */
  void* getObject(bool detach = false);

  /**
   * Get the class name for the user domain object.
   */
  const char* getClassName() const;

  /** return true if this key matches other. */
  bool operator==(const CacheableKey& other) const;

  /** return the hashcode for this key. */
  int32_t hashcode() const;

  /**
   *@brief serialize this object in geode PDX format
   *@param PdxWriter to serialize the PDX object
   **/
  void toData(std::shared_ptr<PdxWriter> output);
  /**
   *@brief Deserialize this object
   *@param PdxReader to Deserialize the PDX object
   **/
  void fromData(std::shared_ptr<PdxReader> input);
  /**
   *@brief serialize this object
   **/
  void toData(DataOutput& output) const;
  /**
   *@brief deserialize this object, typical implementation should return
   * the 'this' pointer.
   **/
  void fromData(DataInput& input);
  /**
   *@brief return the classId of the instance being serialized.
   * This is used by deserialization to determine what instance
   * type to create and derserialize into.
   */
  int32_t classId() const { return 0; }
  /**
   *@brief return the size in bytes of the instance being serialized.
   * This is used to determine whether the cache is using up more
   * physical memory than it has been configured to use. The method can
   * return zero if the user does not require the ability to control
   * cache memory utilization.
   * Note that you must implement this only if you use the HeapLRU feature.
   */
  uint32_t objectSize() const;
  /**
   * Display this object as 'string', which depends on the implementation in
   * the subclasses.
   * The default implementation renders the classname.
   */
  std::shared_ptr<CacheableString> toString() const;

  virtual ~PdxWrapper();

 private:
  /** hide default constructor */
  PdxWrapper();
  PdxWrapper(const char* className,
             std::shared_ptr<PdxSerializer> pdxSerializerPtr);

  FRIEND_STD_SHARED_PTR(PdxWrapper)

  void* m_userObject;
  std::shared_ptr<PdxSerializer> m_serializer;
  UserDeallocator m_deallocator;
  UserObjectSizer m_sizer;
  char* m_className;

  // friend class SerializationRegistry;

  PdxWrapper(const PdxWrapper&);

  const PdxWrapper& operator=(const PdxWrapper&);
};
}  // namespace client
}  // namespace geode
}  // namespace apache

#endif  // GEODE_PDXWRAPPER_H_
