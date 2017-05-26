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

class random_test {
    int seed_, n_, m_, count_;
    bool test_correctness_;
    std::mt19937 rnd;
    const int INF = std::numeric_limits<int>::max();
public:
    random_test(int seed, int n, int m, int count, bool test_correctness = false)
        : seed_(seed), n_(n), m_(m), count_(count), rnd(seed_), test_correctness_(test_correctness) {}
    std::string name() const {
        return "random";
    }
    int num_tests() const {
        return count_;
    }
    template<class Solution>
    bool test() {
        Solution s(n_);
        if (test_correctness_) {
            std::vector<int> v(n_, INF);
            for (int i = 0; i < m_; i++) {
                if (rnd() % 2) {
                    int i = rnd() % n_;
                    int x = rnd();
                    s.set(i, x);
                    v[i] = x;
                } else {
                    int l = rnd() % n_;
                    int r = rnd() % n_;
                    if (l > r)
                        std::swap(l, r);
                    if (s.get_min(l, r) != *std::min_element(v.begin() + l, v.begin() + r + 1))
                        return false;
                }
                return true;
            }
        } else {
            for (int i = 0; i < m_; i++) {
                if (rnd() % 2) {
                    int i = rnd() % n_;
                    int x = rnd();
                    s.set(i, x);
                } else {
                    int l = rnd() % n_;
                    int r = rnd() % n_;
                    if (l > r)
                        std::swap(l, r);
                    s.get_min(l, r);
                }
            }
            return true;
        }
    }
};
