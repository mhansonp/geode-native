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
 * The BasicOperations QuickStart Example.
 *
 * This example takes the following steps:
 *
 * 1. Create a Geode Cache.
 * 2. Create the example Region Programmatically.
 * 3.a. Put Entries (Key and Value pairs) into the Region.
 * 3.b. If in 64 bit mode put over 4 GB data to demonstrate capacity.
 * 4. Get Entries from the Region.
 * 5. Invalidate an Entry in the Region.
 * 6. Destroy an Entry in the Region.
 * 7. Close the Cache.
 *
 */

// Include the Geode library.

// Use the "geode" namespace.
using namespace apache::geode::client;

// The BasicOperations QuickStart example.
int main(int argc, char** argv) {
  try {
    // Create a Geode Cache.
    auto cacheFactory = CacheFactory::createCacheFactory();

    auto cachePtr = cacheFactory->create();

    LOGINFO("Created the Geode Cache");

    auto regionFactory = cachePtr->createRegionFactory(CACHING_PROXY);

    LOGINFO("Created the RegionFactory");

    // Create the example Region Programmatically.
    auto regionPtr = regionFactory->create("exampleRegion");

    LOGINFO("Created the Region Programmatically.");

    // Put an Entry (Key and Value pair) into the Region using the
    // direct/shortcut method.
    regionPtr->put("Key1", "Value1");
    LOGINFO("Put the first Entry into the Region");

    // Put an Entry into the Region by manually creating a Key and a Value pair.
    auto keyPtr = CacheableInt32::create(123);
    auto valuePtr = CacheableString::create("123");
    regionPtr->put(keyPtr, valuePtr);

    LOGINFO("Put the second Entry into the Region");

#if (defined(_WIN64) || defined(__sparcv9) || defined(__x86_64__))
    // Put just over 4 GB of data locally
    char* text = new char[1024 * 1024 /* 1 MB */];
    memset(text, 'A', 1024 * 1024 - 1);
    text[1024 * 1024 - 1] = '\0';

    for (int item = 0; item < (5 * 1024 /* 5 GB */); item++) {
      regionPtr->localPut(item, text);
    }

    LOGINFO("Put over 4 GB data locally");
#endif  // 64-bit

    // Get Entries back out of the Region.
    auto result1Ptr = regionPtr->get("Key1");

    LOGINFO("Obtained the first Entry from the Region");

    auto result2Ptr = regionPtr->get(keyPtr);

    LOGINFO("Obtained the second Entry from the Region");

    // Invalidate an Entry in the Region.
    regionPtr->invalidate("Key1");

    LOGINFO("Invalidated the first Entry in the Region");

    // Destroy an Entry in the Region.
    regionPtr->destroy(keyPtr);

    LOGINFO("Destroyed the second Entry in the Region");

    // Close the Geode Cache.
    cachePtr->close();

    LOGINFO("Closed the Geode Cache");

    return 0;
  }
  // An exception should not occur
  catch (const Exception& geodeExcp) {
    LOGERROR("BasicOperations Geode Exception: %s", geodeExcp.getMessage());

    return 1;
  }
}
