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
 * The Durable Client QuickStart Example.
 *
 * This example takes the following steps:
 *
 * 1. Create a Geode Cache with durable client properties Programmatically.
 * 2. Create the example generic Region programmatically.
 * 3. Set DurableCacheListener with "AfterRegionLive" implementation to region.
 * 4. Register Interest to region with durable option.
 * 5. call to readyForEvent().
 * 6. Close the Cache with keepalive options as true.
 *
 */

// Use standard namespaces
using System;

// Use the Geode namespace
using Apache.Geode.Client;

namespace Apache.Geode.Client.QuickStart
{
  // The DurableClient QuickStart example.
  class DurableClientExample
  {
    public void RunDurableClient()
    {
        // Create durable client's properties using api.
      Properties<string, string> durableProp = Properties<string, string>.Create<string, string>();
        durableProp.Insert("durable-client-id", "DurableClientId");
        durableProp.Insert("durable-timeout", "300s");

        // Create a Geode Cache programmatically.
        CacheFactory cacheFactory = CacheFactory.CreateCacheFactory(durableProp);

        Cache cache = cacheFactory.SetSubscriptionEnabled(true)
                                  .Create();

        Console.WriteLine("Created the Geode Cache");
             
        // Create the example Region programmatically.
        RegionFactory regionFactory = cache.CreateRegionFactory(RegionShortcut.CACHING_PROXY);

        IRegion<string, string> region = regionFactory.Create<string, string>("exampleRegion");

        Console.WriteLine("Created the generic Region programmatically.");            

        // Plugin the CacheListener with afterRegionLive. "afterRegionLive()"  will be called 
        // after all the queued events are recieved by client
        AttributesMutator<string, string> attrMutator = region.AttributesMutator;
        attrMutator.SetCacheListener(new DurableCacheListener<string, string>());

        Console.WriteLine("DurableCacheListener set to region.");
        
        // For durable Clients, Register Intrest can be durable or non durable ( default ), 
        // Unregister Interest APIs remain same.

        string [] keys = new string[] { "Key-1" };
        region.GetSubscriptionService().RegisterKeys(keys, true, true);

        Console.WriteLine("Called Register Interest for Key-1 with isDurable as true");

        //Send ready for Event message to Server( only for Durable Clients ). 
        //Server will send queued events to client after recieving this.
        cache.ReadyForEvents();
    	
        Console.WriteLine("Sent ReadyForEvents message to server");

        //wait for some time to recieve events
        System.Threading.Thread.Sleep(1000);

        // Close the Geode Cache with keepalive = true.  Server will queue events for
        // durable registered keys and will deliver all events when client will reconnect
        // within timeout period and send "readyForEvents()"
        cache.Close(true);

        Console.WriteLine("Closed the Geode Cache with keepalive as true");
    }

    public void RunFeeder()
    {
        // Create a Geode Cache Programmatically.
        CacheFactory cacheFactory = CacheFactory.CreateCacheFactory();
        Cache cache = cacheFactory.SetSubscriptionEnabled(true).Create();

        Console.WriteLine("Created the Geode Cache");

        RegionFactory regionFactory = cache.CreateRegionFactory(RegionShortcut.PROXY);

        // Created the Region Programmatically.
        IRegion<string, string> region = regionFactory.Create<string, string>("exampleRegion");

        Console.WriteLine("Created the Region Programmatically.");

        // create two keys with value
        string key1 = "Key-1";
        string value1 = "Value-1";
        region[key1] = value1;
        string key2 = "Key-2";
        string value2 = "Value-2";
        region[key2] = value2;

        Console.WriteLine("Created Key-1 and Key-2 in region. Durable interest was registered only for Key-1.");

        // Close the Geode Cache
        cache.Close();

        Console.WriteLine("Closed the Geode Cache");

    }
    static void Main(string[] args)
    {
      try
      {
        DurableClientExample ex = new DurableClientExample();
 
        //First Run of Durable Client
        ex.RunDurableClient();

        //Intermediate Feeder, feeding events
        ex.RunFeeder();

        //Reconnect Durable Client
        ex.RunDurableClient();

      }
      // An exception should not occur
      catch (GeodeException gfex)
      {
        Console.WriteLine("DurableClient Geode Exception: {0}", gfex.Message);
      }
    }
  }
}
