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

#ifndef GEODE_TCPCONN_H_
#define GEODE_TCPCONN_H_

#include <geode/geode_globals.hpp>
#include "util/Log.hpp"
#include "Connector.hpp"

#include <ace/SOCK_Stream.h>
#include <ace/OS.h>

namespace apache {
namespace geode {
namespace client {

#ifdef WIN32

#define TCPLEVEL IPPROTO_TCP

#else

#include <sys/types.h>
#include <sys/socket.h>

#define TCPLEVEL SOL_TCP

#endif

class CPPCACHE_EXPORT TcpConn : public Connector {
 private:
  ACE_SOCK_Stream* m_io;

 protected:
  ACE_INET_Addr m_addr;
  std::chrono::microseconds m_waitMilliSeconds;

  int32_t m_maxBuffSizePool;

  enum SockOp { SOCK_READ, SOCK_WRITE };

  void clearNagle(ACE_SOCKET sock);
  int32_t maxSize(ACE_SOCKET sock, int32_t flag, int32_t size);

  virtual int32_t socketOp(SockOp op, char* buff, int32_t len,
                           std::chrono::microseconds waitSeconds);

  virtual void createSocket(ACE_SOCKET sock);

 public:
  int m_chunkSize;

  static int getDefaultChunkSize() {
    // Attempt to set chunk size to nearest OS page size
    // for perf improvement
    int pageSize = ACE_OS::getpagesize();
    if (pageSize > 16000000) {
      return 16000000;
    } else if (pageSize > 0) {
      return pageSize + (16000000 / pageSize) * pageSize;
    }

    return 16000000;
  }

  TcpConn(const char* hostname, int32_t port,
          std::chrono::microseconds waitSeconds, int32_t maxBuffSizePool);
  TcpConn(const char* ipaddr, std::chrono::microseconds waitSeconds,
          int32_t maxBuffSizePool);

  virtual ~TcpConn() { close(); }

  // Close this tcp connection
  virtual void close() override;

  void init() override;

  // Listen
  void listen(
      const char* hostname, int32_t port,
      std::chrono::microseconds waitSeconds = DEFAULT_READ_TIMEOUT_SECS);
  void listen(const char* ipaddr, std::chrono::microseconds waitSeconds =
                                      DEFAULT_READ_TIMEOUT_SECS);

  virtual void listen(
      ACE_INET_Addr addr,
      std::chrono::microseconds waitSeconds = DEFAULT_READ_TIMEOUT_SECS);

  // connect
  void connect(const char* hostname, int32_t port,
               std::chrono::microseconds waitSeconds = DEFAULT_CONNECT_TIMEOUT);
  void connect(const char* ipaddr,
               std::chrono::microseconds waitSeconds = DEFAULT_CONNECT_TIMEOUT);

  virtual void connect();

  int32_t receive(char* buff, int32_t len,
                  std::chrono::microseconds waitSeconds) override;
  int32_t send(const char* buff, int32_t len,
               std::chrono::microseconds waitSeconds) override;

  virtual void setOption(int32_t level, int32_t option, void* val,
                         int32_t len) {
    GF_DEV_ASSERT(m_io != nullptr);

    if (m_io->set_option(level, option, val, len) == -1) {
      int32_t lastError = ACE_OS::last_error();
      LOGERROR("Failed to set option, errno: %d: %s", lastError,
               ACE_OS::strerror(lastError));
    }
  }

  void setIntOption(int32_t level, int32_t option, int32_t val) {
    setOption(level, option, (void*)&val, sizeof(int32_t));
  }

  void setBoolOption(int32_t level, int32_t option, bool val) {
    setOption(level, option, (void*)&val, sizeof(bool));
  }

  virtual uint16_t getPort() override;
};
}  // namespace client
}  // namespace geode
}  // namespace apache

#endif  // GEODE_TCPCONN_H_
