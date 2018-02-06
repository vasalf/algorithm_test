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
#include <ascii_table/ascii_table.h>
#include <cstring>
#include <sstream>

template<>
class TypedCell<double> : public Cell {
public:
    TypedCell(double val) : value_(val) {}
    virtual ~TypedCell() {}

    virtual std::string show(std::size_t maxlen) const {
        std::ostringstream ss;
        ss.precision(3);
        ss << std::fixed << value_ << " s";
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
        virtual void add_multitest(std::string, int) {}
        virtual void print(std::string, std::deque<TestResultPtr> tr) {
            for (auto& res : tr) {
                res->print_test(*this);
            }
        }
        virtual void print_multitest_result(const MultitestResult& result) {
            out << "Solution " << result.solution_name;
            if (result.exec_result)
                out << " succeeded ";
            else
                out << " failed ";
            out << "on test " << result.test_name;
            out << ". Time: " << (double)result.exec_time.count() / 1e9 << " s";
            out << " (avg: " << (double) result.exec_time.count() / result.test_num / 1e9 << " s)";
            out << std::endl;
        }
        virtual void print_single_test_result(const SingleTestResult& result) {
            out << "Solution " << result.solution_name;
            if (result.exec_result)
                out << " succeeded ";
            else
                out << " failed ";
            out << "on test " << result.test_name;
            out << ". Time: " << (double)result.exec_time.count() / 1e9 << " s" << std::endl;
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
        virtual void add_multitest(std::string test_name, int num_tests) {
            if (num_tests == 1)
                add_test(test_name);
            else {
                table_.addColumn(Column(Column::Header(test_name, {"total", "avg"})));
            }
        }
        std::vector<CellPtr> table_row;
        virtual void print(std::string solution_name, std::deque<TestResultPtr> tr) {
            table_row.clear();
            table_row.push_back(make_cell<std::string>(solution_name));
            for (auto& res : tr) {
                res->print_test(*this);
            }
            table_.addRow(table_row);
        }
        virtual void print_multitest_result(const MultitestResult& result) {
            if (result.exec_result) {
                table_row.push_back(make_cell<double>((double) result.exec_time.count() / 1e9));
                table_row.push_back(make_cell<double>((double) result.exec_time.count() / result.test_num / 1e9));
            } else {
                table_row.push_back(make_cell<std::string>("FAIL"));
                table_row.push_back(make_cell<std::string>("FAIL"));
            }
        }
        virtual void print_single_test_result(const SingleTestResult& result) {
            if (result.exec_result) {
                table_row.push_back(make_cell<double>((double) result.exec_time.count() / 1e9));
            } else {
                table_row.push_back(make_cell<std::string>("FAIL"));
            }
        }
        virtual void flush() {
            table_.print();
        }
    private:
        Table table_;
    };

    void usage(std::string app) {
        std::string message =
            "This is a libspeedtest application\n"
            "\n"
            "Usage: " + app + " [OPTION]...\n"
            "\n"
            "Available options are: \n"
            "      --quiet                Do not output messages about testing status\n"
            "                             to stderr\n"
            "      --plaintext            Do not use ASCII tables, display stats in\n"
            "                             plain text\n"
            "  -h  --help                 Display this help message and exit\n"
            "\n"
            "Copyright (c) 2017-2018 Vasily Alferov\n"
            "You may find additional help information on https://github.com/vasalf/algorithm_test";
        std::cout << message << std::endl;
    }
    
    void parse_opts(int argc, char* argv[]) {
        for (int i = 1; i < argc; i++) {
            if (std::strcmp(argv[i], "--quiet") == 0)
                st_config.quiet = true;
            else if (std::strcmp(argv[i], "--plaintext") == 0)
                st_config.output_method = SpeedTestConfig::OutputMethod::PlainText;
            else if (std::strcmp(argv[i], "--help") == 0
                     || std::strcmp(argv[i], "-h") == 0)
                st_config.print_help = true;
        }
    }
    
    void run(int argc, char* argv[]) {
        parse_opts(argc, argv);

        if (st_config.print_help) {
            usage(argv[0]);
            exit(0);
        }
        
        switch (st_config.output_method) {
        case SpeedTestConfig::OutputMethod::ASCIITable:
            st_config.output.reset(new ASCIITableStatOutputMethod());
            break;
        case SpeedTestConfig::OutputMethod::PlainText:
            st_config.output.reset(new PlainTextStatOutputMethod());
            break;
        default:
            break;
        }
        
        st_instance->setup();
        st_instance->run();
        st_config.output->flush();
    }

    void SingleTestResult::print_test(StatOutputMethod &stat_output) {
        stat_output.print_single_test_result(*this);
    }

    void MultitestResult::print_test(StatOutputMethod &stat_output) {
        stat_output.print_multitest_result(*this);
    }
};
