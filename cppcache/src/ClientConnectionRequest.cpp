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

#include "ClientConnectionRequest.hpp"
#include <geode/DataOutput.hpp>
#include <geode/DataInput.hpp>
#include "GeodeTypeIdsImpl.hpp"

using namespace apache::geode::client;

void ClientConnectionRequest::toData(DataOutput& output) const {
  output.writeNativeString(m_servergroup.c_str());
  writeSetOfServerLocation(output);
}

void ClientConnectionRequest::fromData(DataInput& input) {}

uint32_t ClientConnectionRequest::objectSize() const { return 0; }

int8_t ClientConnectionRequest::typeId() const {
  return static_cast<int8_t>(GeodeTypeIdsImpl::ClientConnectionRequest);
}

void ClientConnectionRequest::writeSetOfServerLocation(
    DataOutput& output) const {
  output.writeInt(
      static_cast<int32_t>(m_excludeServergroup_serverLocation.size()));
  std::set<ServerLocation>::const_iterator it =
      m_excludeServergroup_serverLocation.begin();
  while (it != m_excludeServergroup_serverLocation.end()) {
    it->toData(output);
    it++;
  }
}
