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

#include <vector>
#include <algorithm>
#include <limits>
#include <string>
#include <cassert>

class segment_tree_from_top {
    const int INF = std::numeric_limits<int>::max();
    int n;
    std::vector<int> t;
    void set(int v, int L, int R, int i, int x) {
        if (R <= i || i < L)
            return;
        else if (L == R - 1)
            t[v] = x;
        else {
            set(2 * v + 1, L, (L + R) >> 1, i, x);
            set(2 * v + 2, (L + R) >> 1, R, i, x);
            t[v] = std::min(t[2 * v + 1], t[2 * v + 2]);
        }
    }
    int get(int v, int L, int R, int l, int r) {
        if (R <= l || r <= L)
            return INF;
        else if (l <= L && R <= r)
            return t[v];
        else
            return std::min(get(2 * v + 1, L, (L + R) >> 1, l, r),
                            get(2 * v + 2, (L + R) >> 1, R, l, r));
    }
public:
    static std::string name() {
        return "st_top";
    }
    segment_tree_from_top(int _n) {
        n = _n;
        t.resize(4 * _n, INF);
    }
    void set(int i, int val) {
        assert(i < n);
        set(0, 0, n, i, val);
    }
    int get_min(int l, int r) {
        return get(0, 0, n, l, r + 1);
    }
};
