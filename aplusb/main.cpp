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

#include <speedtest/speedtest.h>

#include <sstream>

using namespace std;

class DummyTest {
    int _a, _b;
public:
    DummyTest(int a, int b) : _a(a), _b(b) {}

    string name() const {
        std::ostringstream ss;
        ss << "dummy_test(" << _a << "," << _b << ")";
        return ss.str();
    }

    template<class Solution>
    bool test() {
        Solution s;
        int x = s.solve(_a, _b);
        return x == _a + _b;
    }
};

class TheSolution {
public:
    TheSolution() {}

    static string name() {
        return "the_solution";
    }

    int solve(int a, int b) {
        return a + b;
    }
};

int main(int argc, char *argv[]) {
    speedtest::init(speedtest::testers(DummyTest(1, 2),
                                       DummyTest(3, 4)),
                    speedtest::solutions<
                    TheSolution,
                    TheSolution
                    >());
    speedtest::run(argc, argv);
    return 0;
}
