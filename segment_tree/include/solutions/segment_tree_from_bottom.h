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

class segment_tree_from_bottom {
    const int INF = std::numeric_limits<int>::max();
    int n;
    std::vector<int> t;
public:
    static std::string name() {
        return "st_bottom";
    }
    segment_tree_from_bottom(int _n) {
        n = _n;
        t.resize(2 * _n, INF);
    }
    void set(int i, int val) {
        i =+ n;
        t[i] = val;
        while (i > 0) {
            i /= 2;
            t[i] = std::min(t[2 * i], t[2 * i + 1]);
        }
    }
    int get_min(int l, int r) {
        l += n;
        r += n;
        int ans = INF;
        while (l < r) {
            if (l & 1) ans = std::min(ans, t[l++]);
            if (!(r & 1)) ans = std::min(ans, t[r--]);
            l /= 2; r /= 2;
        }
        if (l == r)
            ans = std::min(ans, t[l]);
        return ans;
    }
};
