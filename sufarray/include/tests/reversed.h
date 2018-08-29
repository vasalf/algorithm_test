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

#include <random>
#include <algorithm>
#include <string>

class reversed {
    int n_;
    std::string test_;

public:
    reversed(int seed, int n) : n_(n) {
        std::mt19937 rnd(seed);
        std::uniform_int_distribution<char> dist('a', 'z');
        for (int i = 0; i < n; i++) {
            test_.push_back(dist(rnd));
        }
        sort(test_.rbegin(), test_.rend());
    }

    std::string name() const {
        return "reversed";
    }

    template<class Solution>
    bool test() {
        std::vector<int> sa = Solution::sufarray(test_);

#ifdef VERIFY
        for (int i = 0; i < n_ - 1; i++) {
            if (test_.substr(sa[i]) > test_.substr(sa[i + 1]))
                return false;
        }
#endif

        return true;
    }
};
