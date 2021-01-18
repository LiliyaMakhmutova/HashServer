#include <optional>
#include "pch.h"
#include "CppUnitTest.h"
#include "../HashServer/HashMap.h"
#include "../HashServer/HashMap.cpp"
#include <cstdlib>
#include "windows.h" 

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTestHashMap
{
	TEST_CLASS(UnitTestHashMap)
	{
	public:		
		TEST_METHOD(TestCanGetAfterPut)
		{
			int key = 1;
			std::string value = "apple";
			HashMap hm;
            hm.put(key, value, 1000);			
            //Assert::AreNotEqual(std::nullopt, hm.get(key)); // no std::optional in MUTFramework
			Assert::AreEqual(*hm.get(key), value);
		}
		TEST_METHOD(TestCannotGetAfterRemove) {
            int key = 1;
            std::string value = "apple";
            HashMap hm;
            hm.put(key, value, 1000);
            hm.remove(key);
            Assert::AreNotEqual(*hm.get(key), value);           
        }
        TEST_METHOD(TestCanGetAfterRemoveOtherKey) {
          int key1 = 1;
          int key2 = 2;
          std::string value1 = "apple";
          std::string value2 = "banana";
          HashMap hm;
          hm.put(key1, value1, 1000);
          hm.put(key2, value2, 1000);
          hm.remove(key1);
          Assert::AreNotEqual(*hm.get(key1), value1);
          Assert::AreEqual(*hm.get(key2), value2);
        }
		TEST_METHOD(TestReplaceNewValue) {
          int key = 1;
          std::string value1 = "apple";
          std::string value2 = "banana";
          HashMap hm;
          hm.put(key, value1, 1000);
          hm.put(key, value2, 1000);
          Assert::AreEqual(*hm.get(key), value2);
        }
        TEST_METHOD(TestGetReturnsCorrectMultipleValues) {
          int key1 = 1;
          int key2 = 2;
          std::string value1 = "apple";
          std::string value2 = "banana";
          HashMap hm;
          hm.put(key1, value1, 1000);
          hm.put(key2, value2, 1000);
          Assert::AreEqual(*hm.get(key1), value1);
          Assert::AreEqual(*hm.get(key2), value2);
        }
        TEST_METHOD(TestCannotGetAfterPutAndLongTime) {
          int key = 1;
          std::string value = "apple";
          HashMap hm;
          hm.put(key, value, 1);
          Sleep(5000); // time in milliseconds
          Assert::AreNotEqual(*hm.get(key), value);
        }
	};
}
