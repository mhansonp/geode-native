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
#include "fw_dunit.hpp"
#include "ThinClientHelper.hpp"
#include "testobject/VariousPdxTypes.hpp"

#include <ace/OS.h>
#include <ace/High_Res_Timer.h>
#include <ace/ACE.h>

#include <geode/PdxInstanceFactory.hpp>
#include <geode/UserFunctionExecutionException.hpp>
#include <geode/FunctionService.hpp>
#include <geode/DefaultResultCollector.hpp>
using namespace PdxTests;
/* This is to test
1- funtion execution on pool
*/

#define CLIENT1 s1p1
#define CLIENT2 s1p2
#define LOCATOR1 s2p1
#define SERVER s2p2

bool isLocalServer = false;
bool isLocator = false;
bool isPoolWithEndpoint = false;

const char* locHostPort =
    CacheHelper::getLocatorHostPort(isLocator, isLocalServer, 1);
const char* poolRegNames[] = {"partition_region", "PoolRegion2"};

const char* serverGroup = "ServerGroup1";

char* getFuncIName = (char*)"MultiGetFunctionI";
char* putFuncIName = (char*)"MultiPutFunctionI";
char* getFuncName = (char*)"MultiGetFunction";
char* getFuncName2 = (char*)"MultiGetFunction2";
char* putFuncName = (char*)"MultiPutFunction";
char* rjFuncName = (char*)"RegionOperationsFunction";
char* exFuncName = (char*)"ExceptionHandlingFunction";
char* exFuncNameSendException = (char*)"executeFunction_SendException";
char* exFuncNamePdxType = (char*)"PdxFunctionTest";
char* FETimeOut = (char*)"FunctionExecutionTimeOut";

#define verifyGetResults()                                                     \
  bool found = false;                                                          \
  for (int j = 0; j < 34; j++) {                                               \
    if (j % 2 == 0) continue;                                                  \
    sprintf(buf, "VALUE--%d", j);                                              \
    if (strcmp(buf, std::dynamic_pointer_cast<CacheableString>(                \
                        resultList->operator[](i))                             \
                        ->asChar()) == 0) {                                    \
      LOGINFO(                                                                 \
          "buf = %s "                                                          \
          "std::dynamic_pointer_cast<CacheableString>(resultList->operator[](" \
          "i))->asChar() "                                                     \
          "= %s ",                                                             \
          buf,                                                                 \
          std::dynamic_pointer_cast<CacheableString>(                          \
              resultList->operator[](i))                                       \
              ->asChar());                                                     \
      found = true;                                                            \
      break;                                                                   \
    }                                                                          \
  }                                                                            \
  ASSERT(found, "this returned value is invalid");

#define verifyGetKeyResults()                                                  \
  bool found = false;                                                          \
  for (int j = 0; j < 34; j++) {                                               \
    if (j % 2 == 0) continue;                                                  \
    sprintf(buf, "KEY--%d", j);                                                \
    if (strcmp(buf, std::dynamic_pointer_cast<CacheableString>(                \
                        resultList->operator[](i))                             \
                        ->asChar()) == 0) {                                    \
      LOGINFO(                                                                 \
          "buf = %s "                                                          \
          "std::dynamic_pointer_cast<CacheableString>(resultList->operator[](" \
          "i))->asChar() "                                                     \
          "= %s ",                                                             \
          buf,                                                                 \
          std::dynamic_pointer_cast<CacheableString>(                          \
              resultList->operator[](i))                                       \
              ->asChar());                                                     \
      found = true;                                                            \
      break;                                                                   \
    }                                                                          \
  }                                                                            \
  ASSERT(found, "this returned KEY is invalid");

#define verifyPutResults()                   \
  bool found = false;                        \
  for (int j = 0; j < 34; j++) {             \
    if (j % 2 == 0) continue;                \
    sprintf(buf, "KEY--%d", j);              \
    if (strcmp(buf, value->asChar()) == 0) { \
      found = true;                          \
      break;                                 \
    }                                        \
  }                                          \
  ASSERT(found, "this returned value is invalid");

class MyResultCollector : public DefaultResultCollector {
 public:
  MyResultCollector()
      : DefaultResultCollector(),
        m_endResultCount(0),
        m_addResultCount(0),
        m_getResultCount(0) {}
  ~MyResultCollector() {}

  std::shared_ptr<CacheableVector> getResult(
      std::chrono::milliseconds timeout) override {
    m_getResultCount++;
    return DefaultResultCollector::getResult(timeout);
  }

  void addResult(const std::shared_ptr<Cacheable>& resultItem) override {
    m_addResultCount++;
    if (resultItem == nullptr) {
      return;
    }
    if (auto results =
            std::dynamic_pointer_cast<CacheableArrayList>(resultItem)) {
      for (auto& result : *results) {
        DefaultResultCollector::addResult(result);
      }
    } else {
      DefaultResultCollector::addResult(resultItem);
    }
  }

  void endResults() override {
    m_endResultCount++;
    DefaultResultCollector::endResults();
  }

  uint32_t getEndResultCount() { return m_endResultCount; }
  uint32_t getAddResultCount() { return m_addResultCount; }
  uint32_t getGetResultCount() { return m_getResultCount; }

 private:
  uint32_t m_endResultCount;
  uint32_t m_addResultCount;
  uint32_t m_getResultCount;
};

template <class _T>
bool validateResultTypeAndAllowUserFunctionExecutionException(
    const int32_t index, const std::shared_ptr<Cacheable>& result) {
  if (auto intValue = std::dynamic_pointer_cast<_T>(result)) {
    LOGINFO("%s is %d ", typeid(_T).name(), intValue->value());
    return true;
  } else {
    if (auto uFEPtr =
            std::dynamic_pointer_cast<UserFunctionExecutionException>(result)) {
      LOGINFO("Done casting to uFEPtr");
      LOGINFO("Read expected uFEPtr exception %s ",
              uFEPtr->getMessage()->asChar());
      return true;
    } else {
      LOGINFO("Unexpected result type %s for index %d.",
              result->toString()->asChar(), index);
    }
  }

  return false;
}

bool validateResultTypeIsUserFunctionExecutionException(
    const int32_t index, const std::shared_ptr<Cacheable>& result) {
  if (auto uFEPtr =
          std::dynamic_pointer_cast<UserFunctionExecutionException>(result)) {
    LOGINFO("Done casting to uFEPtr");
    LOGINFO("Read expected uFEPtr exception %s ",
            uFEPtr->getMessage()->asChar());
    return true;
  } else {
    LOGINFO("Unexpected result type %s for index %d.",
            result->toString()->asChar(), index);
  }

  return false;
}

template <class _T>
bool validateResultType(const int32_t index, const std::shared_ptr<Cacheable>& result) {
  return false;
}

DUNIT_TASK_DEFINITION(LOCATOR1, StartLocator1)
  {
    // starting locator
    if (isLocator) {
      CacheHelper::initLocator(1);
      LOG("Locator1 started");
    }
  }
END_TASK_DEFINITION

DUNIT_TASK_DEFINITION(SERVER, StartS12)
  {
    const char* lhp = nullptr;
    if (!isPoolWithEndpoint) lhp = locHostPort;
    if (isLocalServer) {
      CacheHelper::initServer(1, "func_cacheserver1_pool.xml", lhp);
    }
    if (isLocalServer) {
      CacheHelper::initServer(2, "func_cacheserver2_pool.xml", lhp);
    }
  }
END_TASK_DEFINITION

DUNIT_TASK_DEFINITION(SERVER, startServer2)
  {
    const char* lhp = nullptr;
    if (!isPoolWithEndpoint) lhp = locHostPort;
    if (isLocalServer) {
      CacheHelper::initServer(2, "func_cacheserver2_pool.xml", lhp);
    }
  }
END_TASK_DEFINITION

DUNIT_TASK_DEFINITION(CLIENT1, StartC1)
  {
    initClientWithPool(true, poolRegNames[0], locHostPort, serverGroup, nullptr,
                       0, true);

    auto regPtr0 = createRegionAndAttachPool(poolRegNames[0], USE_ACK, nullptr);
    regPtr0->registerAllKeys();

    LOG("Clnt1Init complete.");
  }
END_TASK_DEFINITION

DUNIT_TASK_DEFINITION(CLIENT1, Client1OpTest)
  {
    auto regPtr0 = getHelper()->getRegion(poolRegNames[0]);
    char buf[128];

    for (int i = 0; i < 34; i++) {
      sprintf(buf, "VALUE--%d", i);
      auto value = CacheableString::create(buf);

      sprintf(buf, "KEY--%d", i);
      auto key = CacheableKey::create(buf);
      regPtr0->put(key, value);
    }
    SLEEP(10000);  // let the put finish
    try {
      std::shared_ptr<Cacheable> args = CacheableBoolean::create(1);
      auto routingObj = CacheableVector::create();
      for (int i = 0; i < 34; i++) {
        if (i % 2 == 0) continue;
        sprintf(buf, "KEY--%d", i);
        auto key = CacheableKey::create(buf);
        routingObj->push_back(key);
      }

      // test data dependant function
      //     test get function with result
      auto exc = FunctionService::onRegion(regPtr0);
      ASSERT(exc != nullptr, "onRegion Returned nullptr");
      args = CacheableKey::create("echoString");
      std::shared_ptr<Cacheable> args1 = CacheableKey::create("echoBoolean");
      auto exe1 = exc->withArgs(args);
      auto exe2 = exe1->withArgs(args1);

      auto resultList = CacheableVector::create();
      auto executeFunctionResult =
          exe1->execute(rjFuncName, std::chrono::seconds(15))->getResult();
      if (executeFunctionResult == nullptr) {
        ASSERT(false, "echo String : executeFunctionResult is nullptr");
      } else {
        sprintf(buf, "echo String : result count = %zd",
                executeFunctionResult->size());
        LOG(buf);
        if (auto csp = std::dynamic_pointer_cast<CacheableString>(
                executeFunctionResult->operator[](0))) {
          sprintf(buf, "echo String : cast successful, echoed string= %s",
                  csp->asChar());
          LOG(buf);
        } else {
          FAIL("echo String : wrong argument type");
        }
      }
      executeFunctionResult =
          exc->withFilter(routingObj)
              ->withArgs(args)
              ->execute(exFuncName, std::chrono::seconds(15))
              ->getResult();

      executeFunctionResult =
          exc->withFilter(routingObj)
              ->withArgs(args)
              ->execute(rjFuncName, std::chrono::seconds(15))
              ->getResult();
      if (executeFunctionResult == nullptr) {
        ASSERT(false, "echo String : executeFunctionResult is nullptr");
      } else {
        sprintf(buf, "echo String : result count = %zd",
                executeFunctionResult->size());
        LOG(buf);
        const char* str = std::dynamic_pointer_cast<CacheableString>(
                              executeFunctionResult->operator[](0))
                              ->asChar();
        LOG(str);
        ASSERT(strcmp("echoString", str) == 0, "echoString is not eched back");
      }
      args = CacheableKey::create("echoBoolean");
      executeFunctionResult =
          exc->withFilter(routingObj)
              ->withArgs(args)
              ->execute(rjFuncName, std::chrono::seconds(15))
              ->getResult();
      if (executeFunctionResult == nullptr) {
        ASSERT(false, "echo Boolean: executeFunctionResult is nullptr");
      } else {
        sprintf(buf, "echo Boolean: result count = %zd",
                executeFunctionResult->size());
        LOG(buf);
        bool b = std::dynamic_pointer_cast<CacheableBoolean>(
                     executeFunctionResult->operator[](0))
                     ->value();
        LOG(b == true ? "true" : "false");
        ASSERT(b == true, "true is not eched back");
      }

      executeFunctionResult = exc->withFilter(routingObj)
                                  ->withArgs(args)
                                  ->execute(getFuncName)
                                  ->getResult();
      if (executeFunctionResult == nullptr) {
        ASSERT(false, "region get: executeFunctionResult is nullptr");
      } else {
        for (unsigned item = 0;
             item < static_cast<uint32_t>(executeFunctionResult->size());
             item++) {
          auto arrayList = std::dynamic_pointer_cast<CacheableArrayList>(
              executeFunctionResult->operator[](item));
          for (unsigned pos = 0; pos < static_cast<uint32_t>(arrayList->size());
               pos++) {
            resultList->push_back(arrayList->operator[](pos));
          }
        }
        sprintf(buf, "Region get: result count = %zd", resultList->size());
        LOG(buf);
        ASSERT(resultList->size() == 34,
               "region get: resultList count is not 34");
        for (int32_t i = 0; i < resultList->size(); i++) {
          auto csPtr = std::dynamic_pointer_cast<CacheableString>(
              resultList->operator[](i));
          //  printf(" in csPtr = %u \n",csPtr);
          if (csPtr != nullptr) {
            sprintf(buf, "Region get: result[%d]=%s", i,
                    std::dynamic_pointer_cast<CacheableString>(
                        resultList->operator[](i))
                        ->asChar());
            LOG(buf);
            verifyGetResults()
          } else {
            std::shared_ptr<Cacheable> tmp = resultList->operator[](i);
            if (tmp != nullptr) {
              printf(" in typeid = %d  \n", tmp->typeId());

            } else {
              printf(" in typeid is null \n");
            }
          }
        }
      }
      //     test get function with customer collector
      auto myRC = std::make_shared<MyResultCollector>();
      executeFunctionResult = exc->withFilter(routingObj)
                                  ->withArgs(args)
                                  ->withCollector(myRC)
                                  ->execute(getFuncName)
                                  ->getResult();
      sprintf(buf, "add result count = %d", myRC->getAddResultCount());
      LOG(buf);
      sprintf(buf, "end result count = %d", myRC->getEndResultCount());
      LOG(buf);
      sprintf(buf, "get result count = %d", myRC->getGetResultCount());
      LOG(buf);
      if (executeFunctionResult == nullptr) {
        ASSERT(false,
               "region get new collector: executeFunctionResult is nullptr");
      } else {
        resultList->clear();
        for (unsigned item = 0;
             item < static_cast<uint32_t>(executeFunctionResult->size());
             item++) {
          resultList->push_back(executeFunctionResult->operator[](item));
        }
        sprintf(buf, "Region get new collector: result list count = %zd",
                resultList->size());
        LOG(buf);
        sprintf(buf, "Region get new collector: result count = %zd",
                executeFunctionResult->size());
        LOG(buf);
        ASSERT(
            executeFunctionResult->size() == resultList->size(),
            "region get new collector: executeFunctionResult count is not 34");
        for (int32_t i = 0; i < executeFunctionResult->size(); i++) {
          sprintf(buf, "Region get new collector: result[%d]=%s", i,
                  std::dynamic_pointer_cast<CacheableString>(
                      resultList->operator[](i))
                      ->asChar());
          LOG(buf);
          verifyGetResults()
        }
      }
      //     test put function without result
      exc->withFilter(routingObj)
          ->withArgs(args)
          ->execute(putFuncName, std::chrono::seconds(15));
      SLEEP(10000);  // let the put finish
      for (int i = 0; i < 34; i++) {
        if (i % 2 == 0) continue;
        sprintf(buf, "KEY--%d", i);
       auto key = CacheableKey::create(buf);
        auto value =
            std::dynamic_pointer_cast<CacheableString>(regPtr0->get(key));
        sprintf(buf, "Region put: result[%d]=%s", i, value->asChar());
        LOG(buf);
        verifyPutResults()
      }

      args = routingObj;
      executeFunctionResult =
          exc->withArgs(args)
              ->withFilter(routingObj)
              ->execute(getFuncName, std::chrono::seconds(15))
              ->getResult();

      if (executeFunctionResult == nullptr) {
        ASSERT(false, "get executeFunctionResult is nullptr");
      } else {
        sprintf(buf, "echo String : result count = %zd",
                executeFunctionResult->size());
        LOGINFO(buf);
        resultList->clear();

        for (unsigned item = 0;
             item < static_cast<uint32_t>(executeFunctionResult->size());
             item++) {
          auto arrayList = std::dynamic_pointer_cast<CacheableArrayList>(
              executeFunctionResult->operator[](item));
          for (unsigned pos = 0; pos < static_cast<uint32_t>(arrayList->size());
               pos++) {
            resultList->push_back(arrayList->operator[](pos));
          }
        }
        sprintf(buf, "get result count = %zd", resultList->size());
        LOGINFO(buf);
        ASSERT(resultList->size() == 34,
               "get executeFunctionResult count is not 34");
        for (int32_t i = 0; i < resultList->size(); i++) {
          sprintf(buf, "result[%d] is null\n", i);
          ASSERT(resultList->operator[](i) != nullptr, buf);
          sprintf(buf, "get result[%d]=%s", i,
                  std::dynamic_pointer_cast<CacheableString>(
                      resultList->operator[](i))
                      ->asChar());
          LOGINFO(buf);
          verifyGetKeyResults()
        }
      }

      //-----------------------Test with sendException
      // onRegion-------------------------------//
      for (int i = 1; i <= 200; i++) {
        auto value = CacheableInt32::create(i);

        sprintf(buf, "execKey-%d", i);
        auto key = CacheableKey::create(buf);
        regPtr0->put(key, value);
      }
      LOG("Put for execKey's on region complete.");

      LOG("Adding filter");
      auto arrList = CacheableArrayList::create();
      for (int i = 100; i < 120; i++) {
        sprintf(buf, "execKey-%d", i);
        auto key = CacheableKey::create(buf);
        arrList->push_back(key);
      }

      auto filter = CacheableVector::create();
      for (int i = 100; i < 120; i++) {
        sprintf(buf, "execKey-%d", i);
        auto key = CacheableKey::create(buf);
        filter->push_back(key);
      }
      LOG("Adding filter done.");

      args = CacheableBoolean::create(1);

      auto funcExec = FunctionService::onRegion(regPtr0);
      ASSERT(funcExec != nullptr, "onRegion Returned nullptr");

      auto collector = funcExec->withArgs(args)->withFilter(filter)->execute(
          exFuncNameSendException, std::chrono::seconds(15));
      ASSERT(collector != nullptr, "onRegion collector nullptr");

      auto result = collector->getResult();

      if (result == nullptr) {
        ASSERT(false, "echo String : result is nullptr");
      } else {
        for (int i = 0; i < result->size(); i++) {
          ASSERT(validateResultTypeIsUserFunctionExecutionException(
                     i, result->operator[](i)),
                 "exFuncNameSendException casting to string for bool "
                 "arguement exception.");
        }
      }
      LOG("exFuncNameSendException done for bool arguement.");

      collector = funcExec->withArgs(arrList)->withFilter(filter)->execute(
          exFuncNameSendException, std::chrono::seconds(15));
      ASSERT(collector != nullptr, "onRegion collector for arrList nullptr");

      result = collector->getResult();
      LOGINFO("result->size() = %d & arrList->size()  = %d ", result->size(),
              arrList->size() + 1);
      ASSERT(
          result->size() == arrList->size() + 1,
          "region get: resultList count is not as arrayList count + exception");

      for (int i = 0; i < result->size(); i++) {
        ASSERT(validateResultTypeAndAllowUserFunctionExecutionException<
                   CacheableInt32>(i, result->operator[](i)),
               "Unexpected result type.");
      }

      LOG("exFuncNameSendException done for arrayList arguement.");

      LOG("exFuncNameSendException for string arguement.");

      args = CacheableString::create("Multiple");
      collector = funcExec->withArgs(args)->withFilter(filter)->execute(
          exFuncNameSendException, std::chrono::seconds(15));
      ASSERT(collector != nullptr, "onRegion collector for string nullptr");
      result = collector->getResult();
      LOGINFO("result->size() for Multiple String = %d ", result->size());
      ASSERT(result->size() == 1,
             "region get: resultList count is not as string exception");

      for (int i = 0; i < result->size(); i++) {
        LOG("exFuncNameSendException now casting to "
            "UserFunctionExecutionExceptionPtr for arrayList arguement "
            "exception.");
        ASSERT(validateResultTypeIsUserFunctionExecutionException(
                   i, result->operator[](i)),
               "exFuncNameSendException casting to string for string "
               "arguement exception.");
      }

      LOG("exFuncNameSendException for string arguement done.");

      LOGINFO(
          "test exFuncNameSendException function with customer collector with "
          "bool as arguement using onRegion.");

      auto myRC1 = std::make_shared<MyResultCollector>();
      result = funcExec->withArgs(args)
                   ->withFilter(filter)
                   ->withCollector(myRC1)
                   ->execute(exFuncNameSendException)
                   ->getResult();
      LOGINFO("add result count = %d", myRC1->getAddResultCount());
      LOGINFO("end result count = %d", myRC1->getEndResultCount());
      LOGINFO("get result count = %d", myRC1->getGetResultCount());
      ASSERT(1 == myRC1->getAddResultCount(), "add result count is not 1");
      ASSERT(1 == myRC1->getEndResultCount(), "end result count is not 1");
      ASSERT(1 == myRC1->getGetResultCount(), "get result count is not 1");
      if (result == nullptr) {
        ASSERT(false, "region get new collector: result is nullptr");
      } else {
        LOGINFO("Region get new collector: result count = %d", result->size());
        ASSERT(1 == result->size(),
               "region get new collector: result count is not 1");
      }

      LOGINFO(
          "test exFuncNameSendException function with customer collector with "
          "bool as arguement done using onRegion.");

      //-----------------------Test with sendException onRegion
      // done-------------------------------//

      // restore the data set
      for (int i = 0; i < 34; i++) {
        sprintf(buf, "VALUE--%d", i);
        auto value = CacheableString::create(buf);

        sprintf(buf, "KEY--%d", i);
        auto key = CacheableKey::create(buf);
        regPtr0->put(key, value);
      }

      // test data independant function
      //     test get function with result
      args = routingObj;

      // test data independant function on one server
      LOG("test data independant get function on one server");
      exc = FunctionService::onServer(getHelper()->cachePtr);
      executeFunctionResult =
          exc->withArgs(args)->execute(getFuncIName)->getResult();
      if (executeFunctionResult == nullptr) {
        ASSERT(false, "get executeFunctionResult is nullptr");
      } else {
        resultList->clear();
        for (unsigned item = 0;
             item < static_cast<uint32_t>(executeFunctionResult->size());
             item++) {
          auto arrayList = std::dynamic_pointer_cast<CacheableArrayList>(
              executeFunctionResult->operator[](item));
          for (unsigned pos = 0; pos < static_cast<uint32_t>(arrayList->size());
               pos++) {
            resultList->push_back(arrayList->operator[](pos));
          }
        }
        sprintf(buf, "get result count = %zd", resultList->size());
        LOG(buf);
        ASSERT(resultList->size() == 17,
               "get executeFunctionResult count is not 17");
        for (int32_t i = 0; i < resultList->size(); i++) {
          sprintf(buf, "result[%d] is null\n", i);
          ASSERT(resultList->operator[](i) != nullptr, buf);
          sprintf(buf, "get result[%d]=%s", i,
                  std::dynamic_pointer_cast<CacheableString>(
                      resultList->operator[](i))
                      ->asChar());
          LOG(buf);
          verifyGetResults()
        }
      }
      LOG("test data independant put function on one server");
      //     test put function without result
      exc->withArgs(args)->execute(putFuncIName, std::chrono::seconds(15));
      SLEEP(500);  // let the put finish
      for (int i = 0; i < 34; i++) {
        if (i % 2 == 0) continue;
        sprintf(buf, "KEY--%d", i);
        auto key = CacheableKey::create(buf);
        auto value =
            std::dynamic_pointer_cast<CacheableString>(regPtr0->get(key));
        sprintf(buf, "put: result[%d]=%s", i, value->asChar());
        LOG(buf);
        verifyPutResults()
      }
      LOG("test data independant get function on all servers");
      // test data independant function on all servers
      // test get

      //-----------------------Test with PdxObject
      // onServers-------------------------------//

      try {
       auto serializationRegistry =
            CacheRegionHelper::getCacheImpl(cacheHelper->getCache().get())
                ->getSerializationRegistry();
        serializationRegistry->addPdxType(
            PdxTests::PdxTypes8::createDeserializable);
      } catch (const IllegalStateException&) {
        // ignore exception
      }

      auto pdxobj = std::make_shared<PdxTests::PdxTypes8>();
      for (int i = 0; i < 34; i++) {
        sprintf(buf, "KEY--pdx%d", i);
        auto key = CacheableKey::create(buf);
        regPtr0->put(key, pdxobj);
      }
      LOGINFO("put on pdxObject done");

      auto pdxRoutingObj = CacheableVector::create();
      for (int i = 0; i < 34; i++) {
        if (i % 2 == 0) continue;
        sprintf(buf, "KEY--pdx%d", i);
        auto key = CacheableKey::create(buf);
        pdxRoutingObj->push_back(key);
      }

      auto pdxExc = FunctionService::onServers(getHelper()->cachePtr);
      auto executeFunctionResultPdx = pdxExc->withArgs(pdxRoutingObj)
                                          ->execute(exFuncNamePdxType)
                                          ->getResult();
      LOGINFO("FE on pdxObject done");
      if (executeFunctionResultPdx == nullptr) {
        ASSERT(false, "get executeFunctionResultPdx is nullptr");
      } else {
        LOGINFO("executeFunctionResultPdx size for PdxObject = %d ",
                executeFunctionResultPdx->size());
        ASSERT(2 == executeFunctionResultPdx->size(),
               "executeFunctionResultPdx->size() is not 2");
        auto resultListPdx = CacheableVector::create();
        for (unsigned item = 0;
             item < static_cast<uint32_t>(executeFunctionResultPdx->size());
             item++) {
          auto arrayList = std::dynamic_pointer_cast<CacheableArrayList>(
              executeFunctionResultPdx->operator[](item));
          for (unsigned pos = 0; pos < static_cast<uint32_t>(arrayList->size());
               pos++) {
            resultListPdx->push_back(arrayList->operator[](pos));
          }
        }
        LOGINFO("resultlistPdx size: %d", resultListPdx->size());
        for (int32_t i = 0; i < resultListPdx->size(); i++) {
          sprintf(buf, "result[%d] is null\n", i);
          ASSERT(resultListPdx->operator[](i) != nullptr, buf);
          LOG("resultPdx item is not null");
          LOGINFO(
              "get result[%d]=%s", i,
              std::dynamic_pointer_cast<PdxTypes8>(resultListPdx->operator[](i))
                  ->toString()
                  ->asChar());
          auto pdxObj2 = std::dynamic_pointer_cast<PdxTypes8>(
              resultListPdx->operator[](i));
          ASSERT(pdxobj->equals(pdxObj2) == true,
                 "Pdx Object not equals original object.");
        }
      }
      LOG("test pdx data function on all servers done");

      auto pifPtr = cacheHelper->getCache()->createPdxInstanceFactory(
          "PdxTests.PdxTypes8");
      LOG("PdxInstanceFactoryPtr created....");

      pifPtr->writeInt("i1", 1);
      pifPtr->writeInt("i2", 2);
      pifPtr->writeString("s1", "m_s1");
      pifPtr->writeString("s2", "m_s2");
      pifPtr->writeInt("i3", 3);
      pifPtr->writeInt("i4", 4);

      auto ret = pifPtr->create();
      LOG("PdxInstancePtr created");

      for (int i = 0; i < 34; i++) {
        sprintf(buf, "KEY--pdx%d", i);
       auto key = CacheableKey::create(buf);
        regPtr0->put(key, pdxobj);
      }
      LOGINFO("put on pdxObject done");

      auto pdxInstanceRoutingObj = CacheableVector::create();
      for (int i = 0; i < 34; i++) {
        if (i % 2 == 0) continue;
        sprintf(buf, "KEY--pdx%d", i);
        auto key = CacheableKey::create(buf);
        pdxInstanceRoutingObj->push_back(key);
      }

      auto pdxInstanceExc = FunctionService::onServers(getHelper()->cachePtr);
      auto executeFunctionResultPdxInstance =
          pdxInstanceExc->withArgs(pdxInstanceRoutingObj)
              ->execute(exFuncNamePdxType)
              ->getResult();
      LOGINFO("FE on pdxObject done");
      if (executeFunctionResultPdxInstance == nullptr) {
        ASSERT(false, "get executeFunctionResultPdxInstance is nullptr");
      } else {
        LOGINFO("executeFunctionResultPdxInstance size for PdxObject = %d ",
                executeFunctionResultPdxInstance->size());
        ASSERT(2 == executeFunctionResultPdx->size(),
               "executeFunctionResultPdxInstance->size() is not 2");
        auto resultListPdxInstance = CacheableVector::create();
        for (unsigned item = 0;
             item <
             static_cast<uint32_t>(executeFunctionResultPdxInstance->size());
             item++) {
          auto arrayList = std::dynamic_pointer_cast<CacheableArrayList>(
              executeFunctionResultPdxInstance->operator[](item));
          for (unsigned pos = 0; pos < static_cast<uint32_t>(arrayList->size());
               pos++) {
            resultListPdxInstance->push_back(arrayList->operator[](pos));
          }
        }
        LOGINFO("resultlistPdxInstance size: %d",
                resultListPdxInstance->size());
        for (int32_t i = 0; i < resultListPdxInstance->size(); i++) {
          sprintf(buf, "result[%d] is null\n", i);
          ASSERT(resultListPdxInstance->operator[](i) != nullptr, buf);
          LOG("resultPdx item is not null");
          LOGINFO("get result[%d]=%s", i,
                  std::dynamic_pointer_cast<PdxTypes8>(
                      resultListPdxInstance->operator[](i))
                      ->toString()
                      ->asChar());
          auto pdxObj2 = std::dynamic_pointer_cast<PdxTypes8>(
              resultListPdxInstance->operator[](i));

          ASSERT(pdxobj->equals(pdxObj2) == true,
                 "Pdx Object not equals original object.");
        }
      }
      LOG("test pdx data function on all servers done");

      //-----------------------Test with PdxObject onServers
      // done.-------------------------------//

      // repopulate with the original values
      for (int i = 0; i < 34; i++) {
        sprintf(buf, "VALUE--%d", i);
        auto value = CacheableString::create(buf);

        sprintf(buf, "KEY--%d", i);
        auto key = CacheableKey::create(buf);
        regPtr0->put(key, value);
      }
      SLEEP(10000);  // let the put finish
      //---------------------------------------------------------------------
      // adding one more server, this should be return by locator and then
      // function should be executed on all the 3 servers

      const char* lhp = nullptr;
      if (!isPoolWithEndpoint) lhp = locHostPort;

      if (isLocalServer) {
        CacheHelper::initServer(3, "func_cacheserver3_pool.xml", lhp);
      }
      LOGINFO("Sleeping for 60s...");
      SLEEP(60000);  // let this servers gets all the data

      //---------------------------------------------------------------------
      exc = FunctionService::onServers(getHelper()->cachePtr);
      executeFunctionResult =
          exc->withArgs(args)->execute(getFuncIName)->getResult();
      if (executeFunctionResult == nullptr) {
        ASSERT(false, "get executeFunctionResult is nullptr");
      } else {
        resultList->clear();
        for (unsigned item = 0;
             item < static_cast<uint32_t>(executeFunctionResult->size());
             item++) {
          auto arrayList = std::dynamic_pointer_cast<CacheableArrayList>(
              executeFunctionResult->operator[](item));
          for (unsigned pos = 0; pos < static_cast<uint32_t>(arrayList->size());
               pos++) {
            resultList->push_back(arrayList->operator[](pos));
          }
        }
        sprintf(buf, "get result count = %zd", resultList->size());
        LOG(buf);
        printf("resultlist size: %zd", resultList->size());
        ASSERT(resultList->size() == 51,
               "get executeFunctionResult on all servers count is not 51");
        for (int32_t i = 0; i < resultList->size(); i++) {
          sprintf(buf, "result[%d] is null\n", i);
          ASSERT(resultList->operator[](i) != nullptr, buf);
          sprintf(buf, "get result[%d]=%s", i,
                  std::dynamic_pointer_cast<CacheableString>(
                      resultList->operator[](i))
                      ->asChar());
          LOG(buf);
          verifyGetResults()
        }
      }
      LOG("test data independant put function on all servers");
      // test data independant function on all servers
      // test put
      exc->withArgs(args)->execute(putFuncIName, std::chrono::seconds(15));
      SLEEP(10000);  // let the put finish
      for (int i = 0; i < 34; i++) {
        if (i % 2 == 0) continue;
        sprintf(buf, "KEY--%d", i);
        auto key = CacheableKey::create(buf);
        auto value =
            std::dynamic_pointer_cast<CacheableString>(regPtr0->get(key));
        sprintf(buf, "put: result[%d]=%s", i, value->asChar());
        LOG(buf);
        verifyPutResults()
      }
      //-----------------------Test with sendException
      // onServers-------------------------------//
      LOG("OnServers with sendException");
      args = CacheableBoolean::create(1);
      funcExec = FunctionService::onServers(getHelper()->cachePtr);

      collector = funcExec->withArgs(args)->execute(exFuncNameSendException,
                                                    std::chrono::seconds(15));
      ASSERT(collector != nullptr, "onServers collector for bool nullptr");

      result = collector->getResult();
      ASSERT(result->size() == 3,
             "Should have got 3 exception strings for sendException.");
      if (result == nullptr) {
        ASSERT(false, "echo String : result is nullptr");
      } else {
        for (int i = 0; i < result->size(); i++) {
          ASSERT(validateResultTypeIsUserFunctionExecutionException(
                     i, result->operator[](i)),
                 "exFuncNameSendException casting to string for bool arguement "
                 "Unknown exception.");
        }
      }

      collector = funcExec->withArgs(arrList)->execute(
          exFuncNameSendException, std::chrono::seconds(15));
      ASSERT(collector != nullptr, "onServers collector for arrList nullptr");

      result = collector->getResult();
      ASSERT(result->size() == (arrList->size() + 1) * 3,
             "onServers get: resultList count is not as arrayList count + "
             "exception");

      LOG("Printing only string exception results for onServers with "
          "sendException");
      for (int i = 0; i < result->size(); i++) {
        ASSERT(validateResultTypeAndAllowUserFunctionExecutionException<
                   CacheableInt32>(i, result->operator[](i)),
               "Unexpected result type.");
      }

      LOG("Printing all results for onServers with sendException");
      for (int i = 0; i < result->size(); i++) {
        ASSERT(validateResultTypeAndAllowUserFunctionExecutionException<
                   CacheableInt32>(i, result->operator[](i)),
               "Unexpected result type.");
      }

      LOG("exFuncNameSendException for string arguement.");

      args = CacheableString::create("Multiple");
      collector = funcExec->withArgs(args)->execute(exFuncNameSendException,
                                                    std::chrono::seconds(15));
      ASSERT(collector != nullptr, "onServers collector for string nullptr");

      result = collector->getResult();
      ASSERT(result->size() == 3,
             "region get: resultList count is not as string exception");

      for (int i = 0; i < result->size(); i++) {
        ASSERT(validateResultTypeIsUserFunctionExecutionException(
                   i, result->operator[](i)),
               "exFuncNameSendException casting to string for string arguement "
               "exception.");
      }

      LOG("exFuncNameSendException for string arguement done.");

      LOGINFO(
          "test exFuncNameSendException function with customer collector with "
          "bool as arguement using onServers.");

      auto myRC2 = std::make_shared<MyResultCollector>();
      result = funcExec->withArgs(args)
                   ->withCollector(myRC2)
                   ->execute(exFuncNameSendException)
                   ->getResult();
      ASSERT(3 == myRC2->getAddResultCount(), "add result count is not 3");
      ASSERT(1 == myRC2->getEndResultCount(), "end result count is not 1");
      ASSERT(1 == myRC2->getGetResultCount(), "get result count is not 1");
      LOGINFO("add result count = %d", myRC2->getAddResultCount());
      LOGINFO("end result count = %d", myRC2->getEndResultCount());
      LOGINFO("get result count = %d", myRC2->getGetResultCount());
      if (result == nullptr) {
        ASSERT(false, "region get new collector: result is nullptr");
      } else {
        LOGINFO("Region get new collector: result count = %d", result->size());
        ASSERT(3 == result->size(),
               "region get new collector: result count is not 1");
      }

      LOGINFO(
          "test exFuncNameSendException function with customer collector with "
          "bool as arguement done using onServers.");

      LOG("OnServers with sendException done");

      //-----------------------Test with sendException onServers
      // done.-------------------------------//
    } catch (const Exception& excp) {
      std::string logmsg = "";
      logmsg += excp.getName();
      logmsg += ": ";
      logmsg += excp.what();
      LOG(logmsg.c_str());
      LOG(excp.getStackTrace());
      FAIL("Function Execution Failed!");
    }
    isPoolWithEndpoint = true;
  }
END_TASK_DEFINITION

DUNIT_TASK_DEFINITION(CLIENT1, StopC1)
  {
    cleanProc();
    LOG("Clnt1Down complete: Keepalive = True");
  }
END_TASK_DEFINITION

DUNIT_TASK_DEFINITION(CLIENT1, Client2OpTest)
  {
    std::chrono::milliseconds timeout = std::chrono::seconds{15};
    auto regPtr0 = getHelper()->getRegion(poolRegNames[0]);
    char buf[128];
    SLEEP(10000);
    try {
      LOGINFO("FETimeOut begin onRegion");
      auto RexecutionPtr = FunctionService::onRegion(regPtr0);
      auto fe = RexecutionPtr->withArgs(CacheableInt32::create(timeout.count()))
                    ->execute(FETimeOut, timeout)
                    ->getResult();
      if (fe == nullptr) {
        ASSERT(false, "functionResult is nullptr");
      } else {
        sprintf(buf, "result count = %zd", fe->size());
        LOG(buf);
        for (int i = 0; i < fe->size(); i++) {
          bool b =
              std::dynamic_pointer_cast<CacheableBoolean>(fe->operator[](i))
                  ->value();
          LOG(b == true ? "true" : "false");
          ASSERT(b == true, "true is not eched back");
        }
      }
      LOGINFO("FETimeOut done onRegion");

      LOGINFO("FETimeOut begin onServer");
      auto serverExc = FunctionService::onServer(getHelper()->cachePtr);
      auto vec = serverExc->withArgs(CacheableInt32::create(timeout.count()))
                     ->execute(FETimeOut, timeout)
                     ->getResult();
      if (vec == nullptr) {
        ASSERT(false, "functionResult is nullptr");
      } else {
        sprintf(buf, "result count = %zd", vec->size());
        LOG(buf);
        for (int i = 0; i < vec->size(); i++) {
          bool b =
              std::dynamic_pointer_cast<CacheableBoolean>(vec->operator[](i))
                  ->value();
          LOG(b == true ? "true" : "false");
          ASSERT(b == true, "true is not eched back");
        }
      }
      LOGINFO("FETimeOut done onServer");

      LOGINFO("FETimeOut begin onServers");
      auto serversExc = FunctionService::onServers(getHelper()->cachePtr);
      auto vecs = serversExc->withArgs(CacheableInt32::create(timeout.count()))
                      ->execute(FETimeOut, timeout)
                      ->getResult();
      if (vecs == nullptr) {
        ASSERT(false, "functionResult is nullptr");
      } else {
        sprintf(buf, "result count = %zd", vecs->size());
        LOG(buf);
        for (int i = 0; i < vecs->size(); i++) {
          bool b =
              std::dynamic_pointer_cast<CacheableBoolean>(vecs->operator[](i))
                  ->value();
          LOG(b == true ? "true" : "false");
          ASSERT(b == true, "true is not eched back");
        }
      }
      LOGINFO("FETimeOut done onServers");
    } catch (const Exception& excp) {
      std::string logmsg = "";
      logmsg += excp.getName();
      logmsg += ": ";
      logmsg += excp.what();
      LOG(logmsg.c_str());
      LOG(excp.getStackTrace());
      FAIL("Function Execution Failed!");
    }
    isPoolWithEndpoint = true;
  }
END_TASK_DEFINITION

DUNIT_TASK_DEFINITION(SERVER, CloseServers)
  {
    // stop servers
    if (isLocalServer) {
      CacheHelper::closeServer(1);
      LOG("SERVER1 stopped");
    }
    if (isLocalServer) {
      CacheHelper::closeServer(2);
      LOG("SERVER2 stopped");
    }
    if (isLocalServer) {
      CacheHelper::closeServer(3);
      LOG("SERVER3 stopped");
    }
    isPoolWithEndpoint = true;
  }
END_TASK_DEFINITION

DUNIT_TASK_DEFINITION(LOCATOR1, CloseLocator1)
  {
    // stop locator
    if (isLocator) {
      CacheHelper::closeLocator(1);
      LOG("Locator1 stopped");
    }
  }
END_TASK_DEFINITION

class putThread : public ACE_Task_Base {
 private:
  std::shared_ptr<Region> regPtr;
  int m_min;
  int m_max;
  int m_failureCount;
  int m_timeoutCount;
  volatile bool m_stop;

 public:
  putThread(std::shared_ptr<Region> rp, int min, int max, bool isWarmUpTask)
      : regPtr(rp),
        m_min(min),
        m_max(max),
        m_failureCount(0),
        m_timeoutCount(0),
        m_stop(false) {}

  int getFailureCount() { return m_failureCount; }

  int getTimeoutCount() { return m_timeoutCount; }

  int svc(void) {
    bool networkhop ATTR_UNUSED = false;
  std::shared_ptr<Cacheable> args = nullptr;
  std::shared_ptr<ResultCollector> rPtr = nullptr;
    auto regPtr0 = getHelper()->getRegion(poolRegNames[0]);
    while (!m_stop) {
      for (int i = m_min; i < m_max; i++) {
        try {
          char buf[128];
          sprintf(buf, "am-%d", i);
          auto key = CacheableKey::create(buf);
          auto routingObj = CacheableVector::create();
          routingObj->push_back(key);
          auto exc = FunctionService::onRegion(regPtr0);
          exc->execute(routingObj, args, rPtr, getFuncName2,
                       std::chrono::seconds(300))
              ->getResult();
        } catch (const TimeoutException& te) {
          LOGINFO("Timeout exception occurred %s", te.what());
          m_timeoutCount++;
        } catch (const Exception&) {
          LOG("Exception occurred");
        } catch (...) {
          LOG("Random Exception occurred");
        }
      }
    }
    LOG("PutThread done");
    return 0;
  }
  void start() { activate(); }
  void stop() {
    m_stop = true;
    wait();
  }
};

void executeFunction() {
  auto regPtr0 = getHelper()->getRegion(poolRegNames[0]);
  TestUtils::getCacheImpl(getHelper()->cachePtr)->getAndResetNetworkHopFlag();
std::shared_ptr<Cacheable> args = nullptr;
std::shared_ptr<ResultCollector> rPtr = nullptr;
  int failureCount = 0;
  LOGINFO("executeFunction started");
  for (int i = 0; i < 300; i++) {
    LOGINFO("executeFunction %d ", i);
    bool networkhop = TestUtils::getCacheImpl(getHelper()->cachePtr)
                          ->getAndResetNetworkHopFlag();
    if (networkhop) {
      failureCount++;
    }
    char buf[128];
    sprintf(buf, "am-%d", i);
    auto key = CacheableKey::create(buf);
    auto routingObj = CacheableVector::create();
    routingObj->push_back(key);
    auto exc = FunctionService::onRegion(regPtr0);
    exc->execute(routingObj, args, rPtr, getFuncName2,
                 std::chrono::seconds(300))
        ->getResult();
  }
  LOGINFO("executeFunction failureCount %d", failureCount);
  ASSERT(failureCount <= 10 && failureCount > 0, "failureCount should be zero");
}

DUNIT_TASK_DEFINITION(CLIENT1, closeServer2)
  {
    // stop servers
    if (isLocalServer) {
      CacheHelper::closeServer(2);
      LOG("SERVER2 stopped");
    }
  }
END_TASK_DEFINITION

DUNIT_TASK_DEFINITION(CLIENT1, closeServer1)
  {
    // stop servers
    if (isLocalServer) {
      CacheHelper::closeServer(1);
      LOG("SERVER1 stopped");
    }
  }
END_TASK_DEFINITION

void runFunctionExecution() {
  CALL_TASK(StartLocator1);
  CALL_TASK(StartS12);
  CALL_TASK(StartC1);
  CALL_TASK(Client1OpTest);
  CALL_TASK(Client2OpTest);
  CALL_TASK(StopC1);
  CALL_TASK(CloseServers);
  CALL_TASK(CloseLocator1);
}

DUNIT_MAIN
  { runFunctionExecution(); }
END_MAIN
