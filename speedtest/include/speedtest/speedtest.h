// -*- mode: c++; -*-
/*
 * Copyright (c) 2017-2018 Vasily Alferov
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once

#include <iostream>
#include <utility>
#include <memory>
#include <chrono>
#include <deque>
#include <string>
#include <type_traits>

namespace speedtest {
    class StatOutputMethod;

    struct BasicTestResult {
        std::string solution_name;
        std::string test_name;
        bool exec_result;
        virtual void print_test(StatOutputMethod& statOutputMethod) = 0;
    };

    struct SingleTestResult : public BasicTestResult {
        std::chrono::nanoseconds exec_time;

        SingleTestResult(bool result, std::chrono::nanoseconds time_elapsed) {
            exec_result = result;
            exec_time = time_elapsed;
        }

        virtual void print_test(StatOutputMethod& stat_output);
    };

    struct MultitestResult : public BasicTestResult {
        int test_num;
        std::chrono::nanoseconds exec_time;

        MultitestResult(bool result, std::chrono::nanoseconds time_elapsed, int num_tests) {
            exec_result = result;
            exec_time = time_elapsed;
            test_num = num_tests;
        }

        virtual void print_test(StatOutputMethod& stat_output);
    };

    typedef std::shared_ptr<BasicTestResult> TestResultPtr;

    class StatOutputMethod {
    public:
        StatOutputMethod() {}
        virtual ~StatOutputMethod() {}

        virtual void add_test(std::string test_name) = 0;
        virtual void add_multitest(std::string test_name, int test_num) = 0;
        virtual void print(std::string solution_name, std::deque<TestResultPtr> tr) = 0;
        virtual void flush() = 0;
        virtual void print_multitest_result(const MultitestResult& result) = 0;
        virtual void print_single_test_result(const SingleTestResult& result) = 0;
    };

    // Member check idiom
    // [some black magic from the deep abyss of C++]
    template<class Tester>
    class IsMultitest {
        template<class U, int (U::*)() const> struct Check;
        template<class U> static char check(Check<U, &U::num_tests> *);
        template<class U> static int check(...);
    public:
        static constexpr bool value = sizeof(check<Tester>(nullptr)) == sizeof(char);
    };

    template<class Tester>
    using is_multitest = std::integral_constant<bool, IsMultitest<Tester>::value>;
    template<class Tester>
    using is_singletest = std::integral_constant<bool, !IsMultitest<Tester>::value>;
    
    struct SpeedTestConfig {
        enum class OutputMethod { ASCIITable, PlainText };
        std::unique_ptr<StatOutputMethod> output;
        bool quiet = false;
        bool print_help = false;
        OutputMethod output_method = OutputMethod::ASCIITable;
        // stackoverflow hacks
        template<class Tester>
        typename std::enable_if<is_multitest<Tester>::value, void>::type add_test(const Tester& t) {
            output->add_multitest(t.name(), t.num_tests());
        }
        template<class Tester>
        typename std::enable_if<is_singletest<Tester>::value, void>::type add_test(const Tester& t) {
            output->add_test(t.name());
        }
        template<class Tester>
        static typename std::enable_if<is_multitest<Tester>::value, int>::type get_num_tests(const Tester& t) {
            return t.num_tests();
        }
        template<class Tester>
        static typename std::enable_if<is_singletest<Tester>::value, int>::type get_num_tests(const Tester&) {
            return -1;
        }
    };
    
    extern SpeedTestConfig st_config;

    template<class Tester, class Solution>
    typename std::enable_if<is_multitest<Tester>::value, TestResultPtr>::type inner_run(Tester& t) {
        bool ret = true;
        int rep = t.num_tests();

        auto t1 = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < rep; i++) {
            ret = ret & t.template test<Solution>();
        }
        auto t2 = std::chrono::high_resolution_clock::now();

        return std::make_shared<MultitestResult>(ret, t2 - t1, rep);
    }

    template<class Tester, class Solution>
    typename std::enable_if<is_singletest<Tester>::value, TestResultPtr>::type inner_run(Tester& t) {
        auto t1 = std::chrono::high_resolution_clock::now();
        bool result = t.template test<Solution>();
        auto t2 = std::chrono::high_resolution_clock::now();

        return std::make_shared<SingleTestResult>(result, t2 - t1);
    }

    template<class Tester, class Solution>
    TestResultPtr run(const Tester& t) {
        Tester tester_copy = t;

        if (!st_config.quiet)
            std::cerr << "Running solution " << Solution::name() << " on test " << tester_copy.name() << std::endl;
        TestResultPtr ret = inner_run<Tester, Solution>(tester_copy);

        ret->solution_name = Solution::name();
        ret->test_name = tester_copy.name();

        if (!ret->exec_result)
            std::cerr << "Warning: solution " << Solution::name() << " failed on test " << tester_copy.name() << std::endl;
        
        return ret;
    }
    
    template<class T, class... Others>
    class TesterList {
    public:
        TesterList(T tester, Others... others) : val_(std::move(tester)),
                                                 next_(others...) {}

        template<class Solution>
        std::deque<TestResultPtr> run() const {
            TestResultPtr head = speedtest::run<T, Solution>(val_);
            std::deque<TestResultPtr> ret = next_.template run<Solution>();
            ret.push_front(head);
            return ret;
        }

        void setup() {
            st_config.add_test(val_);
            next_.setup();
        }
        
    private:
        T val_;
        TesterList<Others...> next_;
    };

    template<class T>
    class TesterList<T> {
    public:
        TesterList(T tester) : val_(std::move(tester)) {}

        template<class Solution>
        std::deque<TestResultPtr> run() const {
            TestResultPtr ret = speedtest::run<T, Solution>(val_);
            return std::deque<TestResultPtr>({ret});
        }

        void setup() {
            st_config.add_test(val_);
        }
        
    private:
        T val_;
    };

    template<class... Types>
    TesterList<Types...> testers(Types&&... args) {
        return TesterList<Types...>(args...);
    }

    template<class Solution, class TesterList>
    void process_solution(const TesterList& tl) {
        if (!st_config.quiet)
            std::cerr << "Processing solution " << Solution::name() << std::endl;
        auto res = tl.template run<Solution>();
        if (!st_config.quiet)
            std::cerr << "Finished processing solution " << Solution::name() << std::endl;
        st_config.output->print(Solution::name(), res);
    }

    template<class T, class... Others>
    class SolutionList {
    public:
        SolutionList() : next_() {}

        template<class TesterList>
        void run(const TesterList& tl) const {
            speedtest::process_solution<T, TesterList>(tl);
            next_.run(tl);
        }
        
    private:
        SolutionList<Others...> next_;
    };

    template<class T>
    class SolutionList<T> {
    public:
        SolutionList() {}

        template<class TesterList>
        void run(const TesterList& tl) const {
            speedtest::process_solution<T, TesterList>(tl);
        }
    };

    template<class... Types>
    SolutionList<Types...> solutions() {
        return SolutionList<Types...>();
    }
    
    class BasicSpeedTest {
    public:
        BasicSpeedTest() {}
        ~BasicSpeedTest() {}

        virtual void run() = 0;
        virtual void setup() = 0;
    };

    template<class StoredTesterList,
             class StoredSolutionList>
    class SpeedTest : public BasicSpeedTest {
    public:
        
        SpeedTest(StoredTesterList&& tester_list,
                  StoredSolutionList&& solution_list) : tester_list_(tester_list),
                                                        solution_list_(solution_list) {}

        virtual ~SpeedTest() {}
        
        virtual void run() {
            solution_list_.run(tester_list_);
        }

        virtual void setup() {
            tester_list_.setup();
        }
        
    private:
        StoredTesterList tester_list_;
        StoredSolutionList solution_list_;
    };

    extern std::unique_ptr<BasicSpeedTest> st_instance;

    template<class StoredTesterList,
             class StoredSolutionList>
    void init(StoredTesterList&& tl, StoredSolutionList&& sl) {
        st_instance.reset(new SpeedTest<StoredTesterList, StoredSolutionList>(std::move(tl), std::move(sl)));
    }
    
    void run(int argc, char* argv[]);
};
