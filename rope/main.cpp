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

/**
 * Rope speedtest.
 * This is a speedtest for several possible rope data structure realizations.
 * The structures should contain an int value.
 *
 * Solution structures must have the following public interface:
 *
 * class generic_solution {
 * public:
 *     generic_solution();
 *     generic_solution(const generic_solution&) = delete;
 *     ~generic_solution();
 *     // Insert a value before the given position.
 *     void insert(int before, int value);
 *     // Erase a value at the given position.
 *     void erase(int where);
 *     // Split into two structures, left first elements to the first.
 *     std::pair<generic_solution, generic_solution> split(int left);
 *     // Merge two structures into one, lt would be the first.
 *     static generic_solution merge(const generic_solution&& lt, const generic_solution&& rt);
 *     // Access element at index.
 *     int at(int index);
 *     // Convert to std::vector<int>.
 *     operator std::vector<int>();
 *     // Solution name
 *     static std::string name();
 * };
 */


#include <speedtest/speedtest.h>

#include <tests/build_long_struct.h>
#include <tests/build_shuffle.h>
#include <tests/insert_erase.h>
#include <tests/build_insert_erase.h>

#include <solutions/treap.h>
#include <solutions/splay.h>
#include <solutions/avl.h>

#include <random>

int main(int argc, char *argv[]) {
    speedtest::init(speedtest::testers(build_long_struct(179, 1e6),
                                       build_shuffle(179, 1e6),
                                       insert_erase(179, 5e6),
                                       build_insert_erase(179, 1e6, 1e6)),
                    speedtest::solutions<
                            olymp_treap<c_rnd_eng>,
                            olymp_treap<std::mt19937>,
                            opt_treap<c_rnd_eng>,
                            opt_treap<std::mt19937>,
                            nr_treap<c_rnd_eng>,
                            nr_treap<std::mt19937>,
                            splay_tree,
                            avl_tree
                    >());

    speedtest::run(argc, argv);
    return 0;
}
