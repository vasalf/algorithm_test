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

#include <speedtest/speedtest.h>
#include <ascii_table/ascii_table.h>
#include <sstream>

template<>
class TypedCell<double> : public Cell {
public:
    TypedCell(double val) : value_(val) {}
    virtual ~TypedCell() {}

    virtual std::string show(std::size_t maxlen) const {
        std::ostringstream ss;
        ss.precision(9);
        ss << std::fixed << value_;
        if (ss.str().length() > maxlen)
            throw CellLengthException(ss.str(), maxlen);
        return ss.str();
    }
private:
    double value_;
};

namespace speedtest {
    std::unique_ptr<BasicSpeedTest> st_instance;
    SpeedTestConfig st_config;
    
    class PlainTextStatOutputMethod : public StatOutputMethod {
    public:
        PlainTextStatOutputMethod() {
            out.precision(9);
            out << std::fixed;
        }
        virtual ~PlainTextStatOutputMethod() {}

        virtual void add_test(std::string) {}
        virtual void print(std::string, std::deque<TestResult> tr) {
            for (auto& res : tr) {
                out << "Solution " << res.solution_name;
                if (res.exec_result)
                    out << " succeeded ";
                else
                    out << " failed ";
                out << "on test " << res.test_name;
                out << ". Time: " << (double)res.exec_time.count() / 1e9 << " s";
                out << std::endl;
            }
        }
        virtual void flush() {
            std::cout << out.str();
        }
    private:
        std::ostringstream out;
    };
    
    class ASCIITableStatOutputMethod : public StatOutputMethod {
    public:
        ASCIITableStatOutputMethod() {
            table_.addColumn(Column(Column::Header("solution", {})));
        }
        virtual ~ASCIITableStatOutputMethod() {}

        virtual void add_test(std::string test_name) {
            table_.addColumn(Column(Column::Header(test_name, {})));
        }
        virtual void print(std::string solution_name, std::deque<TestResult> tr) {
            std::vector<CellPtr> table_row;
            table_row.push_back(make_cell<std::string>(solution_name));
            for (auto& res : tr) {
                table_row.push_back(make_cell<double>((double)res.exec_time.count() / 1e9));
            }
            table_.addRow(table_row);
        }
        virtual void flush() {
            table_.print();
        }
    private:
        Table table_;
    };
    
    void run(int argc, char* argv[]) {
        st_config.output.reset(new ASCIITableStatOutputMethod());
        st_instance->setup();
        st_instance->run();
        st_config.output->flush();
    }
};
