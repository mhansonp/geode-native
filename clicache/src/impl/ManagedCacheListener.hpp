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

#include "../geode_defs.hpp"
#include <vcclr.h>
#include "begin_native.hpp"
#include <geode/CacheListener.hpp>
#include "end_native.hpp"

#include "../ICacheListener.hpp"

namespace apache {
  namespace geode {
    namespace client {

      /// <summary>
      /// Wraps the managed <see cref="Apache.Geode.Client.ICacheListener" />
      /// object and implements the native <c>apache::geode::client::CacheListener</c> interface.
      /// </summary>
      class ManagedCacheListenerGeneric
        : public apache::geode::client::CacheListener
      {
      public:

        /// <summary>
        /// Constructor to initialize with the provided managed object.
        /// </summary>
        /// <param name="userptr">
        /// The managed object.
        /// </param>
        inline ManagedCacheListenerGeneric(
          /*Apache::Geode::Client::ICacheListener^ managedptr,*/ Object^ userptr)
          : /*m_managedptr( managedptr ),*/ m_userptr(userptr) { }

        /// <summary>
        /// Static function to create a <c>ManagedCacheListener</c> using given
        /// managed assembly path and given factory function.
        /// </summary>
        /// <param name="assemblyPath">
        /// The path of the managed assembly that contains the <c>ICacheListener</c>
        /// factory function.
        /// </param>
        /// <param name="factoryFunctionName">
        /// The name of the factory function of the managed class for creating
        /// an object that implements <c>ICacheListener</c>.
        /// This should be a static function of the format
        /// {Namespace}.{Class Name}.{Method Name}.
        /// </param>
        /// <exception cref="IllegalArgumentException">
        /// If the managed library cannot be loaded or the factory function fails.
        /// </exception>
        static apache::geode::client::CacheListener* create(const char* assemblyPath,
          const char* factoryFunctionName);

        /// <summary>
        /// Destructor -- does nothing.
        /// </summary>
        virtual ~ManagedCacheListenerGeneric() { }

        /// <summary>
        /// Handles the event of a new key being added to a region.
        /// </summary>
        /// <remarks>
        /// The entry did not previously exist in this region in the local cache
        /// (even with a null value).
        /// <para>
        /// This function does not throw any exception.
        /// </para>
        /// </remarks>
        /// <param name="ev">
        /// Denotes the event object associated with the entry creation.
        /// </param>
        /// <seealso cref="Apache.Geode.Client.Region.Create" />
        /// <seealso cref="Apache.Geode.Client.Region.Put" />
        /// <seealso cref="Apache.Geode.Client.Region.Get" />
        virtual void afterCreate(const apache::geode::client::EntryEvent& ev);

        /// <summary>
        /// Handles the event of an entry's value being modified in a region.
        /// </summary>
        /// <remarks>
        /// This entry previously existed in this region in the local cache,
        /// but its previous value may have been null.
        /// </remarks>
        /// <param name="ev">
        /// EntryEvent denotes the event object associated with updating the entry.
        /// </param>
        /// <seealso cref="Apache.Geode.Client.Region.Put" />
        virtual void afterUpdate(const apache::geode::client::EntryEvent& ev);

        /// <summary>
        /// Handles the event of an entry's value being invalidated.
        /// </summary>
        /// <param name="ev">
        /// EntryEvent denotes the event object associated with the entry invalidation.
        /// </param>
        virtual void afterInvalidate(const apache::geode::client::EntryEvent& ev);

        /// <summary>
        /// Handles the event of an entry being destroyed.
        /// </summary>
        /// <param name="ev">
        /// EntryEvent denotes the event object associated with the entry destruction.
        /// </param>
        /// <seealso cref="Apache.Geode.Client.Region.Destroy" />
        virtual void afterDestroy(const apache::geode::client::EntryEvent& ev);

        /// <summary>
        /// Handles the event of a region being cleared.
        /// </summary>
        virtual void afterRegionClear(const apache::geode::client::RegionEvent& ev);

        /// <summary>
        /// Handles the event of a region being invalidated.
        /// </summary>
        /// <remarks>
        /// Events are not invoked for each individual value that is invalidated
        /// as a result of the region being invalidated. Each subregion, however,
        /// gets its own <c>regionInvalidated</c> event invoked on its listener.
        /// </remarks>
        /// <param name="ev">
        /// RegionEvent denotes the event object associated with the region invalidation.
        /// </param>
        /// <seealso cref="Apache.Geode.Client.Region.InvalidateRegion" />
        virtual void afterRegionInvalidate(const apache::geode::client::RegionEvent& ev);

        /// <summary>
        /// Handles the event of a region being destroyed.
        /// </summary>
        /// <remarks>
        /// Events are not invoked for each individual entry that is destroyed
        /// as a result of the region being destroyed. Each subregion, however,
        /// gets its own <c>afterRegionDestroyed</c> event invoked on its listener.
        /// </remarks>
        /// <param name="ev">
        /// RegionEvent denotes the event object associated with the region destruction.
        /// </param>
        /// <seealso cref="Apache.Geode.Client.Region.DestroyRegion" />
        virtual void afterRegionDestroy(const apache::geode::client::RegionEvent& ev);

        /// <summary>
        /// Handles the event of a region being live.
        /// </summary>
        /// <remarks>
        /// Each subregion gets its own <c>afterRegionLive</c> event invoked on its listener.
        /// </remarks>
        /// <param name="ev">
        /// RegionEvent denotes the event object associated with the region going live.
        /// </param>
        /// <seealso cref="Apache.Geode.Client.Cache.ReadyForEvents" />
        virtual void afterRegionLive(const apache::geode::client::RegionEvent& ev);

        /// <summary>
        /// Called when the region containing this callback is destroyed, when
        /// the cache is closed.
        /// </summary>
        /// <remarks>
        /// Implementations should clean up any external resources,
        /// such as database connections. Any runtime exceptions this method
        /// throws will be logged.
        /// <para>
        /// It is possible for this method to be called multiple times on a single
        /// callback instance, so implementations must be tolerant of this.
        /// </para>
        /// </remarks>
        /// <seealso cref="Apache.Geode.Client.Cache.Close" />
        /// <seealso cref="Apache.Geode.Client.Region.DestroyRegion" />
        virtual void close(const std::shared_ptr<apache::geode::client::Region>& region);

        ///<summary>
        ///Called when all the endpoints associated with region are down.
        ///This will be called when all the endpoints are down for the first time.
        ///If endpoints come up and again go down it will be called again.
        ///This will also be called when all endpoints are down and region is attached to the pool.
        ///</summary>
        ///<remarks>
        ///</remark>
        ///<param>
        ///region Region^ denotes the assosiated region.
        ///</param>
        virtual void afterRegionDisconnected(const std::shared_ptr<apache::geode::client::Region>& region);

        /// <summary>
        /// Returns the wrapped managed object reference.
        /// </summary>
        inline Apache::Geode::Client::ICacheListener<Object^, Object^>^ ptr() const
        {
          return m_managedptr;
        }

        inline void setptr(Apache::Geode::Client::ICacheListener<Object^, Object^>^ managedptr)
        {
          m_managedptr = managedptr;
        }

        inline Object^ userptr() const
        {
          return m_userptr;
        }

      private:

        /// <summary>
        /// Using gcroot to hold the managed delegate pointer (since it cannot be stored directly).
        /// Note: not using auto_gcroot since it will result in 'Dispose' of the ICacheListener
        /// to be called which is not what is desired when this object is destroyed. Normally this
        /// managed object may be created by the user and will be handled automatically by the GC.
        /// </summary>
        gcroot<Apache::Geode::Client::ICacheListener<Object^, Object^>^> m_managedptr;

        gcroot<Object^> m_userptr;
      };

    }  // namespace client
  }  // namespace geode
}  // namespace apache
