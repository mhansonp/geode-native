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
#include "TcpSslConn.hpp"

#include <geode/SystemProperties.hpp>
#include <geode/DistributedSystem.hpp>
#include "CacheImpl.hpp"

namespace apache {
namespace geode {
namespace client {

Ssl* TcpSslConn::getSSLImpl(ACE_SOCKET sock, const char* pubkeyfile,
                            const char* privkeyfile) {
  const char* libName = "cryptoImpl";
  if (m_dll.open(libName, RTLD_NOW | RTLD_GLOBAL, 0) == -1) {
    char msg[1000] = {0};
    ACE_OS::snprintf(msg, 1000, "cannot open library: %s", libName);
    LOGERROR(msg);
    throw FileNotFoundException(msg);
  }

  gf_create_SslImpl func =
      reinterpret_cast<gf_create_SslImpl>(m_dll.symbol("gf_create_SslImpl"));
  if (func == nullptr) {
    char msg[1000];
    ACE_OS::snprintf(msg, 1000,
                     "cannot find function %s in library gf_create_SslImpl",
                     "cryptoImpl");
    LOGERROR(msg);
    throw IllegalStateException(msg);
  }
  return reinterpret_cast<Ssl*>(
      func(sock, pubkeyfile, privkeyfile, m_pemPassword));
}

void TcpSslConn::createSocket(ACE_SOCKET sock) {
  LOGDEBUG("Creating SSL socket stream");
  m_ssl = getSSLImpl(sock, m_pubkeyfile, m_privkeyfile);
}

void TcpSslConn::listen(ACE_INET_Addr addr,
                        std::chrono::microseconds waitSeconds) {
  GF_DEV_ASSERT(m_ssl != nullptr);

  int32_t retVal = m_ssl->listen(addr, waitSeconds);

  if (retVal == -1) {
    char msg[256];
    int32_t lastError = ACE_OS::last_error();
    if (lastError == ETIME || lastError == ETIMEDOUT) {
      throw TimeoutException(
          "TcpSslConn::listen Attempt to listen timed out after" +
          util::chrono::duration::to_string(waitSeconds) + ".");
    }
    // sprintf( msg, "TcpSslConn::listen failed with errno: %d: %s", lastError,
    // ACE_OS::strerror(lastError) );
    ACE_OS::snprintf(msg, 255, "TcpSslConn::listen failed with errno: %d: %s",
                     lastError, ACE_OS::strerror(lastError));
    throw GeodeIOException(msg);
  }
}

void TcpSslConn::connect() {
  GF_DEV_ASSERT(m_ssl != nullptr);

  ACE_OS::signal(SIGPIPE, SIG_IGN);  // Ignore broken pipe

  // m_ssl->init();

  std::chrono::microseconds waitMicroSeconds = m_waitMilliSeconds;

  LOGDEBUG("Connecting SSL socket stream to %s:%d waiting %d micro sec",
           m_addr.get_host_name(), m_addr.get_port_number(), waitMicroSeconds.count());

  int32_t retVal = m_ssl->connect(m_addr, waitMicroSeconds);

  if (retVal == -1) {
    char msg[256];
    int32_t lastError = ACE_OS::last_error();
    if (lastError == ETIME || lastError == ETIMEDOUT) {
      // this is only called by constructor, so we must delete m_ssl
      GF_SAFE_DELETE(m_ssl);
      throw TimeoutException(
          "TcpSslConn::connect Attempt to connect timed out after " +
          util::chrono::duration::to_string(waitMicroSeconds) + ".");
    }
    ACE_OS::snprintf(msg, 256, "TcpSslConn::connect failed with errno: %d: %s",
                     lastError, ACE_OS::strerror(lastError));
    // this is only called by constructor, so we must delete m_ssl
    GF_SAFE_DELETE(m_ssl);
    throw GeodeIOException(msg);
  }
}

void TcpSslConn::close() {
  if (m_ssl != nullptr) {
    m_ssl->close();
    gf_destroy_SslImpl func = reinterpret_cast<gf_destroy_SslImpl>(
        m_dll.symbol("gf_destroy_SslImpl"));
    func((void*)m_ssl);
    m_ssl = nullptr;
  }
}

int32_t TcpSslConn::socketOp(TcpConn::SockOp op, char* buff, int32_t len,
                             std::chrono::microseconds waitSeconds) {
  {
    GF_DEV_ASSERT(m_ssl != nullptr);
    GF_DEV_ASSERT(buff != nullptr);

#if GF_DEVEL_ASSERTS == 1
    if (len <= 0) {
      LOGERROR(
          "TcpSslConn::socketOp called with a length of %d specified. "
          "No operation performed.",
          len);
      GF_DEV_ASSERT(false);
    }
#endif
    // passing wait time as micro seconds
    ACE_Time_Value waitTime(waitSeconds);
    ACE_Time_Value endTime(ACE_OS::gettimeofday());
    endTime += waitTime;
    ACE_Time_Value sleepTime(0, 100);
    size_t readLen = 0;
    ssize_t retVal;
    bool errnoSet = false;

    int32_t sendlen = len;
    int32_t totalsend = 0;

    while (len > 0 && waitTime > ACE_Time_Value::zero) {
      if (len > m_chunkSize) {
        sendlen = m_chunkSize;
        len -= m_chunkSize;
      } else {
        sendlen = len;
        len = 0;
      }
      do {
        if (op == SOCK_READ) {
          retVal = m_ssl->recv(buff, sendlen, &waitTime, &readLen);
        } else {
          retVal = m_ssl->send(buff, sendlen, &waitTime, &readLen);
        }
        sendlen -= static_cast<int32_t>(readLen);
        totalsend += static_cast<int32_t>(readLen);
        if (retVal < 0) {
          int32_t lastError = ACE_OS::last_error();
          if (lastError == EAGAIN) {
            ACE_OS::sleep(sleepTime);
          } else {
            errnoSet = true;
            break;
          }
        } else if (retVal == 0 && readLen == 0) {
          ACE_OS::last_error(EPIPE);
          errnoSet = true;
          break;
        }

        buff += readLen;

        waitTime = endTime - ACE_OS::gettimeofday();
        if (waitTime <= ACE_Time_Value::zero) break;
      } while (sendlen > 0);
      if (errnoSet) break;
    }

    if (len > 0 && !errnoSet) {
      ACE_OS::last_error(ETIME);
    }

    GF_DEV_ASSERT(len >= 0);
    return totalsend;
  }
}

uint16_t TcpSslConn::getPort() {
  GF_DEV_ASSERT(m_ssl != nullptr);

  ACE_INET_Addr localAddr;
  m_ssl->getLocalAddr(*(ACE_Addr*)&localAddr);
  return localAddr.get_port_number();
}

}  // namespace client
}  // namespace geode
}  // namespace apache
