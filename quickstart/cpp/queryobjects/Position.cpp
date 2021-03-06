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

#include "Position.hpp"
#include <cwchar>
#include <wchar.h>

using namespace apache::geode::client;
using namespace testobject;

int32_t Position::cnt = 0;

Position::Position() { init(); }

Position::Position(const char* id, int32_t out) {
  init();
  secId = CacheableString::create(id);
  qty = out - (cnt % 2 == 0 ? 1000 : 100);
  mktValue = qty * 1.2345998;
  sharesOutstanding = out;
  secType = (wchar_t*)malloc((wcslen(L"a") + 1) * sizeof(wchar_t));
  wcscpy(secType, L"a");
  pid = cnt++;
}

Position::~Position() {
  if (secType != NULL) {
    free(secType);
  }
}

void Position::init() {
  avg20DaysVol = 0;
  bondRating = nullptr;
  convRatio = 0.0;
  country = nullptr;
  delta = 0.0;
  industry = 0;
  issuer = 0;
  mktValue = 0.0;
  qty = 0.0;
  secId = nullptr;
  secLinks = nullptr;
  secType = NULL;
  sharesOutstanding = 0;
  underlyer = nullptr;
  volatility = 0;
  pid = 0;
}

void Position::toData(apache::geode::client::DataOutput& output) const {
  output.writeInt(avg20DaysVol);
  output.writeObject(bondRating);
  output.writeDouble(convRatio);
  output.writeObject(country);
  output.writeDouble(delta);
  output.writeInt(industry);
  output.writeInt(issuer);
  output.writeDouble(mktValue);
  output.writeDouble(qty);
  output.writeObject(secId);
  output.writeObject(secLinks);
  output.writeUTF(secType);
  output.writeInt(sharesOutstanding);
  output.writeObject(underlyer);
  output.writeInt(volatility);
  output.writeInt(pid);
}

apache::geode::client::Serializable* Position::fromData(
    apache::geode::client::DataInput& input) {
  input.readInt(&avg20DaysVol);
  input.readObject(bondRating);
  input.readDouble(&convRatio);
  input.readObject(country);
  input.readDouble(&delta);
  input.readInt(&industry);
  input.readInt(&issuer);
  input.readDouble(&mktValue);
  input.readDouble(&qty);
  input.readObject(secId);
  input.readObject(secLinks);
  input.readUTF(&secType);
  input.readInt(&sharesOutstanding);
  input.readObject(underlyer);
  input.readInt(&volatility);
  input.readInt(&pid);
  return this;
}
std::shared_ptr<CacheableString> Position::toString() const {
  char buf[2048];
  sprintf(buf,
          "Position Object:[ secId=%s type=%ls sharesOutstanding=%d id=%d ]",
          secId->toString(), this->secType, this->sharesOutstanding, this->pid);
  return CacheableString::create(buf);
}
