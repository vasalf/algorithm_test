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

#include <ascii_table/ascii_table.h>
#include <string>
#include <vector>
#include <memory>

using namespace std;

int main() {
    Table t;

    t.addColumn(Column(Column::Header("First column", {})), 20);
    t.addColumn(Column(Column::Header("Second column", {})), 20);
    t.addColumn(Column(Column::Header("Third column", {})));

    t.addRow({make_shared<TypedCell<string>>("1, 1"),
                make_shared<TypedCell<string>>("1, 2"),
                make_shared<TypedCell<string>>("1, 3")});
    t.addRow({make_shared<TypedCell<string>>("2, 1"),
                make_shared<TypedCell<string>>("2, 2"),
                make_shared<TypedCell<string>>("2, 3")});

    t.print();

    return 0;
}
