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
#include <geode/Execution.hpp>
#include "end_native.hpp"

#include "native_shared_ptr.hpp"

using namespace System;

namespace Apache
{
  namespace Geode
  {
    namespace Client
    {
      namespace native = apache::geode::client;

      generic<class TResult>
      interface class IResultCollector;

      generic<class TResult>
      ref class ResultCollector;

      /// <summary>
      /// This class encapsulates events that occur for cq.
      /// </summary>
      generic<class TResult>
      public ref class Execution sealed
      {
      public:
        /// <summary>
		    /// Add a routing object, 
        /// Return self.
        /// </summary>
		    generic<class TFilter>
        Execution<TResult>^ WithFilter(System::Collections::Generic::ICollection<TFilter>^ routingObj);

        /// <summary>
		    /// Add an argument, 
        /// Return self.
        /// </summary>
        generic<class TArgs>
		    Execution<TResult>^ WithArgs(TArgs args);

        /// <summary>
		    /// Add a result collector, 
        /// Return self.
        /// </summary>
		    Execution<TResult>^ WithCollector(IResultCollector<TResult>^ rc);

        /// <summary>
        /// Execute a function, 
        /// Return resultCollector.
        /// </summary>
		    /// <param name="timeout"> Value to wait for the operation to finish before timing out.</param> 
        IResultCollector<TResult>^ Execute(String^ func, TimeSpan timeout);

        /// <summary>
        /// Execute a function, 
        /// Return resultCollector.
        /// </summary>
        IResultCollector<TResult>^ Execute(String^ func);

      internal:

        /// <summary>
        /// Internal factory function to wrap a native object pointer inside
        /// this managed class with null pointer check.
        /// </summary>
        /// <param name="nativeptr">The native object pointer</param>
        /// <returns>
        /// The managed wrapper object; null if the native pointer is null.
        /// </returns>
        inline static Execution<TResult>^ Create( std::shared_ptr<native::Execution> nativeptr, IResultCollector<TResult>^ rc )
        {
          return __nullptr == nativeptr ? nullptr :
            gcnew Execution<TResult>( nativeptr, rc );
	      }

        /// <summary>
        /// Private constructor to wrap a native object pointer.
        /// </summary>
        /// <param name="nativeptr">The native object pointer</param>
        inline Execution( std::shared_ptr<native::Execution> nativeptr, IResultCollector<TResult>^ rc )
        {
          m_rc = rc;
          m_nativeptr = gcnew native_shared_ptr<native::Execution>(nativeptr);
        }
      private:
        IResultCollector<TResult>^ m_rc;

        native_shared_ptr<native::Execution>^ m_nativeptr;
      };
    }  // namespace Client
  }  // namespace Geode
}  // namespace Apache

