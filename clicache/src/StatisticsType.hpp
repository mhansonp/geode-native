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
#include <geode/statistics/StatisticsType.hpp>
#include <geode/statistics/StatisticDescriptor.hpp>
#include "end_native.hpp"

using namespace System;

namespace Apache
{
  namespace Geode
  {
    namespace Client
    {
      ref class StatisticDescriptor;

      /// <summary>
      /// This class is used to describe a logical collection of StatisticDescriptors.These descriptions
      /// are used to create an instance of <see cref="Statistics" /> class.
      /// </summary>
      /// <para>
      /// To get an instance of this interface use an instance of
      /// <see cref="StatisticsFactory" /> class.
      /// </para>
      public ref class StatisticsType sealed
      {
      public:
        /// <summary>
        /// Returns the name of this statistics type.
        /// </summary>
        virtual property String^ Name
        {
          virtual String^ get( );
        }

        /// <summary>
        /// Returns a description of this statistics type.
        /// </summary>
        virtual property String^ Description
        {
          virtual String^ get( );
        }

        /// <summary>
        /// Returns descriptions of the statistics that this statistics type
        /// gathers together.
        /// </summary>
        virtual property array<StatisticDescriptor^>^ Statistics
        {
          virtual array<StatisticDescriptor^>^ get( );
        }

        /// <summary>
        /// Returns the id of the statistic with the given name in this
        /// statistics instance.
        /// </summary>
        /// <param name="name">the statistic name</param>
        /// <returns>the id of the statistic with the given name</returns>
        /// <exception cref="IllegalArgumentException">
        /// if no statistic named <c>name</c> exists in this
        /// statistic instance.
        /// </exception>
        virtual System::Int32 NameToId(String^ name);

        /// <summary>
        /// Returns the descriptor of the statistic with the given name in this
        /// statistics instance.
        /// </summary>
        /// <param name="name">the statistic name</param>
        /// <returns>the descriptor of the statistic with the given name</returns>
        /// <exception cref="IllegalArgumentException">
        /// if no statistic named <c>name</c> exists in this
        /// statistic instance.
        /// </exception>
        virtual StatisticDescriptor^ NameToDescriptor(String^ name);

        /// <summary>
        /// Returns the total number of statistics descriptors in the type.
        /// </summary>
        virtual property System::Int32 DescriptorsCount
        {
          virtual System::Int32 get( );
        }

      internal:
        /// <summary>
        /// Internal factory function to wrap a native object pointer inside
        /// this managed class, with null pointer check.
        /// </summary>
        /// <param name="nativeptr">native object pointer</param>
        /// <returns>
        /// the managed wrapper object, or null if the native pointer is null.
        /// </returns>
        inline static StatisticsType^ Create(
          apache::geode::statistics::StatisticsType* nativeptr )
        {
          return __nullptr == nativeptr ? nullptr :
            gcnew StatisticsType( nativeptr );
        }

        apache::geode::statistics::StatisticsType* GetNative()
        {
          return m_nativeptr;
        }

      private:
        /// <summary>
        /// Private constructor to wrap a native object pointer
        /// </summary>
        /// <param name="nativeptr">The native object pointer</param>
        inline StatisticsType( apache::geode::statistics::StatisticsType* nativeptr )
          : m_nativeptr( nativeptr )
        {
        }

        apache::geode::statistics::StatisticsType* m_nativeptr;

      };
    }  // namespace Client
  }  // namespace Geode
}  // namespace Apache


