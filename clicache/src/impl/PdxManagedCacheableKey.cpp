
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

#include "begin_native.hpp"
#include <GeodeTypeIdsImpl.hpp>
#include "CacheRegionHelper.hpp"
#include "end_native.hpp"

#include "PdxManagedCacheableKey.hpp"
#include "../DataInput.hpp"
#include "../DataOutput.hpp"
#include "../CacheableString.hpp"
#include "../ExceptionTypes.hpp"
#include "ManagedString.hpp"
#include "PdxHelper.hpp"
#include "SafeConvert.hpp"
#include "CacheResolver.hpp"

using namespace System;

namespace apache
{
  namespace geode
  {
    namespace client
    {
      void PdxManagedCacheableKey::toData(apache::geode::client::DataOutput& output) const
      {
        try {
          System::UInt32 pos = (int)output.getBufferLength();
          auto cache = CacheResolver::Lookup(output.getCache());
          Apache::Geode::Client::DataOutput mg_output(&output, true, cache);
          Apache::Geode::Client::Internal::PdxHelper::SerializePdx(%mg_output, m_managedptr);
          //m_managedptr->ToData( %mg_output );
          //this will move the cursor in c++ layer
          mg_output.WriteBytesToUMDataOutput();
          PdxManagedCacheableKey* tmp = const_cast<PdxManagedCacheableKey*>(this);
          tmp->m_objectSize = (int)(output.getBufferLength() - pos);
        }
        catch (Apache::Geode::Client::GeodeException^ ex) {
          ex->ThrowNative();
        }
        catch (System::Exception^ ex) {
          Apache::Geode::Client::GeodeException::ThrowNative(ex);
        }
      }

      void PdxManagedCacheableKey::fromData(apache::geode::client::DataInput& input)
      {
        try {
          int pos = input.getBytesRead();
          auto cache = CacheResolver::Lookup(input.getCache());
          Apache::Geode::Client::DataInput mg_input(&input, true, cache);
          //m_managedptr = m_managedptr->FromData( %mg_input );
          Apache::Geode::Client::IPdxSerializable^ tmp = Apache::Geode::Client::Internal::PdxHelper::DeserializePdx(%mg_input, false,  CacheRegionHelper::getCacheImpl(input.getCache())->getSerializationRegistry().get());
          m_managedptr = tmp;
          m_managedDeltaptr = dynamic_cast<Apache::Geode::Client::IGeodeDelta^>(tmp);

          //this will move the cursor in c++ layer
          input.advanceCursor(mg_input.BytesReadInternally);
          m_objectSize = input.getBytesRead() - pos;
          // m_hashcode = m_managedptr->GetHashCode();
        }
        catch (Apache::Geode::Client::GeodeException^ ex) {
          ex->ThrowNative();
        }
        catch (System::Exception^ ex) {
          Apache::Geode::Client::GeodeException::ThrowNative(ex);
        }
      }

      System::UInt32 PdxManagedCacheableKey::objectSize() const
      {
        try {
          return m_objectSize;
        }
        catch (Apache::Geode::Client::GeodeException^ ex) {
          ex->ThrowNative();
        }
        catch (System::Exception^ ex) {
          Apache::Geode::Client::GeodeException::ThrowNative(ex);
        }
        return 0;
      }

      System::Int32 PdxManagedCacheableKey::classId() const
      {
        /*System::UInt32 classId;
        try {
        classId = m_managedptr->ClassId;
        }
        catch (Com::Vmware::Cache::GeodeException^ ex) {
        ex->ThrowNative();
        }
        catch (System::Exception^ ex) {
        Apache::Geode::Client::GeodeException::ThrowNative(ex);
        }*/
        //return (m_classId >= 0x80000000 ? 0 : m_classId);
        return 0;
      }

      int8_t PdxManagedCacheableKey::typeId() const
      {
        /*if (m_classId >= 0x80000000) {
          return (int8_t)((m_classId - 0x80000000) % 0x20000000);
          }
          else if (m_classId <= 0x7F) {
          return (int8_t)GeodeTypeIdsImpl::CacheableUserData;
          }
          else if (m_classId <= 0x7FFF) {
          return (int8_t)GeodeTypeIdsImpl::CacheableUserData2;
          }
          else {
          return (int8_t)GeodeTypeIdsImpl::CacheableUserData4;
          }*/
        return (int8_t)GeodeTypeIdsImpl::PDX;
      }

      int8_t PdxManagedCacheableKey::DSFID() const
      {
        // convention that [0x8000000, 0xa0000000) is for FixedIDDefault,
        // [0xa000000, 0xc0000000) is for FixedIDByte,
        // [0xc0000000, 0xe0000000) is for FixedIDShort
        // and [0xe0000000, 0xffffffff] is for FixedIDInt
        // Note: depends on fact that FixedIDByte is 1, FixedIDShort is 2
        // and FixedIDInt is 3; if this changes then correct this accordingly
        //if (m_classId >= 0x80000000) {
        //  return (int8_t)((m_classId - 0x80000000) / 0x20000000);
        //}
        return 0;
      }

      std::shared_ptr<apache::geode::client::CacheableString> PdxManagedCacheableKey::toString() const
      {
        try {
          std::shared_ptr<apache::geode::client::CacheableString> cStr;
          Apache::Geode::Client::CacheableString::GetCacheableString(
            m_managedptr->ToString(), cStr);
          return cStr;
        }
        catch (Apache::Geode::Client::GeodeException^ ex) {
          ex->ThrowNative();
        }
        catch (System::Exception^ ex) {
          Apache::Geode::Client::GeodeException::ThrowNative(ex);
        }
        return nullptr;
      }

      bool PdxManagedCacheableKey::operator ==(const apache::geode::client::CacheableKey& other) const
      {
        try {
          // now checking classId(), typeId(), DSFID() etc. will be much more
          // expensive than just a dynamic_cast
          const PdxManagedCacheableKey* p_other =
            dynamic_cast<const PdxManagedCacheableKey*>(&other);
          if (p_other != NULL) {
            return ((Apache::Geode::Client::IPdxSerializable^)m_managedptr)->Equals((p_other->ptr()));
          }
          return false;
        }
        catch (Apache::Geode::Client::GeodeException^ ex) {
          ex->ThrowNative();
        }
        catch (System::Exception^ ex) {
          Apache::Geode::Client::GeodeException::ThrowNative(ex);
        }
        return false;
      }

      bool PdxManagedCacheableKey::operator ==(const PdxManagedCacheableKey& other) const
      {
        try {
          return ((Apache::Geode::Client::IPdxSerializable^)m_managedptr)->Equals((other.ptr()));
        }
        catch (Apache::Geode::Client::GeodeException^ ex) {
          ex->ThrowNative();
        }
        catch (System::Exception^ ex) {
          Apache::Geode::Client::GeodeException::ThrowNative(ex);
        }
        return false;
      }

      System::Int32 PdxManagedCacheableKey::hashcode() const
      {
        if (m_hashcode != 0)
          return m_hashcode;
        try {
          PdxManagedCacheableKey* tmp = const_cast<PdxManagedCacheableKey*>(this);
          tmp->m_hashcode = (
            (Apache::Geode::Client::IPdxSerializable^)tmp->m_managedptr)
            ->GetHashCode();
          return m_hashcode;
        }
        catch (Apache::Geode::Client::GeodeException^ ex) {
          ex->ThrowNative();
        }
        catch (System::Exception^ ex) {
          Apache::Geode::Client::GeodeException::ThrowNative(ex);
        }
        return 0;
      }

      size_t PdxManagedCacheableKey::logString(char* buffer, size_t maxLength) const
      {
        try {
          if (maxLength > 0) {
            String^ logstr = m_managedptr->GetType()->Name + '(' +
              m_managedptr->ToString() + ')';
            Apache::Geode::Client::ManagedString mg_str(logstr);
            return snprintf(buffer, maxLength, "%s", mg_str.CharPtr);
          }
        }
        catch (Apache::Geode::Client::GeodeException^ ex) {
          ex->ThrowNative();
        }
        catch (System::Exception^ ex) {
          Apache::Geode::Client::GeodeException::ThrowNative(ex);
        }
        return 0;
      }

      bool PdxManagedCacheableKey::hasDelta()
      {
        if (m_managedDeltaptr)
        {
          return m_managedDeltaptr->HasDelta();
        }
        return false;
      }

      void PdxManagedCacheableKey::toDelta(DataOutput& output) const
      {
        try {
          auto cache = CacheResolver::Lookup(output.getCache());
          Apache::Geode::Client::DataOutput mg_output(&output, true, cache);
          m_managedDeltaptr->ToDelta(%mg_output);
          //this will move the cursor in c++ layer
          mg_output.WriteBytesToUMDataOutput();
        }
        catch (Apache::Geode::Client::GeodeException^ ex) {
          ex->ThrowNative();
        }
        catch (System::Exception^ ex) {
          Apache::Geode::Client::GeodeException::ThrowNative(ex);
        }
      }

      void PdxManagedCacheableKey::fromDelta(native::DataInput& input)
      {
        try {
          auto cache = CacheResolver::Lookup(input.getCache());
          Apache::Geode::Client::DataInput mg_input(&input, true, cache);
          m_managedDeltaptr->FromDelta(%mg_input);

          //this will move the cursor in c++ layer
          input.advanceCursor(mg_input.BytesReadInternally);

          m_hashcode = m_managedptr->GetHashCode();
        }
        catch (Apache::Geode::Client::GeodeException^ ex) {
          ex->ThrowNative();
        }
        catch (System::Exception^ ex) {
          Apache::Geode::Client::GeodeException::ThrowNative(ex);
        }
      }

      std::shared_ptr<Delta> PdxManagedCacheableKey::clone()
      {
        try {
          if (auto cloneable = dynamic_cast<ICloneable^>((Apache::Geode::Client::IGeodeDelta^) m_managedDeltaptr)) {
            auto Mclone = dynamic_cast<Apache::Geode::Client::IPdxSerializable^>(cloneable->Clone());
            return std::shared_ptr<Delta>(static_cast<PdxManagedCacheableKey*>(SafeGenericM2UMConvert(Mclone)));
          } else {
            return Delta::clone();
          }
        }
        catch (Apache::Geode::Client::GeodeException^ ex) {
          ex->ThrowNative();
        }
        catch (System::Exception^ ex) {
          Apache::Geode::Client::GeodeException::ThrowNative(ex);
        }
        return nullptr;
      }
    }  // namespace client
  }  // namespace geode
}  // namespace apache
