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

#include "SSLImpl.hpp"

#include <ace/INET_Addr.h>
#include <ace/SOCK_IO.h>
#include <ace/Guard_T.h>
#include <ace/SSL/SSL_SOCK_Connector.h>
#include <ace/SSL/SSL_SOCK_Acceptor.h>
#include <ace/OS.h>
#include <cstdint>

namespace apache {
namespace geode {
namespace client {

ACE_Recursive_Thread_Mutex SSLImpl::s_mutex;
volatile bool SSLImpl::s_initialized = false;

void *gf_create_SslImpl(ACE_SOCKET sock, const char *pubkeyfile,
                        const char *privkeyfile, const char *pemPassword) {
  return reinterpret_cast<void *>(
      new SSLImpl(sock, pubkeyfile, privkeyfile, pemPassword));
}

void gf_destroy_SslImpl(void *impl) {
  SSLImpl *theLib = reinterpret_cast<SSLImpl *>(impl);
  delete theLib;
}

extern "C" {

static int pem_passwd_cb(char *buf, int size, int rwflag, void *passwd) {
  strncpy(buf, (char *)passwd, size);
  buf[size - 1] = '\0';
  return (strlen(buf));
}
}

SSLImpl::SSLImpl(ACE_SOCKET sock, const char *pubkeyfile,
                 const char *privkeyfile, const char *password) {
  ACE_Guard<ACE_Recursive_Thread_Mutex> guard(SSLImpl::s_mutex);

  if (SSLImpl::s_initialized == false) {
    ACE_SSL_Context *sslctx = ACE_SSL_Context::instance();

    SSL_CTX_set_cipher_list(sslctx->context(), "DEFAULT");
    sslctx->set_mode(ACE_SSL_Context::SSLv23_client);
    sslctx->load_trusted_ca(pubkeyfile);

    if (strlen(password) > 0) {
      SSL_CTX_set_default_passwd_cb(sslctx->context(), pem_passwd_cb);
      SSL_CTX_set_default_passwd_cb_userdata(sslctx->context(),
                                             const_cast<char *>(password));
    }

    sslctx->private_key(privkeyfile);
    sslctx->certificate(privkeyfile);
    SSLImpl::s_initialized = true;
  }
  m_io = new ACE_SSL_SOCK_Stream();
  m_io->set_handle(reinterpret_cast<ACE_HANDLE>(sock));
}

SSLImpl::~SSLImpl() {
  ACE_Guard<ACE_Recursive_Thread_Mutex> guard(SSLImpl::s_mutex);

  if (m_io != NULL) {
    delete m_io;
  }
}

void SSLImpl::close() {
  ACE_Guard<ACE_Recursive_Thread_Mutex> guard(SSLImpl::s_mutex);

  if (m_io != NULL) {
    m_io->close();
  }
}

int SSLImpl::setOption(int level, int option, void *optval, int optlen) {
  return m_io->set_option(level, option, optval, optlen);
}

int SSLImpl::listen(ACE_INET_Addr addr, std::chrono::microseconds waitSeconds) {
  ACE_SSL_SOCK_Acceptor listener(addr, 1);
  if (waitSeconds > std::chrono::microseconds::zero()) {
    ACE_Time_Value wtime(waitSeconds);
    return listener.accept(*m_io, 0, &wtime);
  } else {
    return listener.accept(*m_io, 0);
  }
}

int SSLImpl::connect(ACE_INET_Addr ipaddr,
                     std::chrono::microseconds waitSeconds) {
  ACE_SSL_SOCK_Connector conn;
  if (waitSeconds > std::chrono::microseconds::zero()) {
    ACE_Time_Value wtime(waitSeconds);
    return conn.connect(*m_io, ipaddr, &wtime);
  } else {
    return conn.connect(*m_io, ipaddr);
  }
}

ssize_t SSLImpl::recv(void *buf, size_t len, const ACE_Time_Value *timeout,
                      size_t *bytes_transferred) {
  return m_io->recv_n(buf, len, 0, timeout, bytes_transferred);
}

ssize_t SSLImpl::send(const void *buf, size_t len,
                      const ACE_Time_Value *timeout,
                      size_t *bytes_transferred) {
  return m_io->send_n(buf, len, 0, timeout, bytes_transferred);
}

int SSLImpl::getLocalAddr(ACE_Addr &addr) { return m_io->get_local_addr(addr); }

}  // namespace client
}  // namespace geode
}  // namespace apache
