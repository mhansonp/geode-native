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

#include "geode_defs.hpp"
#include "begin_native.hpp"
#include <geode/RegionAttributes.hpp>
#include "end_native.hpp"

#include "native_shared_ptr.hpp"
#include "IGeodeSerializable.hpp"
#include "ExpirationAction.hpp"
#include "DiskPolicyType.hpp"
#include "GeodeClassIds.hpp"

#include "ICacheLoader.hpp"
#include "ICacheWriter.hpp"
#include "ICacheListener.hpp"
#include "IPartitionResolver.hpp"
#include "Properties.hpp"

using namespace System;

namespace Apache
{
  namespace Geode
  {
    namespace Client
    {
      namespace native = apache::geode::client;

      /// <summary>
      /// Defines attributes for configuring a region.
      /// </summary>
      /// <remarks>
      /// These are
      /// <c>ICacheListener</c>, <c>ICacheLoader</c>, <c>ICacheWriter</c>,
      /// scope, mirroring, and expiration attributes
      /// for the region itself; expiration attributes for the region entries;
      /// and whether statistics are enabled for the region and its entries.
      ///
      /// To create an instance of this interface, use
      /// <see cref="AttributesFactory.CreateRegionAttributes" />.
      ///
      /// For compatibility rules and default values, see <see cref="AttributesFactory" />.
      /// <para>
      /// Note that the <c>RegionAttributes</c> are not distributed with the region.
      /// </para>
      /// </remarks>
      /// <seealso cref="AttributesFactory" />
      /// <seealso cref="AttributesMutator" />
      /// <seealso cref="Region.Attributes" />
      generic <class TKey, class TValue>
      public ref class RegionAttributes sealed
        : public IGeodeSerializable
      {
      public:

        /// <summary>
        /// Gets the cache loader for the region.
        /// </summary>
        /// <returns>
        /// region's <c>ICacheLoader</c> or null if none
        /// </returns>
        property Apache::Geode::Client::ICacheLoader<TKey, TValue>^ CacheLoader
        {
          Apache::Geode::Client::ICacheLoader<TKey, TValue>^ get();
        }

        /// <summary>
        /// Gets the cache writer for the region.
        /// </summary>
        /// <returns>
        /// region's <c>ICacheWriter</c> or null if none
        /// </returns>
        property ICacheWriter<TKey, TValue>^ CacheWriter
        {
          ICacheWriter<TKey, TValue>^ get();
        }

        /// <summary>
        /// Gets the cache listener for the region.
        /// </summary>
        /// <returns>
        /// region's <c>ICacheListener</c> or null if none
        /// </returns>
        property ICacheListener<TKey, TValue>^ CacheListener
        {
          ICacheListener<TKey, TValue>^ get();
        }

        /// <summary>
        /// Gets the partition resolver for the region.
        /// </summary>
        /// <returns>
        /// region's <c>IPartitionResolver</c> or null if none
        /// </returns>
        property IPartitionResolver<TKey, TValue>^ PartitionResolver
        {
          IPartitionResolver<TKey, TValue>^ get();
        }

        /// <summary>
        /// Gets the <c>timeToLive</c> value for the region as a whole.
        /// </summary>
        /// <returns>the timeToLive duration for this region</returns>
        property TimeSpan RegionTimeToLive
        {
          TimeSpan get();
        }

        /// <summary>
        /// Gets the <c>timeToLive</c> expiration action for the region as a whole.
        /// </summary>
        /// <returns>the timeToLive action for this region</returns>
        property ExpirationAction RegionTimeToLiveAction
        {
          ExpirationAction get();
        }

        /// <summary>
        /// Gets the <c>idleTimeout</c> value for the region as a whole.
        /// </summary>
        /// <returns>the IdleTimeout duration for this region</returns>
        property TimeSpan RegionIdleTimeout
        {
          TimeSpan get();
        }

        /// <summary>
        /// Gets the <c>idleTimeout</c> expiration action for the region as a whole.
        /// </summary>
        /// <returns>the idleTimeout action for this region</returns>
        property ExpirationAction RegionIdleTimeoutAction
        {
          ExpirationAction get();
        }

        /// <summary>
        /// Gets the <c>timeToLive</c> value for entries in this region.
        /// </summary>
        /// <returns>the timeToLive duration for entries in this region</returns>
        property TimeSpan EntryTimeToLive
        {
          TimeSpan get();
        }

        /// <summary>
        /// Gets the <c>timeToLive</c> expiration action for entries in this region.
        /// </summary>
        /// <returns>the timeToLive action for entries in this region</returns>
        property ExpirationAction EntryTimeToLiveAction
        {
          ExpirationAction get();
        }

        /// <summary>
        /// Gets the <c>idleTimeout</c> value for entries in this region.
        /// </summary>
        /// <returns>the idleTimeout duration for entries in this region</returns>
        property TimeSpan EntryIdleTimeout
        {
          TimeSpan get();
        }

        /// <summary>
        /// Gets the <c>idleTimeout</c> expiration action for entries in this region.
        /// </summary>
        /// <returns>the idleTimeout action for entries in this region</returns>
        property ExpirationAction EntryIdleTimeoutAction
        {
          ExpirationAction get();
        }

        /// <summary>
        /// If true, this region will store data in the current process.
        /// </summary>
        /// <returns>true if caching is enabled</returns>
        property bool CachingEnabled
        {
          bool get();
        }


        // MAP ATTRIBUTES

        /// <summary>
        /// Returns the initial capacity of the entry's local cache.
        /// </summary>
        /// <returns>the initial capacity</returns>
        property System::Int32 InitialCapacity
        {
          System::Int32 get();
        }

        /// <summary>
        /// Returns the load factor of the entry's local cache.
        /// </summary>
        /// <returns>the load factor</returns>
        property Single LoadFactor
        {
          Single get();
        }

        /// <summary>
        /// Returns the concurrency level of the entry's local cache.
        /// </summary>
        /// <returns>the concurrency level</returns>
        /// <seealso cref="AttributesFactory" />
        property System::Int32 ConcurrencyLevel
        {
          System::Int32 get();
        }

        /// <summary>
        /// Returns the maximum number of entries this cache will hold before
        /// using LRU eviction. 
        /// </summary>
        /// <returns>the maximum LRU size, or 0 for no limit</returns>
        property System::UInt32 LruEntriesLimit
        {
          System::UInt32 get();
        }

        /// <summary>
        /// Returns the disk policy type of the region.
        /// </summary>
        /// <returns>the disk policy type, default is null</returns>
        property DiskPolicyType DiskPolicy
        {
          DiskPolicyType get();
        }

        /// <summary>
        /// Returns the ExpirationAction used for LRU Eviction, default is LOCAL_DESTROY.
        /// </summary>
        /// <returns>the LRU eviction action</returns>
        property ExpirationAction LruEvictionAction
        {
          ExpirationAction get();
        }

        /// <summary>
        /// Returns the path of the library from which
        /// the factory function will be invoked on a cache server.
        /// </summary>
        /// <returns>the CacheLoader library path</returns>
        property String^ CacheLoaderLibrary
        {
          String^ get();
        }

        /// <summary>
        /// Rreturns the symbol name of the factory function from which
        /// the loader will be created on a cache server.
        /// </summary>
        /// <returns>the CacheLoader factory function name</returns>
        property String^ CacheLoaderFactory
        {
          String^ get();
        }

        /// <summary>
        /// Returns the path of the library from which
        /// the factory function will be invoked on a cache server.
        /// </summary>
        /// <returns>the CacheListener library path</returns>
        property String^ CacheListenerLibrary
        {
          String^ get();
        }

        /// <summary>
        /// Returns the path of the library from which
        /// the factory function will be invoked on a cache server.
        /// </summary>
        /// <returns>the PartitionResolver library path</returns>
        property String^ PartitionResolverLibrary
        {
          String^ get();
        }

        /// <summary>
        /// Returns the symbol name of the factory function from which
        /// the loader will be created on a cache server.
        /// </summary>
        /// <returns>the CacheListener factory function name</returns>
        property String^ CacheListenerFactory
        {
          String^ get();
        }

        /// <summary>
        /// Returns the symbol name of the factory function from which
        /// the loader will be created on a cache server.
        /// </summary>
        /// <returns>the PartitionResolver factory function name</returns>
        property String^ PartitionResolverFactory
        {
          String^ get();
        }

        /// <summary>
        /// Returns the path of the library from which
        /// the factory function will be invoked on a cache server.
        /// </summary>
        /// <returns>the CacheWriter library path</returns>
        property String^ CacheWriterLibrary
        {
          String^ get();
        }

        /// <summary>
        /// Returns the symbol name of the factory function from which
        /// the loader will be created on a cache server.
        /// </summary>
        /// <returns>the CacheWriter factory function name</returns>
        property String^ CacheWriterFactory
        {
          String^ get();
        }

        /// <summary>
        /// True if all the attributes are equal to those of <c>other</c>.
        /// </summary>
        /// <param name="other">attribute object to compare</param>
        /// <returns>true if equal</returns>
        bool Equals(RegionAttributes<TKey, TValue>^ other);

        /// <summary>
        /// True if all the attributes are equal to those of <c>other</c>.
        /// </summary>
        /// <param name="other">attribute object to compare</param>
        /// <returns>true if equal</returns>
        virtual bool Equals(Object^ other) override;

        /// <summary>
        /// Throws IllegalStateException if the attributes are not suited for serialization
        /// such as those that have a cache callback (listener, loader, or writer) set
        /// directly instead of providing the library path and factory function.
        /// </summary>
        /// <exception cref="IllegalStateException">if the attributes cannot be serialized</exception>
        void ValidateSerializableAttributes();

        /// <summary>
        /// This method returns the list of endpoints (servername:portnumber) separated by commas.
        /// </summary>
        /// <returns>list of endpoints</returns>
        property String^ Endpoints
        {
          String^ get();
        }

        /// <summary>
        /// This method returns the name of the attached pool.
        /// </summary>
        /// <returns>pool name</returns>
        property String^ PoolName
        {
          String^ get();
        }

        /// <summary>
        /// True if client notification is enabled.
        /// </summary>
        /// <returns>true if enabled</returns>
        property bool ClientNotificationEnabled
        {
          bool get();
        }
        /// <summary>
        /// True if cloning is enabled for in case of delta.
        /// </summary>
        /// <returns>true if enabled</returns>

        property bool CloningEnabled
        {
          bool get();
        }

        /// <summary>
        /// Returns the path of the library from which
        /// the factory function will be invoked on a cache server.
        /// </summary>
        /// <returns>the PersistenceManager library path</returns>
        property String^ PersistenceLibrary
        {
          String^ get();
        }

        /// <summary>
        /// Returns the symbol name of the factory function from which
        /// the persistence manager will be created on a cache server.
        /// </summary>
        /// <returns>the PersistenceManager factory function name</returns>
        property String^ PersistenceFactory
        {
          String^ get();
        }

        /// <summary>
        /// Returns the properties set for persistence.
        /// </summary>
        /// <returns>properties for the PersistenceManager</returns>
        property Properties<String^, String^>^ PersistenceProperties
        {
          Properties<String^, String^>^ get();
        }

        /// <summary>
        /// Returns the concurrency check enabled flag of the region
        /// </summary>
        /// <returns>the concurrency check enabled flag</returns>
        /// <seealso cref="AttributesFactory" />
        property bool ConcurrencyChecksEnabled
        {
          bool get();
        }

        /// <summary>
        /// Serializes this Properties object.
        /// </summary>
        /// <param name="output">the DataOutput stream to use for serialization</param>
        virtual void ToData(DataOutput^ output);

        /// <summary>
        /// Deserializes this Properties object.
        /// </summary>
        /// <param name="input">the DataInput stream to use for reading data</param>
        /// <returns>the deserialized Properties object</returns>
        virtual void FromData(DataInput^ input);

        /// <summary>
        /// return the size of this object in bytes
        /// </summary>
        virtual property System::UInt32 ObjectSize
        {
          virtual System::UInt32 get()
          {
            return 0;  //don't care
          }
        }

        /// <summary>
        /// Returns the classId of this class for serialization.
        /// </summary>
        /// <returns>classId of the Properties class</returns>
        /// <seealso cref="../../IGeodeSerializable.ClassId" />
        virtual property System::UInt32 ClassId
        {
          inline virtual System::UInt32 get()
          {
            return GeodeClassIds::RegionAttributes;
          }
        }


      internal:

        /// <summary>
        /// Internal factory function to wrap a native object pointer inside
        /// this managed class with null pointer check.
        /// </summary>
        /// <param name="nativeptr">The native object pointer</param>
        /// <returns>
        /// The managed wrapper object; null if the native pointer is null.
        /// </returns>
        inline static RegionAttributes<TKey, TValue>^ Create(std::shared_ptr<native::RegionAttributes> nativeptr)
        {
          return __nullptr == nativeptr ? nullptr :
            gcnew RegionAttributes<TKey, TValue>( nativeptr );
        }

        std::shared_ptr<native::RegionAttributes> GetNative()
        {
          return m_nativeptr->get_shared_ptr();
        }

      private:

        /// <summary>
        /// Private constructor to wrap a native object pointer
        /// </summary>
        /// <param name="nativeptr">The native object pointer</param>
        inline RegionAttributes<TKey, TValue>(std::shared_ptr<native::RegionAttributes> nativeptr)
        {
          m_nativeptr = gcnew native_shared_ptr<native::RegionAttributes>(nativeptr);
        }

        native_shared_ptr<native::RegionAttributes>^ m_nativeptr;
      };
    }  // namespace Client
  }  // namespace Geode
}  // namespace Apache


