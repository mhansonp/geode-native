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

#include <string>
#include <cstdlib>
#include <string>

#include <ace/OS.h>
#include <ace/DLL.h>

#include <geode/geode_globals.hpp>
#include <geode/util/chrono/duration.hpp>
#include <geode/SystemProperties.hpp>
#include <geode/ExceptionTypes.hpp>

#include "CppCacheLibrary.hpp"
#include "util/Log.hpp"

#if defined(_WIN32)
#include <windows.h>
#else
#include <dlfcn.h>
#endif


namespace {

const char StatisticsSampleInterval[] = "statistic-sample-rate";
const char StatisticsEnabled[] = "statistic-sampling-enabled";
const char AppDomainEnabled[] = "appdomain-enabled";
const char StatisticsArchiveFile[] = "statistic-archive-file";
const char LogFilename[] = "log-file";
const char LogLevel[] = "log-level";

const char Name[] = "name";
const char JavaConnectionPoolSize[] = "connection-pool-size";
const char DebugStackTraceEnabled[] = "stacktrace-enabled";
// crash dump related properties
const char CrashDumpEnabled[] = "crash-dump-enabled";

const char LicenseFilename[] = "license-file";
const char LicenseType[] = "license-type";
const char CacheXMLFile[] = "cache-xml-file";
const char LogFileSizeLimit[] = "log-file-size-limit";
const char LogDiskSpaceLimit[] = "log-disk-space-limit";
const char StatsFileSizeLimit[] = "archive-file-size-limit";
const char StatsDiskSpaceLimit[] = "archive-disk-space-limit";
const char HeapLRULimit[] = "heap-lru-limit";
const char HeapLRUDelta[] = "heap-lru-delta";
const char MaxSocketBufferSize[] = "max-socket-buffer-size";
const char PingInterval[] = "ping-interval";
const char RedundancyMonitorInterval[] = "redundancy-monitor-interval";
const char DisableShufflingEndpoint[] = "disable-shuffling-of-endpoints";
const char NotifyAckInterval[] = "notify-ack-interval";
const char NotifyDupCheckLife[] = "notify-dupcheck-life";
const char DurableClientId[] = "durable-client-id";
const char DurableTimeout[] = "durable-timeout";
const char ConnectTimeout[] = "connect-timeout";
const char ConnectWaitTimeout[] = "connect-wait-timeout";
const char BucketWaitTimeout[] = "bucket-wait-timeout";
const char ConflateEvents[] = "conflate-events";
const char SecurityClientDhAlgo[] = "security-client-dhalgo";
const char SecurityClientKsPath[] = "security-client-kspath";
const char GridClient[] = "grid-client";
const char AutoReadyForEvents[] = "auto-ready-for-events";
const char SslEnabled[] = "ssl-enabled";
const char TimeStatisticsEnabled[] = "enable-time-statistics";
const char SslKeyStore[] = "ssl-keystore";
const char SslTrustStore[] = "ssl-truststore";
const char SslKeystorePassword[] =
    "ssl-keystore-password";  // adongre: Added for Ticket #758
const char ThreadPoolSize[] = "max-fe-threads";
const char SuspendedTxTimeout[] = "suspended-tx-timeout";
const char DisableChunkHandlerThread[] = "disable-chunk-handler-thread";
const char OnClientDisconnectClearPdxTypeIds[] =
    "on-client-disconnect-clear-pdxType-Ids";
const char TombstoneTimeoutInMSec[] = "tombstone-timeout";
const char DefaultConflateEvents[] = "server";

const char DefaultDurableClientId[] = "";
constexpr auto DefaultDurableTimeout = std::chrono::seconds(300);

constexpr auto DefaultConnectTimeout = std::chrono::seconds(59);
constexpr auto DefaultConnectWaitTimeout = std::chrono::seconds(0);
constexpr auto DefaultBucketWaitTimeout = std::chrono::seconds(0);

constexpr auto DefaultSamplingInterval = std::chrono::seconds(1);
const bool DefaultSamplingEnabled = true;
const bool DefaultAppDomainEnabled = false;

const char DefaultStatArchive[] = "statArchive.gfs";
const char DefaultLogFilename[] = "";  // stdout...

const apache::geode::client::Log::LogLevel DefaultLogLevel =
    apache::geode::client::Log::Config;

const int DefaultJavaConnectionPoolSize = 5;
const bool DefaultDebugStackTraceEnabled = false;  // or true

// defaults for crash dump related properties
const bool DefaultCrashDumpEnabled = true;

const bool DefaultGridClient = false;
const bool DefaultAutoReadyForEvents = true;
const bool DefaultSslEnabled = false;
const bool DefaultTimeStatisticsEnabled = false;  // or true;

const char DefaultSslKeyStore[] = "";
const char DefaultSslTrustStore[] = "";
const char DefaultSslKeystorePassword[] = "";  // adongre: Added for Ticket #758
const char DefaultName[] = "";
const char DefaultCacheXMLFile[] = "";
const uint32_t DefaultLogFileSizeLimit = 0;     // = unlimited
const uint32_t DefaultLogDiskSpaceLimit = 0;    // = unlimited
const uint32_t DefaultStatsFileSizeLimit = 0;   // = unlimited
const uint32_t DefaultStatsDiskSpaceLimit = 0;  // = unlimited

const uint32_t DefaultMaxQueueSize = 80000;
const uint32_t DefaultHeapLRULimit = 0;  // = unlimited, disabled when it is 0
const int32_t DefaultHeapLRUDelta = 10;  // = unlimited, disabled when it is 0

const int32_t DefaultMaxSocketBufferSize = 65 * 1024;
constexpr auto DefaultPingInterval = std::chrono::seconds(10);
constexpr auto DefaultRedundancyMonitorInterval = std::chrono::seconds(10);
constexpr auto DefaultNotifyAckInterval = std::chrono::seconds(1);
constexpr auto DefaultNotifyDupCheckLife = std::chrono::seconds(300);
const char DefaultSecurityPrefix[] = "security-";
const char DefaultSecurityClientDhAlgo[] ATTR_UNUSED = "";
const char DefaultSecurityClientKsPath[] ATTR_UNUSED = "";
const uint32_t DefaultThreadPoolSize = ACE_OS::num_processors() * 2;
constexpr auto DefaultSuspendedTxTimeout = std::chrono::seconds(30);
constexpr auto DefaultTombstoneTimeout = std::chrono::seconds(480);
// not disable; all region api will use chunk handler thread
const bool DefaultDisableChunkHandlerThread = false;
const bool DefaultOnClientDisconnectClearPdxTypeIds = false;

}  // namespace

namespace apache {
namespace geode {
namespace client {

namespace impl {

void* getFactoryFunc(const char* lib, const char* funcName);

}  // namespace impl

SystemProperties::SystemProperties(
    const std::shared_ptr<Properties>& propertiesPtr, const char* configFile)
    : m_statisticsSampleInterval(DefaultSamplingInterval),
      m_statisticsEnabled(DefaultSamplingEnabled),
      m_appDomainEnabled(DefaultAppDomainEnabled),
      m_statisticsArchiveFile(nullptr),
      m_logFilename(nullptr),
      m_logLevel(DefaultLogLevel),
      m_sessions(0 /* setup  later in processProperty */),
      m_name(nullptr),
      m_debugStackTraceEnabled(DefaultDebugStackTraceEnabled),
      m_crashDumpEnabled(DefaultCrashDumpEnabled),
      m_disableShufflingEndpoint(false),
      m_cacheXMLFile(nullptr),
      m_logFileSizeLimit(DefaultLogFileSizeLimit),
      m_logDiskSpaceLimit(DefaultLogDiskSpaceLimit),
      m_statsFileSizeLimit(DefaultStatsFileSizeLimit),
      m_statsDiskSpaceLimit(DefaultStatsDiskSpaceLimit),
      m_maxQueueSize(DefaultMaxQueueSize),
      m_javaConnectionPoolSize(DefaultJavaConnectionPoolSize),
      m_heapLRULimit(DefaultHeapLRULimit),
      m_heapLRUDelta(DefaultHeapLRUDelta),
      m_maxSocketBufferSize(DefaultMaxSocketBufferSize),
      m_pingInterval(DefaultPingInterval),
      m_redundancyMonitorInterval(DefaultRedundancyMonitorInterval),
      m_notifyAckInterval(DefaultNotifyAckInterval),
      m_notifyDupCheckLife(DefaultNotifyDupCheckLife),
      m_securityClientDhAlgo(nullptr),
      m_securityClientKsPath(nullptr),
      m_durableClientId(nullptr),
      m_durableTimeout(DefaultDurableTimeout),
      m_connectTimeout(DefaultConnectTimeout),
      m_connectWaitTimeout(DefaultConnectWaitTimeout),
      m_bucketWaitTimeout(DefaultBucketWaitTimeout),
      m_gridClient(DefaultGridClient),
      m_autoReadyForEvents(DefaultAutoReadyForEvents),
      m_sslEnabled(DefaultSslEnabled),
      m_timestatisticsEnabled(DefaultTimeStatisticsEnabled),
      m_sslKeyStore(nullptr),
      m_sslTrustStore(nullptr),
      m_sslKeystorePassword(nullptr),  // adongre: Added for Ticket #758
      m_conflateEvents(nullptr),
      m_threadPoolSize(DefaultThreadPoolSize),
      m_suspendedTxTimeout(DefaultSuspendedTxTimeout),
      m_tombstoneTimeout(DefaultTombstoneTimeout),
      m_disableChunkHandlerThread(DefaultDisableChunkHandlerThread),
      m_onClientDisconnectClearPdxTypeIds(
          DefaultOnClientDisconnectClearPdxTypeIds) {
  processProperty(ConflateEvents, DefaultConflateEvents);

  processProperty(DurableClientId, DefaultDurableClientId);

  processProperty(SslKeyStore, DefaultSslKeyStore);
  processProperty(SslTrustStore, DefaultSslTrustStore);
  // adongre: Added for Ticket #758
  processProperty(SslKeystorePassword, DefaultSslKeystorePassword);

  processProperty(StatisticsArchiveFile, DefaultStatArchive);

  processProperty(LogFilename, DefaultLogFilename);
  processProperty(CacheXMLFile, DefaultCacheXMLFile);
  processProperty(Name, DefaultName);

  // now that defaults are set, consume files and override the defaults.
  class ProcessPropsVisitor : public Properties::Visitor {
    SystemProperties* m_sysProps;

   public:
    explicit ProcessPropsVisitor(SystemProperties* sysProps)
        : m_sysProps(sysProps) {}
    void visit(const std::shared_ptr<CacheableKey>& key,
               const std::shared_ptr<Cacheable>& value) {
      auto prop = key->toString();
      std::shared_ptr<CacheableString> val;
      if (value != nullptr) {
        val = value->toString();
      }
      m_sysProps->processProperty(prop->asChar(), val->asChar());
    }
  } processPropsVisitor(this);

  m_securityPropertiesPtr = Properties::create();
  auto givenConfigPtr = Properties::create();
  // Load the file from product tree.
  try {
    std::string defsysprops =
        CppCacheLibrary::getProductDir() + "/defaultSystem/geode.properties";
    givenConfigPtr->load(defsysprops.c_str());
  } catch (Exception&) {
    LOGERROR(
        "Unable to determine Product Directory. Please set the "
        "GFCPP environment variable.");
    throw;
  }

  // Load the file from current directory.
  if (configFile == nullptr) {
    givenConfigPtr->load("./geode.properties");
  } else {
    givenConfigPtr->load(configFile);
  }
  // process each loaded property.
  givenConfigPtr->foreach (processPropsVisitor);

  // Now consume any properties provided by the Properties object in code.
  if (propertiesPtr != nullptr) {
    propertiesPtr->foreach (processPropsVisitor);
  }

  m_securityClientDhAlgo = m_securityPropertiesPtr->find(SecurityClientDhAlgo);
  m_securityClientKsPath = m_securityPropertiesPtr->find(SecurityClientKsPath);

}

SystemProperties::~SystemProperties() {
  GF_SAFE_DELETE_ARRAY(m_statisticsArchiveFile);
  GF_SAFE_DELETE_ARRAY(m_logFilename);
  GF_SAFE_DELETE_ARRAY(m_name);
  GF_SAFE_DELETE_ARRAY(m_cacheXMLFile);
  GF_SAFE_DELETE_ARRAY(m_durableClientId);
  GF_SAFE_DELETE_ARRAY(m_sslKeyStore);
  GF_SAFE_DELETE_ARRAY(m_sslTrustStore);
  // adongre: Added for Ticket #758
  GF_SAFE_DELETE_ARRAY(m_sslKeystorePassword);
  GF_SAFE_DELETE_ARRAY(m_conflateEvents);
}

void SystemProperties::throwError(const char* msg) {
  LOGERROR(msg);
  throw GeodeConfigException(msg);
}

template <class _Rep, class _Period>
void SystemProperties::parseDurationProperty(
    const std::string& property, const std::string& value,
    std::chrono::duration<_Rep, _Period>& duration) {
  try {
    duration = util::chrono::duration::from_string<
        std::chrono::duration<_Rep, _Period>>(value);
  } catch (std::invalid_argument&) {
    throwError(
        ("SystemProperties: non-duration " + property + "=" + value).c_str());
  }
}

void SystemProperties::processProperty(const char* property,
                                       const char* value) {
  std::string prop = property;
  if (prop == ThreadPoolSize) {
    char* end;
    uint32_t si = strtoul(value, &end, 10);
    if (!*end) {
      m_threadPoolSize = si;
    } else {
      throwError(
          ("SystemProperties: non-integer " + prop + "=" + value).c_str());
    }
  } else if (prop == MaxSocketBufferSize) {
    char* end;
    long si = strtol(value, &end, 10);
    if (!*end) {
      m_maxSocketBufferSize = si;
    } else {
      throwError(
          ("SystemProperties: non-integer " + prop + "=" + value).c_str());
    }

  } else if (prop == PingInterval) {
    try {
      m_pingInterval =
          util::chrono::duration::from_string<decltype(m_pingInterval)>(
              std::string(value));
    } catch (std::invalid_argument&) {
      throwError(
          ("SystemProperties: non-duration " + prop + "=" + value).c_str());
    }
  } else if (prop == RedundancyMonitorInterval) {
    try {
      m_redundancyMonitorInterval =
          util::chrono::duration::from_string<decltype(
              m_redundancyMonitorInterval)>(std::string(value));
    } catch (std::invalid_argument&) {
      throwError(
          ("SystemProperties: non-duration " + prop + "=" + value).c_str());
    }
  } else if (prop == NotifyAckInterval) {
    try {
      m_notifyAckInterval =
          util::chrono::duration::from_string<decltype(m_notifyAckInterval)>(
              std::string(value));
    } catch (std::invalid_argument&) {
      throwError(
          ("SystemProperties: non-duration " + prop + "=" + value).c_str());
    }
  } else if (prop == NotifyDupCheckLife) {
    parseDurationProperty(prop, std::string(value), m_notifyDupCheckLife);
  } else if (prop == StatisticsSampleInterval) {
    parseDurationProperty(prop, std::string(value), m_statisticsSampleInterval);
  } else if (prop == DurableTimeout) {
    parseDurationProperty(prop, std::string(value), m_durableTimeout);
  } else if (prop == ConnectTimeout) {
    parseDurationProperty(prop, std::string(value), m_connectTimeout);
  } else if (prop == ConnectWaitTimeout) {
    parseDurationProperty(prop, std::string(value), m_connectWaitTimeout);
  } else if (prop == BucketWaitTimeout) {
    parseDurationProperty(prop, std::string(value), m_bucketWaitTimeout);
  } else if (prop == DisableShufflingEndpoint) {
    std::string val = value;
    if (val == "false") {
      m_disableShufflingEndpoint = false;
    } else if (val == "true") {
      m_disableShufflingEndpoint = true;
    }
  } else if (prop == AppDomainEnabled) {
    std::string val = value;
    if (val == "false") {
      m_appDomainEnabled = false;
    } else if (val == "true") {
      m_appDomainEnabled = true;
    }
  } else if (prop == DebugStackTraceEnabled) {
    std::string val = value;
    if (val == "false") {
      m_debugStackTraceEnabled = false;
    } else if (val == "true") {
      m_debugStackTraceEnabled = true;
    } else {
      throwError(("SystemProperties: non-boolean " + prop + "=" + val).c_str());
    }

  } else if (prop == GridClient) {
    std::string val = value;
    if (val == "false") {
      m_gridClient = false;
    } else if (val == "true") {
      m_gridClient = true;
    } else {
      throwError(("SystemProperties: non-boolean " + prop + "=" + val).c_str());
    }
  } else if (prop == AutoReadyForEvents) {
    std::string val = value;
    if (val == "false") {
      m_autoReadyForEvents = false;
    } else if (val == "true") {
      m_autoReadyForEvents = true;
    } else {
      throwError(("SystemProperties: non-boolean " + prop + "=" + val).c_str());
    }
  } else if (prop == SslEnabled) {
    std::string val = value;
    if (val == "false") {
      m_sslEnabled = false;
    } else if (val == "true") {
      m_sslEnabled = true;
    } else {
      throwError(("SystemProperties: non-boolean " + prop + "=" + val).c_str());
    }

  } else if (prop == TimeStatisticsEnabled) {
    std::string val = value;
    if (val == "false") {
      m_timestatisticsEnabled = false;
    } else if (val == "true") {
      m_timestatisticsEnabled = true;
    } else {
      throwError(("SystemProperties: non-boolean " + prop + "=" + val).c_str());
    }

  } else if (prop == CrashDumpEnabled) {
    std::string val = value;
    if (val == "false") {
      m_crashDumpEnabled = false;
    } else if (val == "true") {
      m_crashDumpEnabled = true;
    } else {
      throwError(("SystemProperties: non-boolean " + prop + "=" + val).c_str());
    }
  } else if (prop == StatisticsEnabled) {
    std::string val = value;
    if (val == "false") {
      m_statisticsEnabled = false;
    } else if (val == "true") {
      m_statisticsEnabled = true;
    } else {
      throwError(("SystemProperties: non-boolean " + prop + "=" + val).c_str());
    }

  } else if (prop == StatisticsArchiveFile) {
    if (m_statisticsArchiveFile != nullptr) {
      delete[] m_statisticsArchiveFile;
    }
    size_t len = strlen(value) + 1;
    m_statisticsArchiveFile = new char[len];
    ACE_OS::strncpy(m_statisticsArchiveFile, value, len);

  } else if (prop == LogFilename) {
    if (m_logFilename != nullptr) {
      delete[] m_logFilename;
    }
    if (value != nullptr) {
      size_t len = strlen(value) + 1;
      m_logFilename = new char[len];
      ACE_OS::strncpy(m_logFilename, value, len);
    }
  } else if (prop == LogLevel) {
    try {
      Log::LogLevel level = Log::charsToLevel(value);
      m_logLevel = level;

    } catch (IllegalArgumentException&) {
      throwError(("SystemProperties: unknown log level " + prop + "=" + value)
                     .c_str());
    }

  } else if (prop == JavaConnectionPoolSize) {
    char* end;
    long si = strtol(value, &end, 10);
    if (!*end) {
      m_javaConnectionPoolSize = si;
    } else {
      throwError(
          ("SystemProperties: non-integer " + prop + "=" + value).c_str());
    }

  } else if (prop == Name) {
    if (m_name != nullptr) {
      delete[] m_name;
    }
    if (value != nullptr) {
      size_t len = strlen(value) + 1;
      m_name = new char[len];
      ACE_OS::strncpy(m_name, value, len);
    }
  } else if (prop == DurableClientId) {
    if (m_durableClientId != nullptr) {
      delete[] m_durableClientId;
      m_durableClientId = nullptr;
    }
    if (value != nullptr) {
      size_t len = strlen(value) + 1;
      m_durableClientId = new char[len];
      ACE_OS::strncpy(m_durableClientId, value, len);
    }
  } else if (prop == SslKeyStore) {
    if (m_sslKeyStore != nullptr) {
      delete[] m_sslKeyStore;
      m_sslKeyStore = nullptr;
    }
    if (value != nullptr) {
      size_t len = strlen(value) + 1;
      m_sslKeyStore = new char[len];
      ACE_OS::strncpy(m_sslKeyStore, value, len);
    }
  } else if (prop == SslTrustStore) {
    if (m_sslTrustStore != nullptr) {
      delete[] m_sslTrustStore;
      m_sslTrustStore = nullptr;
    }
    if (value != nullptr) {
      size_t len = strlen(value) + 1;
      m_sslTrustStore = new char[len];
      ACE_OS::strncpy(m_sslTrustStore, value, len);
    }
    // adongre: Added for Ticket #758
  } else if (prop == SslKeystorePassword) {
    if (m_sslKeystorePassword != nullptr) {
      delete[] m_sslKeystorePassword;
      m_sslKeystorePassword = nullptr;
    }
    if (value != nullptr) {
      size_t len = strlen(value) + 1;
      m_sslKeystorePassword = new char[len];
      ACE_OS::strncpy(m_sslKeystorePassword, value, len);
    }
  } else if (prop == ConflateEvents) {
    if (m_conflateEvents != nullptr) {
      delete[] m_conflateEvents;
      m_conflateEvents = nullptr;
    }
    if (value != nullptr) {
      size_t len = strlen(value) + 1;
      m_conflateEvents = new char[len];
      ACE_OS::strncpy(m_conflateEvents, value, len);
    }
  } else if (prop == LicenseFilename) {
    // ignore license-file
  } else if (prop == LicenseType) {
    // ignore license-type
  } else if (prop == CacheXMLFile) {
    if (m_cacheXMLFile != nullptr) {
      delete[] m_cacheXMLFile;
    }
    if (value != nullptr) {
      size_t len = strlen(value) + 1;
      m_cacheXMLFile = new char[len];
      ACE_OS::strncpy(m_cacheXMLFile, value, len);
    }

  } else if (prop == LogFileSizeLimit) {
    char* end;
    long si = strtol(value, &end, 10);
    if (!*end) {
      m_logFileSizeLimit = si;

    } else {
      throwError(
          ("SystemProperties: non-integer " + prop + "=" + value).c_str());
    }
  } else if (prop == LogDiskSpaceLimit) {
    char* end;
    long si = strtol(value, &end, 10);
    if (!*end) {
      m_logDiskSpaceLimit = si;

    } else {
      throwError(
          ("SystemProperties: non-integer " + prop + "=" + value).c_str());
    }
  } else if (prop == StatsFileSizeLimit) {
    char* end;
    long si = strtol(value, &end, 10);
    if (!*end) {
      m_statsFileSizeLimit = si;

    } else {
      throwError(
          ("SystemProperties: non-integer " + prop + "=" + value).c_str());
    }

  } else if (prop == StatsDiskSpaceLimit) {
    char* end;
    long si = strtol(value, &end, 10);
    if (!*end) {
      m_statsDiskSpaceLimit = si;
    } else {
      throwError(
          ("SystemProperties: non-integer " + prop + "=" + value).c_str());
    }

  } else if (prop == HeapLRULimit) {
    char* end;
    long si = strtol(value, &end, 10);
    if (!*end) {
      m_heapLRULimit = si;

    } else {
      throwError(
          ("SystemProperties: non-integer " + prop + "=" + value).c_str());
    }
  } else if (prop == HeapLRUDelta) {
    char* end;
    long si = strtol(value, &end, 10);
    if (!*end) {
      m_heapLRUDelta = si;

    } else {
      throwError(
          ("SystemProperties: non-integer " + prop + "=" + value).c_str());
    }
  } else if (prop == SuspendedTxTimeout) {
    parseDurationProperty(prop, std::string(value), m_suspendedTxTimeout);
  } else if (prop == TombstoneTimeoutInMSec) {
    parseDurationProperty(prop, std::string(value), m_tombstoneTimeout);
  } else if (strncmp(property, DefaultSecurityPrefix,
                     sizeof(DefaultSecurityPrefix) - 1) == 0) {
    m_securityPropertiesPtr->insert(property, value);
  } else if (prop == DisableChunkHandlerThread) {
    std::string val = value;
    if (val == "false") {
      m_disableChunkHandlerThread = false;
    } else if (val == "true") {
      m_disableChunkHandlerThread = true;
    } else {
      throwError(("SystemProperties: non-boolean " + prop + "=" + val).c_str());
    }
  } else if (prop == OnClientDisconnectClearPdxTypeIds) {
    std::string val = value;
    if (val == "false") {
      m_onClientDisconnectClearPdxTypeIds = false;
    } else if (val == "true") {
      m_onClientDisconnectClearPdxTypeIds = true;
    } else {
      throwError(("SystemProperties: non-boolean " + prop + "=" + val).c_str());
    }
  } else {
    char msg[1000];
    ACE_OS::snprintf(msg, 1000, "SystemProperties: unknown property: %s = %s",
                     property, value);
    throwError(msg);
  }
}

void SystemProperties::logSettings() {
  // *** PLEASE ADD IN ALPHABETICAL ORDER - USER VISIBLE ***

  std::string settings = "Geode Native Client System Properties:";

  settings += "\n  appdomain-enabled = ";
  settings += isAppDomainEnabled() ? "true" : "false";

  settings += "\n  archive-disk-space-limit = ";
  settings += std::to_string(statsDiskSpaceLimit());

  settings += "\n  archive-file-size-limit = ";
  settings += std::to_string(statsFileSizeLimit());

  settings += "\n  auto-ready-for-events = ";
  settings += autoReadyForEvents() ? "true" : "false";

  settings += "\n  bucket-wait-timeout = ";
  settings += util::chrono::duration::to_string(bucketWaitTimeout());

  settings += "\n  cache-xml-file = ";
  settings += cacheXMLFile();

  settings += "\n  conflate-events = ";
  settings += conflateEvents();

  settings += "\n  connect-timeout = ";
  settings += util::chrono::duration::to_string(connectTimeout());

  settings += "\n  connection-pool-size = ";
  settings += std::to_string(javaConnectionPoolSize());

  settings += "\n  connect-wait-timeout = ";
  settings += util::chrono::duration::to_string(connectWaitTimeout());

  settings += "\n  crash-dump-enabled = ";
  settings += crashDumpEnabled() ? "true" : "false";

  settings += "\n  disable-chunk-handler-thread = ";
  settings += disableChunkHandlerThread() ? "true" : "false";

  settings += "\n  disable-shuffling-of-endpoints = ";
  settings += isEndpointShufflingDisabled() ? "true" : "false";

  settings += "\n  durable-client-id = ";
  settings += durableClientId();

  settings += "\n  durable-timeout = ";
  settings += util::chrono::duration::to_string(durableTimeout());

  // *** PLEASE ADD IN ALPHABETICAL ORDER - USER VISIBLE ***

  settings += "\n  enable-time-statistics = ";
  settings += getEnableTimeStatistics() ? "true" : "false";

  settings += "\n  grid-client = ";
  settings += isGridClient() ? "true" : "false";

  settings += "\n  heap-lru-delta = ";
  settings += std::to_string(heapLRUDelta());
  /* adongre  - Coverity II
   * CID 29195: Printf arg type mismatch (PW.PRINTF_ARG_MISMATCH)
   */
  settings += "\n  heap-lru-limit = ";
  settings += std::to_string(heapLRULimit());

  // settings += "\n  license-file = ";
  // settings += licenseFilename();

  // settings += "\n  license-type = ";
  // settings += licenseType();

  settings += "\n  log-disk-space-limit = ";
  settings += std::to_string(logDiskSpaceLimit());

  settings += "\n  log-file = ";
  settings += logFilename();

  settings += "\n  log-file-size-limit = ";
  settings += std::to_string(logFileSizeLimit());

  settings += "\n  log-level = ";
  settings += Log::levelToChars(logLevel());

  settings += "\n  max-fe-threads = ";
  settings += std::to_string(threadPoolSize());

  settings += "\n  max-socket-buffer-size = ";
  settings += std::to_string(maxSocketBufferSize());

  settings += "\n  notify-ack-interval = ";
  settings += util::chrono::duration::to_string(notifyAckInterval());

  settings += "\n  notify-dupcheck-life = ";
  settings += util::chrono::duration::to_string(notifyDupCheckLife());

  settings += "\n  on-client-disconnect-clear-pdxType-Ids = ";
  settings += onClientDisconnectClearPdxTypeIds() ? "true" : "false";

  // *** PLEASE ADD IN ALPHABETICAL ORDER - USER VISIBLE ***

  settings += "\n  ping-interval = ";
  settings += util::chrono::duration::to_string(pingInterval());

  settings += "\n  redundancy-monitor-interval = ";
  settings += util::chrono::duration::to_string(redundancyMonitorInterval());

  settings += "\n  security-client-dhalgo = ";
  settings += securityClientDhAlgo();

  settings += "\n  security-client-kspath = ";
  settings += securityClientKsPath();

  settings += "\n  ssl-enabled = ";
  settings += sslEnabled() ? "true" : "false";

  settings += "\n  ssl-keystore = ";
  settings += sslKeyStore();

  settings += "\n  ssl-truststore = ";
  settings += sslTrustStore();

  // settings += "\n ssl-keystore-password = ";
  // settings += sslKeystorePassword();

  settings += "\n  stacktrace-enabled = ";
  settings += debugStackTraceEnabled() ? "true" : "false";

  settings += "\n  statistic-archive-file = ";
  settings += statisticsArchiveFile();

  settings += "\n  statistic-sampling-enabled = ";
  settings += statisticsEnabled() ? "true" : "false";

  settings += "\n  statistic-sample-rate = ";
  settings += util::chrono::duration::to_string(statisticsSampleInterval());

  settings += "\n  suspended-tx-timeout = ";
  settings += util::chrono::duration::to_string(suspendedTxTimeout());

  settings += "\n  tombstone-timeout = ";
  settings += util::chrono::duration::to_string(tombstoneTimeout());

  // *** PLEASE ADD IN ALPHABETICAL ORDER - USER VISIBLE ***

  LOGCONFIG(settings.c_str());
}

}  // namespace client
}  // namespace geode
}  // namespace apache
