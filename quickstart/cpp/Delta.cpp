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

// Include the Geode library.

/*
 * The Delta QuickStart Example.
 *
 * This example takes the following steps:
 *
 * 1. Create a Geode Cache.
 * 2. Get the example Region from the Cache.
 * 3. Put an Entry into the Region.
 * 4. Set delta for a value.
 * 5. Put entry with delta in region.
 * 6. Local-invalidate entry in region.
 * 7. Get entry from server.
 * 8. Verify that delta was applied on server, by examining entry.
 * 9. Close the Cache.
 *
 */

#include "deltaobjects/DeltaExample.hpp"

// Use the "geode" namespace.
using namespace apache::geode::client;

// The Delta QuickStart example.
int main(int argc, char** argv) {
  try {
    // Create a Geode Cache.
   auto prptr = Properties::create();
   prptr->insert("cache-xml-file", "XMLs/clientDelta.xml");

   auto cacheFactory = CacheFactory::createCacheFactory(prptr);

   auto cachePtr = cacheFactory->create();

   LOGINFO("Created the Geode Cache");

   // get the example Region.
   auto regPtr = cachePtr->getRegion("exampleRegion");

   LOGINFO("Obtained the Region from the Cache");

   // Register our Serializable/Cacheable Delta objects, DeltaExample.
   serializationRegistry->addType(DeltaExample::create);

   // Creating Delta Object.
   DeltaExample* ptr = new DeltaExample(10, 15, 20);
   std::shared_ptr<Cacheable> valPtr(ptr);
   // Put the delta object. This will send the complete object to the server.
   regPtr->put("Key1", valPtr);
   LOGINFO("Completed put for a delta object");

   // Changing state of delta object.
   ptr->setField1(9);

   // Put delta object again. Since delta flag is set true it will calculate
   // Delta and send only Delta to the server.
   regPtr->put("Key1", valPtr);
   LOGINFO("Completed put with delta");

   // Locally invalidating the key.
   regPtr->localInvalidate("Key1");

   // Fetching the value from server.
   auto retVal = std::dynamic_pointer_cast<DeltaExample>(regPtr->get("Key1"));

   // Verification
   if (retVal->getField1() != 9)
     throw Exception("First field should have been 9");
   if (retVal->getField2() != 15)
     throw Exception("Second field should have been 15");
   if (retVal->getField3() != 20)
     throw Exception("Third field should have been 20");
   LOGINFO("Delta has been successfully applied at server");

   cachePtr->close();

   LOGINFO("Closed the Geode Cache");

   return 0;
  }
  // An exception should not occur

  catch (const Exception& geodeExcp) {
    LOGERROR("Delta Geode Exception: %s", geodeExcp.getMessage());

    return 1;
  }
}
