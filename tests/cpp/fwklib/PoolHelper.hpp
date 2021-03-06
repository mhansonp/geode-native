#pragma once

#ifndef GEODE_FWKLIB_POOLHELPER_H_
#define GEODE_FWKLIB_POOLHELPER_H_

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

#include "fwklib/FrameworkTest.hpp"
#include "fwklib/FwkObjects.hpp"
#include "fwklib/FwkStrCvt.hpp"
#include "fwklib/FwkLog.hpp"
#include <geode/PoolFactory.hpp>
#include <cstdlib>

#include <string>
#include <map>

namespace apache {
namespace geode {
namespace client {
namespace testframework {

// ----------------------------------------------------------------------------

/** @class PoolHelper
 * @brief Class used to define a valid combination of attributes and
 * specifications for a pool.
 */
class PoolHelper {
  const FwkPool* m_pool;
  std::string m_spec;

 public:
  /** Fill in this instance of PoolHelper based on the spec named by sname.
   */
  PoolHelper(const FrameworkTest* test) : m_pool(NULL) {
    m_spec = test->getStringValue("poolSpec");
    if (m_spec.empty()) {
      FWKEXCEPTION("Failed to find PoolSpec definition.");
    }
    test->bbSet("GFE_BB", "testScheme", " ");
    m_pool = test->getPoolSnippet(m_spec);
    if (m_pool == NULL) {
      FWKEXCEPTION("Failed to find pool definition.");
    }
  }

  const std::string specName() { return m_spec; }

  std::string poolAttributesToString(std::shared_ptr<Pool>& pool) {
    std::string sString;
    sString += "\npoolName: ";
    sString += FwkStrCvt(pool->getName()).toString();
    sString += "\nFreeConnectionTimeout: ";
    sString += FwkStrCvt(pool->getFreeConnectionTimeout()).toString();
    sString += "\nLoadConditioningInterval: ";
    sString += FwkStrCvt(pool->getLoadConditioningInterval()).toString();
    sString += "\nSocketBufferSize: ";
    sString += FwkStrCvt(pool->getSocketBufferSize()).toString();
    sString += "\nReadTimeout: ";
    sString += FwkStrCvt(pool->getReadTimeout()).toString();
    sString += "\nMinConnections: ";
    sString += FwkStrCvt(pool->getMinConnections()).toString();
    sString += "\nMaxConnections: ";
    sString += FwkStrCvt(pool->getMaxConnections()).toString();
    sString += "\nStatisticInterval: ";
    sString += FwkStrCvt(pool->getStatisticInterval()).toString();
    sString += "\nRetryAttempts: ";
    sString += FwkStrCvt(pool->getRetryAttempts()).toString();
    sString += "\nSubscriptionEnabled: ";
    sString += pool->getSubscriptionEnabled() ? "true" : "false";
    sString += "\nSubscriptionRedundancy: ";
    sString += FwkStrCvt(pool->getSubscriptionRedundancy()).toString();
    sString += "\nSubscriptionMessageTrackingTimeout: ";
    sString +=
        FwkStrCvt(pool->getSubscriptionMessageTrackingTimeout()).toString();
    sString += "\nSubscriptionAckInterval: ";
    sString += FwkStrCvt(pool->getSubscriptionAckInterval()).toString();
    sString += "\nServerGroup: ";
    sString += pool->getServerGroup();
    sString += "\nIdleTimeout: ";
    sString += FwkStrCvt((int64_t)pool->getIdleTimeout()).toString();
    sString += "\nPingInterval: ";
    sString += FwkStrCvt((int64_t)pool->getPingInterval()).toString();
    sString += "\nThreadLocalConnections: ";
    sString += pool->getThreadLocalConnections() ? "true" : "false";
    sString += "\nMultiuserAuthentication: ";
    sString += pool->getMultiuserAuthentication() ? "true" : "false";
    sString += "\nPRSingleHopEnabled: ";
    sString += pool->getPRSingleHopEnabled() ? "true" : "false";
    sString += "\nLocator: ";
    auto str = dynamic_cast<CacheableStringArray*>(pool->getLocators().get());
    if (pool->getLocators() != nullptr && pool->getLocators()->length() > 0) {
      for (int32_t stri = 0; stri < str->length(); stri++) {
        sString += str->operator[](stri)->asChar();
        sString += ",";
      }
    }
    sString += "\nServers: ";
    str = dynamic_cast<CacheableStringArray*>(pool->getServers().get());
    if (pool->getServers() != nullptr && pool->getServers()->length() > 0) {
      for (int32_t stri = 0; stri < str->length(); stri++) {
        sString += str->operator[](stri)->asChar();
        sString += ",";
      }
    }
    sString += "\n";
    return sString;
  }

  std::shared_ptr<Pool> createPool() {
    const char* poolName = m_pool->getName().c_str();
    auto pptr = m_pool->m_poolManager.find(poolName);
    if (pptr == nullptr) {
      pptr = m_pool->createPool();
    }
    FWKINFO(" Following are Pool attributes :" << poolAttributesToString(pptr));
    return pptr;
  }
  std::shared_ptr<Pool> createPoolForPerf() {
    auto pptr = m_pool->createPool();
    FWKINFO(" Following are Pool attributes :" << poolAttributesToString(pptr));
    return pptr;
  }
};

}  // namespace testframework
}  // namespace client
}  // namespace geode
}  // namespace apache

// ----------------------------------------------------------------------------

#endif  // GEODE_FWKLIB_POOLHELPER_H_
