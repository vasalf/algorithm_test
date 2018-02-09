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

#include <speedtest/runtime.h>

#include <random>
#include <limits>

class insert_erase {
    int seed_, n_;
    std::mt19937 rnd_;
    std::uniform_int_distribution<int> dist_;
#ifdef VERIFY
    std::vector<int> w;
#endif
public:
    insert_erase(int seed, int n)
        : seed_(seed),
          n_(n),
          rnd_(seed),
          dist_(std::numeric_limits<int>::min(), std::numeric_limits<int>::max()) {
#ifdef VERIFY
        std::cerr << "Running verification solution on test " << name() << std::endl;
        test<verify>();
#endif
    }

    std::string name() const {
        return "insert_erase";
    }

    std::vector<std::string> tested_params() const {
        return { "insert", "erase" };
    }

    template<class Solution>
    bool test() {
        rnd_ = std::mt19937(seed_);

        Solution s;
        int cnt = 0;

        for (int i = 0; i < n_; i++) {
            int type;
            if (cnt == 0)
                type = 1;
            else
                type = rnd_() % 2;
            if (type == 1) {
                int at = rnd_() % (cnt + 1);
                int x = dist_(rnd_);
                MULTIPARAMTEST_INVOKE("insert", s.insert(at, x);)
                cnt++;
            } else {
                int at = rnd_() % cnt;
                MULTIPARAMTEST_INVOKE("erase", s.erase(at);)
                cnt--;
            }
        }

#ifdef VERIFY
        if (Solution::name() == "verify") {
            w = (std::vector<int>)s;
        } else {
            for (int i = 0; i < cnt; i++) {
                if (w[i] != s.at(i))
                    return false;
            }
            if (w != (std::vector<int>)s) {
                return false;
            }
        }
#endif

        return true;
    }
};
