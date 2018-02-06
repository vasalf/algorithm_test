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

#include <random>
#include <limits>
#include <algorithm>
#include <utility>

class long_queries {
    int seed_, n_, m_, count_;
    std::mt19937 rnd;
    const int INF = std::numeric_limits<int>::max();
    std::pair<int, int> gen_query() {
        int l = rnd() % (n_ / 2);
        int r = l + (n_ / 2) + rnd() % (n_ - l - (n_ / 2));
        return std::make_pair(l, r);
    }
public:
    long_queries(int seed, int n, int m, int count)
        : seed_(seed), n_(n), m_(m), count_(count), rnd(seed_){}
    std::string name() const {
        return "long_queries";
    }
    int num_tests() const {
        return count_;
    }
    template<class Solution>
    bool test() {
        Solution s(n_);
        for (int i = 0; i < m_; i++) {
            if (rnd() % 2) {
                int i = rnd() % n_;
                int x = rnd();
                s.set(i, x);
            } else {
                int l, r;
                std::tie(l, r) = gen_query();
                s.get_min(l, r);
            }
        }
        return true;
    }
};
