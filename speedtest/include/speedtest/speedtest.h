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

namespace speedtest {

    struct TestResult {
        std::chrono::nanoseconds exec_time;
        bool exec_result;
    };

    template<class Tester, class Solution>
    TestResult run(const Tester& t) {
        Tester tester_copy = t;
        TestResult ret;

        std::cout << "Running solution " << Solution::name() << " on test " << Tester::name() << std::endl;
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
        
    private:
        T val_;
    };

    template<class... Types>
    TesterList<Types...> testers(Types&&... args) {
        return TesterList<Types...>(args...);
    }

    template<class Solution, class TesterList>
    void process_solution(const TesterList& tl) {
        std::cout << "Processing solution " << Solution::name() << std::endl;
        tl.template run<Solution>();
        std::cout << "Finished processing solution " << Solution::name() << std::endl;
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
