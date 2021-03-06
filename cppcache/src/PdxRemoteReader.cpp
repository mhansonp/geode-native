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
/*
 * PdxRemoteReader.cpp
 *
 *  Created on: Nov 3, 2011
 *      Author: npatel
 */

#include "PdxRemoteReader.hpp"
#include "PdxTypes.hpp"

namespace apache {
namespace geode {
namespace client {

PdxRemoteReader::~PdxRemoteReader() {
  // TODO Auto-generated destructor stub
}

wchar_t PdxRemoteReader::readWideChar(const char* fieldName) {
  int choice = m_localToRemoteMap[m_currentIndex++];

  switch (choice) {
    case -2:
      return PdxLocalReader::readWideChar(fieldName);  // in same order
    case -1: {
      return '\0';  // null value
    }
    default: {
      // sequence id read field and then update
      int position = m_pdxType->getFieldPosition(
          choice, m_offsetsBuffer, m_offsetSize, m_serializedLength);
      PdxLocalReader::resettoPdxHead();
      m_dataInput->advanceCursor(position);
      wchar_t retVal = PdxLocalReader::readWideChar(fieldName);
      PdxLocalReader::resettoPdxHead();
      return retVal;
    }
  }
}

char PdxRemoteReader::readChar(const char* fieldName) {
  int choice = m_localToRemoteMap[m_currentIndex++];

  switch (choice) {
    case -2:
      return PdxLocalReader::readChar(fieldName);  // in same order
    case -1: {
      return '\0';  // null value
    }
    default: {
      // sequence id read field and then update
      int position = m_pdxType->getFieldPosition(
          choice, m_offsetsBuffer, m_offsetSize, m_serializedLength);
      PdxLocalReader::resettoPdxHead();
      m_dataInput->advanceCursor(position);
      char retVal = PdxLocalReader::readChar(fieldName);
      PdxLocalReader::resettoPdxHead();
      return retVal;
    }
  }
}

bool PdxRemoteReader::readBoolean(const char* fieldName) {
  int choice = m_localToRemoteMap[m_currentIndex++];

  switch (choice) {
    case -2: {
      return PdxLocalReader::readBoolean(fieldName);  // in same order
    }
    case -1: {
      return 0;  // null value
    }
    default: {
      // sequence id read field and then update
      int position = m_pdxType->getFieldPosition(
          choice, m_offsetsBuffer, m_offsetSize, m_serializedLength);
      if (position != -1) {
        PdxLocalReader::resettoPdxHead();
        m_dataInput->advanceCursor(position);
        bool retVal = PdxLocalReader::readBoolean(fieldName);
        PdxLocalReader::resettoPdxHead();
        return retVal;
      } else {
        return 0;  // null value
      }
    }
  }
}

int8_t PdxRemoteReader::readByte(const char* fieldName) {
  int choice = m_localToRemoteMap[m_currentIndex++];

  switch (choice) {
    case -2: {
      return PdxLocalReader::readByte(fieldName);  // in same order
    }
    case -1: {
      return 0;
    }
    default: {
      // sequence id read field and then update
      int position = m_pdxType->getFieldPosition(
          choice, m_offsetsBuffer, m_offsetSize, m_serializedLength);
      if (position != -1) {
        PdxLocalReader::resettoPdxHead();
        m_dataInput->advanceCursor(position);
        int8_t retValue;
        retValue = PdxLocalReader::readByte(fieldName);
        PdxLocalReader::resettoPdxHead();
        return retValue;
      } else {
        return 0;  // null value
      }
    }
  }
}

int16_t PdxRemoteReader::readShort(const char* fieldName) {
  int choice = m_localToRemoteMap[m_currentIndex++];

  switch (choice) {
    case -2: {
      return PdxLocalReader::readShort(fieldName);  // in same order
    }
    case -1: {
      return 0;  // null value
    }
    default: {
      // sequence id read field and then update
      int position = m_pdxType->getFieldPosition(
          choice, m_offsetsBuffer, m_offsetSize, m_serializedLength);
      if (position != -1) {
        int16_t value;
        PdxLocalReader::resettoPdxHead();
        m_dataInput->advanceCursor(position);
        value = PdxLocalReader::readShort(fieldName);
        PdxLocalReader::resettoPdxHead();
        return value;
      } else {
        return 0;  // null value
      }
    }
  }
}


int32_t PdxRemoteReader::readInt(const char* fieldName) {
  int choice = m_localToRemoteMap[m_currentIndex++];

  switch (choice) {
    case -2: {
      return PdxLocalReader::readInt(fieldName);  // in same order
    }
    case -1: {
      return 0;  // null value
    }
    default: {
      // sequence id read field and then update
      int position = m_pdxType->getFieldPosition(
          choice, m_offsetsBuffer, m_offsetSize, m_serializedLength);
      if (position != -1) {
        int32_t value;
        PdxLocalReader::resettoPdxHead();
        m_dataInput->advanceCursor(position);
        value = PdxLocalReader::readInt(fieldName);
        PdxLocalReader::resettoPdxHead();
        return value;
      } else {
        return 0;  // null value
      }
    }
  }
}

int64_t PdxRemoteReader::readLong(const char* fieldName) {
  int choice = m_localToRemoteMap[m_currentIndex++];

  switch (choice) {
    case -2: {
      return PdxLocalReader::readLong(fieldName);  // in same order
    }
    case -1: {
      return 0;  // null value
    }
    default: {
      // sequence id read field and then update
      int position = m_pdxType->getFieldPosition(
          choice, m_offsetsBuffer, m_offsetSize, m_serializedLength);
      if (position != -1) {
        int64_t value;
        PdxLocalReader::resettoPdxHead();
        m_dataInput->advanceCursor(position);
        value = PdxLocalReader::readLong(fieldName);
        PdxLocalReader::resettoPdxHead();
        return value;
      } else {
        return 0;  // null value
      }
    }
  }
}

float PdxRemoteReader::readFloat(const char* fieldName) {
  int choice = m_localToRemoteMap[m_currentIndex++];

  switch (choice) {
    case -2: {
      return PdxLocalReader::readFloat(fieldName);  // in same order
    }
    case -1: {
      return 0.0f;  // null value
    }
    default: {
      // sequence id read field and then update
      int position = m_pdxType->getFieldPosition(
          choice, m_offsetsBuffer, m_offsetSize, m_serializedLength);
      if (position != -1) {
        float value;
        PdxLocalReader::resettoPdxHead();
        m_dataInput->advanceCursor(position);
        value = PdxLocalReader::readFloat(fieldName);
        PdxLocalReader::resettoPdxHead();
        return value;
      } else {
        return 0.0f;  // null value
      }
    }
  }
}

double PdxRemoteReader::readDouble(const char* fieldName) {
  int choice = m_localToRemoteMap[m_currentIndex++];

  switch (choice) {
    case -2: {
      return PdxLocalReader::readDouble(fieldName);  // in same order
    }
    case -1: {
      return 0.0;  // null value
    }
    default: {
      // sequence id read field and then update
      int position = m_pdxType->getFieldPosition(
          choice, m_offsetsBuffer, m_offsetSize, m_serializedLength);
      if (position != -1) {
        double value;
        PdxLocalReader::resettoPdxHead();
        m_dataInput->advanceCursor(position);
        value = PdxLocalReader::readDouble(fieldName);
        PdxLocalReader::resettoPdxHead();
        return value;
      } else {
        return 0.0;  // null value
      }
    }
  }
}

char* PdxRemoteReader::readString(const char* fieldName) {
  int choice = m_localToRemoteMap[m_currentIndex++];

  switch (choice) {
    case -2: {
      return PdxLocalReader::readString(fieldName);
    }
    case -1: {
      static char emptyString[] = {static_cast<char>(0)};
      return emptyString;
    }
    default: {
      // sequence id read field and then update
      int position = m_pdxType->getFieldPosition(
          choice, m_offsetsBuffer, m_offsetSize, m_serializedLength);
      if (position != -1) {
        PdxLocalReader::resettoPdxHead();
        m_dataInput->advanceCursor(position);
        char* retVal = PdxLocalReader::readString(fieldName);
        PdxLocalReader::resettoPdxHead();
        return retVal;
      } else {
        static char emptyString[] = {static_cast<char>(0)};
        return emptyString;
      }
    }
  }
}

wchar_t* PdxRemoteReader::readWideString(const char* fieldName) {
  int choice = m_localToRemoteMap[m_currentIndex++];

  switch (choice) {
    case -2: {
      return PdxLocalReader::readWideString(fieldName);
    }
    case -1: {
      static wchar_t emptyString[] = {static_cast<wchar_t>(0)};
      return emptyString;
    }
    default: {
      // sequence id read field and then update
      int position = m_pdxType->getFieldPosition(
          choice, m_offsetsBuffer, m_offsetSize, m_serializedLength);
      if (position != -1) {
        PdxLocalReader::resettoPdxHead();
        m_dataInput->advanceCursor(position);
        wchar_t* retVal = PdxLocalReader::readWideString(fieldName);
        PdxLocalReader::resettoPdxHead();
        return retVal;
      } else {
        static wchar_t emptyString[] = {static_cast<wchar_t>(0)};
        return emptyString;
      }
    }
  }
}
std::shared_ptr<Serializable> PdxRemoteReader::readObject(
    const char* fieldName) {
  int choice = m_localToRemoteMap[m_currentIndex++];

  switch (choice) {
    case -2: {
      return PdxLocalReader::readObject(fieldName);  // in same order
    }
    case -1: {
      return nullptr;
    }
    default: {
      // sequence id read field and then update
      int position = m_pdxType->getFieldPosition(
          choice, m_offsetsBuffer, m_offsetSize, m_serializedLength);
      if (position != -1) {
        std::shared_ptr<Serializable> ptr;
        PdxLocalReader::resettoPdxHead();
        m_dataInput->advanceCursor(position);
        ptr = PdxLocalReader::readObject(fieldName);
        PdxLocalReader::resettoPdxHead();
        return ptr;
      } else {
        return nullptr;  // null value
      }
    }
  }
}

char* PdxRemoteReader::readCharArray(const char* fieldName, int32_t& length) {
  int choice = m_localToRemoteMap[m_currentIndex++];

  switch (choice) {
    case -2:
      return PdxLocalReader::readCharArray(fieldName, length);  // in same order
    case -1: {
      return nullptr;  // null value
    }
    default: {
      // sequence id read field and then update
      int position = m_pdxType->getFieldPosition(
          choice, m_offsetsBuffer, m_offsetSize, m_serializedLength);
      PdxLocalReader::resettoPdxHead();
      m_dataInput->advanceCursor(position);
      char* retVal = PdxLocalReader::readCharArray(fieldName, length);
      PdxLocalReader::resettoPdxHead();
      return retVal;
    }
  }
}

wchar_t* PdxRemoteReader::readWideCharArray(const char* fieldName,
                                            int32_t& length) {
  int choice = m_localToRemoteMap[m_currentIndex++];

  switch (choice) {
    case -2:
      return PdxLocalReader::readWideCharArray(fieldName,
                                               length);  // in same order
    case -1: {
      return nullptr;  // null value
    }
    default: {
      // sequence id read field and then update
      int position = m_pdxType->getFieldPosition(
          choice, m_offsetsBuffer, m_offsetSize, m_serializedLength);
      PdxLocalReader::resettoPdxHead();
      m_dataInput->advanceCursor(position);
      wchar_t* retVal = PdxLocalReader::readWideCharArray(fieldName, length);
      PdxLocalReader::resettoPdxHead();
      return retVal;
    }
  }
}

bool* PdxRemoteReader::readBooleanArray(const char* fieldName,
                                        int32_t& length) {
  int choice = m_localToRemoteMap[m_currentIndex++];

  switch (choice) {
    case -2:
      return PdxLocalReader::readBooleanArray(fieldName,
                                              length);  // in same order
    case -1: {
      return nullptr;  // null value
    }
    default: {
      // sequence id read field and then update
      int position = m_pdxType->getFieldPosition(
          choice, m_offsetsBuffer, m_offsetSize, m_serializedLength);
      PdxLocalReader::resettoPdxHead();
      m_dataInput->advanceCursor(position);
      bool* retVal = PdxLocalReader::readBooleanArray(fieldName, length);
      PdxLocalReader::resettoPdxHead();
      return retVal;
    }
  }
}

int8_t* PdxRemoteReader::readByteArray(const char* fieldName, int32_t& length) {
  int choice = m_localToRemoteMap[m_currentIndex++];

  switch (choice) {
    case -2: {
      return PdxLocalReader::readByteArray(fieldName, length);  // in same order
    }
    case -1: {
      return nullptr;
    }
    default: {
      // sequence id read field and then update
      int position = m_pdxType->getFieldPosition(
          choice, m_offsetsBuffer, m_offsetSize, m_serializedLength);
      if (position != -1) {
        int8_t* byteArrptr;
        PdxLocalReader::resettoPdxHead();
        m_dataInput->advanceCursor(position);
        byteArrptr = PdxLocalReader::readByteArray(fieldName, length);
        PdxLocalReader::resettoPdxHead();
        return byteArrptr;
      } else {
        return nullptr;
      }
    }
  }
}

int16_t* PdxRemoteReader::readShortArray(const char* fieldName,
                                         int32_t& length) {
  int choice = m_localToRemoteMap[m_currentIndex++];

  switch (choice) {
    case -2: {
      return PdxLocalReader::readShortArray(fieldName, length);  // in same
                                                                 // order
    }
    case -1: {
      return nullptr;
    }
    default: {
      // sequence id read field and then update
      int position = m_pdxType->getFieldPosition(
          choice, m_offsetsBuffer, m_offsetSize, m_serializedLength);
      if (position != -1) {
        int16_t* shortArrptr;
        PdxLocalReader::resettoPdxHead();
        m_dataInput->advanceCursor(position);
        shortArrptr = PdxLocalReader::readShortArray(fieldName, length);
        PdxLocalReader::resettoPdxHead();
        return shortArrptr;
      } else {
        return nullptr;
      }
    }
  }
}

int32_t* PdxRemoteReader::readIntArray(const char* fieldName, int32_t& length) {
  int choice = m_localToRemoteMap[m_currentIndex++];

  switch (choice) {
    case -2: {
      return PdxLocalReader::readIntArray(fieldName, length);  // in same order
    }
    case -1: {
      return nullptr;
    }
    default: {
      // sequence id read field and then update
      int position = m_pdxType->getFieldPosition(
          choice, m_offsetsBuffer, m_offsetSize, m_serializedLength);
      if (position != -1) {
        int32_t* intArrptr;
        PdxLocalReader::resettoPdxHead();
        m_dataInput->advanceCursor(position);
        intArrptr = PdxLocalReader::readIntArray(fieldName, length);
        PdxLocalReader::resettoPdxHead();
        return intArrptr;
      } else {
        return nullptr;
      }
    }
  }
}

int64_t* PdxRemoteReader::readLongArray(const char* fieldName,
                                        int32_t& length) {
  int choice = m_localToRemoteMap[m_currentIndex++];

  switch (choice) {
    case -2: {
      return PdxLocalReader::readLongArray(fieldName, length);  // in same order
    }
    case -1: {
      return nullptr;
    }
    default: {
      // sequence id read field and then update
      int position = m_pdxType->getFieldPosition(
          choice, m_offsetsBuffer, m_offsetSize, m_serializedLength);
      if (position != -1) {
        int64_t* longArrptr;
        PdxLocalReader::resettoPdxHead();
        m_dataInput->advanceCursor(position);
        longArrptr = PdxLocalReader::readLongArray(fieldName, length);
        PdxLocalReader::resettoPdxHead();
        return longArrptr;
      } else {
        return nullptr;
      }
    }
  }
}

float* PdxRemoteReader::readFloatArray(const char* fieldName, int32_t& length) {
  int choice = m_localToRemoteMap[m_currentIndex++];

  switch (choice) {
    case -2: {
      return PdxLocalReader::readFloatArray(fieldName, length);  // in same
                                                                 // order
    }
    case -1: {
      return nullptr;
    }
    default: {
      // sequence id read field and then update
      int position = m_pdxType->getFieldPosition(
          choice, m_offsetsBuffer, m_offsetSize, m_serializedLength);
      if (position != -1) {
        float* floatArrptr;
        PdxLocalReader::resettoPdxHead();
        m_dataInput->advanceCursor(position);
        floatArrptr =
            PdxLocalReader::readFloatArray(fieldName, length);  // in same order
        PdxLocalReader::resettoPdxHead();
        return floatArrptr;
      } else {
        return nullptr;
      }
    }
  }
}

double* PdxRemoteReader::readDoubleArray(const char* fieldName,
                                         int32_t& length) {
  int choice = m_localToRemoteMap[m_currentIndex++];

  switch (choice) {
    case -2: {
      return PdxLocalReader::readDoubleArray(fieldName,
                                             length);  // in same order
    }
    case -1: {
      return nullptr;
    }
    default: {
      // sequence id read field and then update
      int position = m_pdxType->getFieldPosition(
          choice, m_offsetsBuffer, m_offsetSize, m_serializedLength);
      if (position != -1) {
        double* doubleArrptr;
        PdxLocalReader::resettoPdxHead();
        m_dataInput->advanceCursor(position);
        doubleArrptr = PdxLocalReader::readDoubleArray(
            fieldName, length);  // in same order
        PdxLocalReader::resettoPdxHead();
        return doubleArrptr;
      } else {
        return nullptr;
      }
    }
  }
}

char** PdxRemoteReader::readStringArray(const char* fieldName,
                                        int32_t& length) {
  int choice = m_localToRemoteMap[m_currentIndex++];

  switch (choice) {
    case -2: {
      return PdxLocalReader::readStringArray(fieldName,
                                             length);  // in same order
    }
    case -1: {
      return nullptr;
    }
    default: {
      // sequence id read field and then update
      int position = m_pdxType->getFieldPosition(
          choice, m_offsetsBuffer, m_offsetSize, m_serializedLength);
      if (position != -1) {
        char** strArray;
        PdxLocalReader::resettoPdxHead();
        m_dataInput->advanceCursor(position);
        strArray = PdxLocalReader::readStringArray(fieldName, length);
        PdxLocalReader::resettoPdxHead();
        return strArray;
      } else {
        return nullptr;
      }
    }
  }
}

wchar_t** PdxRemoteReader::readWideStringArray(const char* fieldName,
                                               int32_t& length) {
  int choice = m_localToRemoteMap[m_currentIndex++];

  switch (choice) {
    case -2: {
      return PdxLocalReader::readWideStringArray(fieldName,
                                                 length);  // in same order
    }
    case -1: {
      return nullptr;
    }
    default: {
      // sequence id read field and then update
      int position = m_pdxType->getFieldPosition(
          choice, m_offsetsBuffer, m_offsetSize, m_serializedLength);
      if (position != -1) {
        wchar_t** strArray;
        PdxLocalReader::resettoPdxHead();
        m_dataInput->advanceCursor(position);
        strArray = PdxLocalReader::readWideStringArray(fieldName, length);
        PdxLocalReader::resettoPdxHead();
        return strArray;
      } else {
        return nullptr;
      }
    }
  }
}
std::shared_ptr<CacheableObjectArray> PdxRemoteReader::readObjectArray(
    const char* fieldName) {
  int choice = m_localToRemoteMap[m_currentIndex++];

  switch (choice) {
    case -2:
      return PdxLocalReader::readObjectArray(fieldName);  // in same order
    case -1: {
      return nullptr;  // null value
    }
    default: {
      // sequence id read field and then update
      int position = m_pdxType->getFieldPosition(
          choice, m_offsetsBuffer, m_offsetSize, m_serializedLength);
      PdxLocalReader::resettoPdxHead();
      m_dataInput->advanceCursor(position);
      auto retVal = PdxLocalReader::readObjectArray(fieldName);
      PdxLocalReader::resettoPdxHead();
      return retVal;
    }
  }
}

int8_t** PdxRemoteReader::readArrayOfByteArrays(const char* fieldName,
                                                int32_t& arrayLength,
                                                int32_t** elementLength) {
  int choice = m_localToRemoteMap[m_currentIndex++];

  switch (choice) {
    case -2:
      return PdxLocalReader::readArrayOfByteArrays(
          fieldName, arrayLength, elementLength);  // in same order
    case -1:
      return nullptr;  // null value
    default: {
      // sequence id read field and then update
      int position = m_pdxType->getFieldPosition(
          choice, m_offsetsBuffer, m_offsetSize, m_serializedLength);
      PdxLocalReader::resettoPdxHead();
      m_dataInput->advanceCursor(position);
      int8_t** retVal = PdxLocalReader::readArrayOfByteArrays(
          fieldName, arrayLength, elementLength);
      PdxLocalReader::resettoPdxHead();
      return retVal;
    }
  }
}
 std::shared_ptr<CacheableDate> PdxRemoteReader::readDate(const char* fieldName) {
   int choice = m_localToRemoteMap[m_currentIndex++];

   switch (choice) {
     case -2:
       return PdxLocalReader::readDate(fieldName);  // in same order
     case -1:
       return nullptr;
     default: {
       // sequence id read field and then update
       int position = m_pdxType->getFieldPosition(
           choice, m_offsetsBuffer, m_offsetSize, m_serializedLength);
       PdxLocalReader::resettoPdxHead();
       m_dataInput->advanceCursor(position);
       auto retVal = PdxLocalReader::readDate(fieldName);
       PdxLocalReader::resettoPdxHead();
       return retVal;
     }
  }
}

void PdxRemoteReader::readCollection(const char* fieldName,
                                     std::shared_ptr<CacheableArrayList>& collection) {
  int choice = m_localToRemoteMap[m_currentIndex++];

  switch (choice) {
    case -2: {
      PdxLocalReader::readCollection(fieldName, collection);  // in same order
      break;
    }
    case -1: {
      collection = nullptr;
      break;  // null value
    }
    default: {
      // sequence id read field and then update
      int position = m_pdxType->getFieldPosition(
          choice, m_offsetsBuffer, m_offsetSize, m_serializedLength);
      if (position != -1) {
        PdxLocalReader::resettoPdxHead();
        m_dataInput->advanceCursor(position);
        PdxLocalReader::readCollection(fieldName, collection);
        PdxLocalReader::resettoPdxHead();
      } else {
        collection = nullptr;
      }
      break;
    }
  }
}
}  // namespace client
}  // namespace geode
}  // namespace apache
