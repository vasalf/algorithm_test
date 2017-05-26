// -*- mode: c++; -*-
/*
 * Copyright (c) 2017 Vasily Alferov
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

namespace speedtest {

    struct TestResult {
        std::string solution_name;
        std::string test_name;
        std::chrono::nanoseconds exec_time;
        bool exec_result;
    };

    class StatOutputMethod {
    public:
        StatOutputMethod() {}
        virtual ~StatOutputMethod() {}

        virtual void add_test(std::string test_name) = 0;
        virtual void print(std::string solution_name, std::deque<TestResult> tr) = 0;
        virtual void flush() = 0;
    };

    struct SpeedTestConfig {
        enum class OutputMethod { ASCIITable, PlainText };
        std::unique_ptr<StatOutputMethod> output;
        bool quiet = false;
        bool print_help = false;
        OutputMethod output_method = OutputMethod::ASCIITable;
    };

    extern SpeedTestConfig st_config;
    
    template<class Tester, class Solution>
    TestResult run(const Tester& t) {
        Tester tester_copy = t;
        TestResult ret;

        ret.solution_name = Solution::name();
        ret.test_name = tester_copy.name();

        if (!st_config.quiet)
            std::cerr << "Running solution " << Solution::name() << " on test " << tester_copy.name() << std::endl;
        auto t1 = std::chrono::high_resolution_clock::now();
        ret.exec_result = tester_copy.template test<Solution>();
        auto t2 = std::chrono::high_resolution_clock::now();
        ret.exec_time = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1);

        return ret;
    }
    
    template<class T, class... Others>
    class TesterList {
    public:
        TesterList(T tester, Others... others) : val_(std::move(tester)),
                                                 next_(others...) {}

        template<class Solution>
        std::deque<TestResult> run() const {
            TestResult head = speedtest::run<T, Solution>(val_);
            std::deque<TestResult> ret = next_.template run<Solution>();
            ret.push_front(head);
            return ret;
        }

        void setup() {
            st_config.output->add_test(val_.name());
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
        std::deque<TestResult> run() const {
            TestResult ret = speedtest::run<T, Solution>(val_);
            return std::deque<TestResult>({ret});
        }

        void setup() {
            st_config.output->add_test(val_.name());
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
