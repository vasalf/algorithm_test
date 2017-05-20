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

#include "ascii_table/table.h"
#include "ascii_table/column.h"
#include "ascii_table/cell.h"

#include <vector>
#include <iostream>
#include <string>
#include <algorithm>

Table::Table() {}

Table::~Table() {}

void Table::addColumn(const Column& column, int width) {
    auto copy = column;
    addColumn(std::move(copy), width);
}

void Table::addColumn(Column&& column, int width) {
    columns_.push_back(column);
    column_widths_.push_back(width);
}

void Table::addRow(const std::vector<CellPtr>& values) {
    auto copy = values;
    addRow(std::move(copy));
}

void Table::addRow(std::vector<CellPtr>&& values) {
    rows_.push_back(values);
}

struct TableFormat {
    char corner;
    char hsep;
    char vsep;
};

static void print_hline(const TableFormat& tf, const std::vector<std::size_t>& column_lengths) {
    if (column_lengths.empty())
        return;
    std::cout << tf.corner;
    for (std::size_t length : column_lengths) {
        for (std::size_t i = 0; i != length; i++)
            std::cout << tf.vsep;
        std::cout << tf.corner;
    }
    std::cout << std::endl;
}

static void print_row(const TableFormat& tf, const std::vector<std::string>& row) {
    if (row.empty())
        return;
    std::cout << tf.hsep;
    for (const std::string& value : row)
        std::cout << value << tf.hsep;
    std::cout << std::endl;
}

static void format_and_print_row(const TableFormat& tf, const std::vector<std::string>& row,
                                 const std::vector<std::size_t>& lengths) {
    std::vector<std::string> will_print;
    for (std::size_t i = 0; i != lengths.size(); i++) {
        will_print.push_back(std::string(lengths[i], ' '));
        std::copy(row[i].begin(), row[i].end(), will_print[i].begin() + 1);
    }
    print_row(tf, will_print);
}

void Table::print() const {
    TableFormat tf;
    tf.corner = '+';
    tf.hsep = '|';
    tf.vsep = '-';

    std::vector<std::size_t> lengths;
    for (std::size_t column_index = 0; column_index != columns_.size(); column_index++) {
        if (column_widths_[column_index] != -1)
            lengths.push_back(column_widths_[column_index] + 2);
        else {
            std::size_t value = columns_[column_index].getHeader().Name().size() + 2;
            for (std::size_t row_index = 0; row_index != rows_.size(); row_index++) {
                value = std::max(value, rows_[row_index][column_index]
                                 ->show(std::numeric_limits<std::size_t>::max()).size() + 2); 
            }
            lengths.push_back(value);
        }
    }

    print_hline(tf, lengths);
    std::vector<std::string> values;
    for (const Column& column : columns_)
        values.push_back(column.getHeader().Name());
    format_and_print_row(tf, values, lengths);
    values.clear();
    print_hline(tf, lengths);
    for (const auto& row : rows_) {
        for (std::size_t i = 0; i != columns_.size(); i++)
            values.push_back(row[i]->show(lengths[i]));
        format_and_print_row(tf, values, lengths);
        values.clear();
    }
    print_hline(tf, lengths);
}
    
