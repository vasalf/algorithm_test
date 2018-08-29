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

#include <speedtest/speedtest.h>

#include <tests/random_str.h>
#include <tests/ordered.h>
#include <tests/reversed.h>

#include <solutions/nlog2_hashes.h>
#include <solutions/nlogn.h>

int main(int argc, char *argv[]) {
    const int test_size = 1e6;

    speedtest::init(speedtest::testers(
                        random_str(179, test_size),
                        ordered(179, test_size),
                        reversed(179, test_size)),
                    speedtest::solutions<
                        nlog2_hashes<std_sort>,
                        nlog2_hashes<std_stable_sort>,
                        nlogn
                    >());

    speedtest::run(argc, argv);
    return 0;
}
