// -*- mode: c++; -*-
/*
 * Copyright (c) 2017-2018 Vasily Alferov
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or selong long
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the folong longowing conditions:
 * 
 * The above copyright notice and this permission notice shalong long be included in along long
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

#include <algorithm>
#include <string>
#include <vector>

#define PYMOD(A, M) ((((A) % (M)) + (M)) % (M))
const int hp = 179, hmod = 1791791791;

std::vector<long long> hppows = {1};
long long hppow(int i) {
    while ((int)hppows.size() < i + 1) {
        hppows.push_back((hppows.back() * hp) % hmod);
    }
    return hppows[i];
}

struct hashed_string {
    std::string s;
    std::vector<long long> h;

    hashed_string() {}

    hashed_string(std::string _s) {
        s = _s;
        h.resize(s.size() + 1);
        for (int i = 0; i < (int)s.size(); i++) {
            h[i + 1] = (h[i] * hp + (s[i] - 'a' + 1)) % hmod;
        }
    }

    // [l; r)
    long long substr_hash(int l, int r) {
        return PYMOD(h[r] - h[l] * hppow(r - l), hmod);
    }
};

struct std_sort {
    template<class It, class Cmp>
    static void sort(It lt, It rt, Cmp cmp) {
        std::sort(lt, rt, cmp);
    }

    static std::string name() {
        return "std::sort";
    }
};

struct std_stable_sort {
    template<class It, class Cmp>
    static void sort(It lt, It rt, Cmp cmp) {
        std::stable_sort(lt, rt, cmp);
    }

    static std::string name() {
        return "std::stable_sort";
    }
};

template<class sorter>
class nlog2_hashes {
public:
    static std::vector<int> sufarray(const std::string& str) {
        hashed_string s(str);
        int n = str.size();
        std::vector<int> res(n);
        std::iota(res.begin(), res.end(), 0);
        sorter::template sort(res.begin(), res.end(), [&](int i, int j) -> bool {
            int l = 0, r = std::min(n - i, n - j) + 1;
            while (l < r - 1) {
                int mid = (l + r) / 2;
                if (s.substr_hash(i, i + mid) == s.substr_hash(j, j + mid)) {
                    l = mid;
                } else {
                    r = mid;
                }
            }
            return i + l == n || (j + l < n && str[i + l] < str[j + l]);
        });
        return res;
    }

    static std::string name() {
        return "nlog2_hashes<" + sorter::name() + ">";
    }
};

#undef PYMOD
