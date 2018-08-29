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
#include <iostream>

#define PYMOD(A, M) ((((A) % (M)) + (M)) % (M))

class nlogn {
public:
    static std::vector<int> sufarray(std::string s) {
        s.push_back('#');
        int n = s.size();
        std::vector<int> p(n);
        std::iota(p.begin(), p.end(), 0);
        std::sort(p.begin(), p.end(), [&s](int i, int j) -> bool { return s[i] < s[j]; });
        std::vector<int> c(n), pos(n);
        c[p[0]] = 0;
        for (int i = 1; i < n; i++) {
            c[p[i]] = c[p[i - 1]] + (s[p[i]] == s[p[i - 1]] ? 0 : 1);
        }
        std::vector<int> p2(n), c2(n);
        for (int k = 1; k < n; k *= 2) {
            pos[0] = 0;
            for (int i = 1; i < n; i++) {
                if (c[p[i]] != c[p[i - 1]]) {
                    pos[c[p[i]]] = i;
                }
            }
            for (int i = 0; i < n; i++) {
                int j = PYMOD(p[i] - k, n);
                p2[pos[c[j]]++] = j;
            }
            std::swap(p, p2);
            c2[p[0]] = 0;
            for (int i = 1; i < n; i++) {
                int jprev = PYMOD(p[i - 1] + k, n);
                int jcur = PYMOD(p[i] + k, n);
                auto pprev = std::make_pair(c[p[i - 1]], c[jprev]);
                auto pcur = std::make_pair(c[p[i]], c[jcur]);
                c2[p[i]] = c2[p[i - 1]] + (pprev == pcur ? 0 : 1); 
            }
            std::swap(c, c2);
        }
        return std::vector<int>(p.begin() + 1, p.end());
    }

    static std::string name() {
        return "nlogn";
    }
};

#undef PYMOD
