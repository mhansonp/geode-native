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

#include "ThinClientHARegion.hpp"
#include "TcrHADistributionManager.hpp"
#include "CacheImpl.hpp"
#include <geode/SystemProperties.hpp>
#include "ReadWriteLock.hpp"
#include <geode/PoolManager.hpp>
#include "ThinClientPoolHADM.hpp"
namespace apache {
namespace geode {
namespace client {

ThinClientHARegion::ThinClientHARegion(
    const std::string& name, CacheImpl* cache,
    const std::shared_ptr<RegionInternal>& rPtr,
    const std::shared_ptr<RegionAttributes>& attributes,
    const std::shared_ptr<CacheStatistics>& stats, bool shared,
    bool enableNotification)
    : ThinClientRegion(name, cache, rPtr, attributes, stats, shared),
      m_attribute(attributes),
      m_processedMarker(false),
      m_poolDM(false) {
  setClientNotificationEnabled(enableNotification);
}

void ThinClientHARegion::initTCR() {
  try {
    bool isPool = m_attribute->getPoolName() != nullptr &&
                  strlen(m_attribute->getPoolName()) > 0;
    if (m_cacheImpl->getDistributedSystem()
            .getSystemProperties()
            .isGridClient()) {
      LOGWARN(
          "Region: HA region having notification channel created for grid "
          "client; force starting required notification, cleanup and "
          "failover threads");
      m_cacheImpl->tcrConnectionManager().startFailoverAndCleanupThreads(
          isPool);
    }

    if (m_attribute->getPoolName() == nullptr ||
        strlen(m_attribute->getPoolName()) == 0) {
      m_poolDM = false;
      m_tcrdm = new TcrHADistributionManager(
          this, m_cacheImpl->tcrConnectionManager(),
          m_cacheImpl->getAttributes());
      m_tcrdm->init();
    } else {
      m_tcrdm = dynamic_cast<ThinClientPoolHADM*>(
          m_cacheImpl->getCache()
              ->getPoolManager()
              .find(m_attribute->getPoolName())
              .get());
      if (m_tcrdm) {
        m_poolDM = true;
        // Pool DM should only be inited once and it
        // is already done in PoolFactory::create();
        // m_tcrdm->init();
        ThinClientPoolHADM* poolDM = dynamic_cast<ThinClientPoolHADM*>(m_tcrdm);
        poolDM->addRegion(this);
        poolDM->incRegionCount();

      } else {
        throw IllegalStateException("pool not found");
      }
    }
  } catch (const Exception& ex) {
    GF_SAFE_DELETE(m_tcrdm);
    LOGERROR(
        "ThinClientHARegion: failed to create a DistributionManager "
        "object due to: %s: %s",
        ex.getName(), ex.what());
    throw;
  }
}

void ThinClientHARegion::acquireGlobals(bool isFailover) {
  if (isFailover) {
    ThinClientRegion::acquireGlobals(isFailover);
  } else {
    m_tcrdm->acquireRedundancyLock();
  }
}

void ThinClientHARegion::releaseGlobals(bool isFailover) {
  if (isFailover) {
    ThinClientRegion::releaseGlobals(isFailover);
  } else {
    m_tcrdm->releaseRedundancyLock();
  }
}

void ThinClientHARegion::handleMarker() {
  TryReadGuard guard(m_rwLock, m_destroyPending);
  if (m_destroyPending) {
    return;
  }

  if (m_listener != nullptr && !m_processedMarker) {
    RegionEvent event(shared_from_this(), nullptr, false);
    int64_t sampleStartNanos = startStatOpTime();
    try {
      m_listener->afterRegionLive(event);
    } catch (const Exception& ex) {
      LOGERROR("Exception in CacheListener::afterRegionLive: %s: %s",
               ex.getName(), ex.what());
    } catch (...) {
      LOGERROR("Unknown exception in CacheListener::afterRegionLive");
    }
    m_cacheImpl->getCachePerfStats().incListenerCalls();
    updateStatOpTime(m_regionStats->getStat(),
                     m_regionStats->getListenerCallTimeId(), sampleStartNanos);
    m_regionStats->incListenerCallsCompleted();
  }
  m_processedMarker = true;
}

bool ThinClientHARegion::getProcessedMarker() {
  return m_processedMarker || !isDurableClient();
}

void ThinClientHARegion::destroyDM(bool keepEndpoints) {
  if (m_poolDM) {
    LOGDEBUG(
        "ThinClientHARegion::destroyDM( ): removing region from "
        "ThinClientPoolHADM list.");
    ThinClientPoolHADM* poolDM = dynamic_cast<ThinClientPoolHADM*>(m_tcrdm);
    poolDM->removeRegion(this);
    poolDM->decRegionCount();
  } else {
    ThinClientRegion::destroyDM(keepEndpoints);
  }
}
}  // namespace client
}  // namespace geode
}  // namespace apache

void ThinClientHARegion::addDisMessToQueue() {
  if (m_poolDM) {
    ThinClientPoolHADM* poolDM = dynamic_cast<ThinClientPoolHADM*>(m_tcrdm);
    poolDM->addDisMessToQueue(this);

    if (poolDM->m_redundancyManager->m_globalProcessedMarker &&
        !m_processedMarker) {
      TcrMessage* regionMsg =
          new TcrMessageClientMarker(m_cache->createDataOutput(), true);
      receiveNotification(regionMsg);
    }
  }
}

GfErrType ThinClientHARegion::getNoThrow_FullObject(
    std::shared_ptr<EventId> eventId, std::shared_ptr<Cacheable>& fullObject,
    std::shared_ptr<VersionTag>& versionTag) {
  TcrMessageRequestEventValue fullObjectMsg(m_cache->createDataOutput(),
                                            eventId);
  TcrMessageReply reply(true, nullptr);

  ThinClientPoolHADM* poolHADM = dynamic_cast<ThinClientPoolHADM*>(m_tcrdm);
  GfErrType err = GF_NOTCON;
  if (poolHADM) {
    err = poolHADM->sendRequestToPrimary(fullObjectMsg, reply);
  } else {
    err = static_cast<TcrHADistributionManager*>(m_tcrdm)->sendRequestToPrimary(
        fullObjectMsg, reply);
  }
  if (err == GF_NOERR) {
    fullObject = reply.getValue();
  }
  versionTag = reply.getVersionTag();
  return err;
}
