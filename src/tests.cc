
// Copyright (c) 2009 Mohannad Alharthi (mohannad.harthi@gmail.com)
// All rights reserved.
// This source code is licensed under the BSD license, which can be found in
// the LICENSE.txt file.

#ifdef _TEST_BUILD

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#include "code_gen.h"
#include "str_helper.h"
//#include "intermediate.h"

// Some helper macros
#define CHECK_RESULT(condition, message, result) \
  std::cout << message << (condition ? ": OK" : ": FAILD") << std::endl; \
  if (!(condition)) \
    std::cout << "\t" << result << std::endl;

// A macro for adding a TestJob object to a TestSet object
#define ADD_TEST(SET, JOB_PTR) SET.jobs_.push_back(JOB_PTR);


// The base class for a test job (or task)
class TestJob {
 public:
  virtual ~TestJob() { }
  virtual void operator()() = 0;
};

void RunTest(TestJob* job) {
  (*job)();
}
void Deallocate(TestJob* job) {
  delete job;
}

// A test set, containing pointers to TestJob objects 
class TestSet {
 public:
  TestSet(const std::string name)
    : name_(name) {
  }
  ~TestSet() {
    std::for_each(jobs_.begin(), jobs_.end(), Deallocate);
  }

  void Start() {
    std::cout << std::endl << name_ << std::endl;
    std::cout << std::string(25, '=') << std::endl;

    std::for_each(jobs_.begin(), jobs_.end(), RunTest);
  };

  std::vector<TestJob*> jobs_;

private:
  std::string name_;
};

// Our unit tests

class FormatTestJob : public TestJob {
public:
  virtual void operator()() {
    using utilities::__Format;

    std::string test1 = Format("Hello, %s %d", "Mohannad", 1986);
    CHECK_RESULT(test1 == "Hello, Mohannad 1986", "utlities::__Format<char>(...) with 'const char*'", test1);
    
    std::string test2 = Format("Hello, %S %d !%", std::string("Mohannad"), 1986);
    CHECK_RESULT(test2 == "Hello, Mohannad 1986 !%", "utlities::__Format<char>(...) with 'std::string'", test2);

    std::wstring test3 = FormatW(L"wHello, %s %d !", L"wMohannad", 1986);
    CHECK_RESULT(test3 == L"wHello, wMohannad 1986 !", "utlities::__Format<wchar_t>(...) with 'const wchar_t*'", WstringToString(test3));

    std::wstring test4 = FormatW(L"wHello, %S %d !", std::wstring(L"wMohannad"), 1986);
    CHECK_RESULT(test4 == L"wHello, wMohannad 1986 !", "utlities::__Format<wchar_t>(...) with 'std::wstring'", WstringToString(test4));
  }

 private:
  // Bad helper function !
  // wstring to string converter
  std::string WstringToString(const std::wstring& w_string) {
    std::string result(w_string.length(), ' ');
    std::copy(w_string.begin(), w_string.end(), result.begin());
    return result;
  }
};

//class IL_TestJob : public TestJob {
//  virtual void operator()() {
//    SymbolTable symbol_table(NULL);
//    symbol_table.Insert(new VariableSymbol("x"));
//
//    VariableOperand var_x("x", &symbol_table);
//    IntermediateInstr instruction(kPlus, &var_x, &var_x, new NumberOperand(10));
//    
//    std::string test1 = instruction.GetAsString();
//    CHECK_RESULT("x = x + 10" == test1, test1, test1);
//  }
//};

// The start point of testing
void RunTests() {
  std::cout << "Running Tests\r\n" << std::endl;

  //TestSet intermediate_set("IL Tests");
  //ADD_TEST(intermediate_set, new IL_TestJob);
  //intermediate_set.Start();

  TestSet format_test("Format<Elem>(...) Test");
  ADD_TEST(format_test, new FormatTestJob);
  format_test.Start();
}

#endif // _TEST_BUILD
