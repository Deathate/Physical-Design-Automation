#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>
#include <memory>
#include <optional>
#include <queue>
#include <ranges>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "cpp_utility/print.cpp"
using namespace std;

unordered_map<string, unordered_set<string>> cell_array;
unordered_map<string, pair<int, int>> cell_distr;
unordered_map<string, unordered_set<string>> net_array;
unordered_set<string> A, B;
unordered_set<string> finalA, finalB;
vector<unordered_set<string>> bucketA, bucketB;
unordered_map<string, int> bucket_searcher;
int PMAX = 0;
float LIMIT_LEFT, LIMIT_RIGHT;

bool inLeftBlock(const string &cell) {
    return A.contains(cell);
}
int Fn(const string &cell, const string &net) {
    return inLeftBlock(cell) ? cell_distr[net].first : cell_distr[net].second;
}
int Tn(const string &cell, const string &net) {
    return inLeftBlock(cell) ? cell_distr[net].second : cell_distr[net].first;
}
int gain(const string &s) {
    int g = 0;
    for (const auto &n : cell_array[s]) {
        if (Fn(s, n) == 1)
            g += 1;
        else if (Tn(s, n) == 0)
            g -= 1;
    }
    return g;
}
auto read_and_preprocess_file(const string &file) {
    ifstream fp(file);
    stringstream ss;
    ss << fp.rdbuf();
    fp.close();

    double balance_factor;
    ss >> balance_factor;
    string part;
    while (getline(ss, part, ';')) {
        istringstream block(part);
        block.ignore();
        block.ignore(4, ' ');
        string net_name;
        block >> net_name;
        while (block >> part) {
            cell_array[part].insert(net_name);
            net_array[net_name].insert(part);
        }
    }

    for (auto &[a, b] : cell_array) {
        PMAX = max((int)b.size(), PMAX);
    }
    bucketA.resize(2 * PMAX + 1);
    bucketB.resize(2 * PMAX + 1);
    float base = cell_array.size() * 0.5f;
    LIMIT_LEFT = base - (int)(balance_factor * base);
    LIMIT_RIGHT = base + (int)(balance_factor * base);
}
void add_gain(const string &ele, const int &v) {
    vector<unordered_set<string>> &bucket = inLeftBlock(ele) ? bucketA : bucketB;
    if (bucket_searcher.contains(ele)) {
        int g = bucket_searcher[ele];
        bucket[PMAX - g].erase(ele);
        bucket[PMAX - g - v].insert(ele);
        bucket_searcher[ele] = g + v;
    }
}
void lock_cell(bool inLeft, const string &cell) {
    bucketA[PMAX - bucket_searcher[cell]].erase(cell);
    bucketB[PMAX - bucket_searcher[cell]].erase(cell);
    bucket_searcher.erase(cell);
    int move = 0;
    if (inLeft) {
        move = -1;
        A.erase(cell);
        B.insert(cell);
    } else {
        move = 1;
        B.erase(cell);
        A.insert(cell);
    }
    for (const auto &i : cell_array[cell]) {
        cell_distr[i].first += move;
        cell_distr[i].second -= move;
    }
}
void initial_gains() {
    for (const auto &[cell, _] : cell_array) {
        int g = gain(cell);
        vector<unordered_set<string>> &bucket = inLeftBlock(cell) ? bucketA : bucketB;
        bucket[PMAX - g].insert(cell);
        bucket_searcher[cell] = g;
    }
}
optional<pair<int, string>> bucket_top(const vector<unordered_set<string>> &bucket) {
    int size = bucket.size();
    for (int i = 0; i < size; ++i) {
        auto &u = bucket[i];
        if (!u.empty()) {
            return {{PMAX - i, *u.begin()}};
        }
    }
    return nullopt;
}
int cut_size(const unordered_set<string> &A, const unordered_set<string> &B) {
    int n = 0;
    for (auto &[net, cells] : net_array) {
        if (any_of(cells.begin(), cells.end(), [&](const string &s) { return A.contains(s); }) &&
            any_of(cells.begin(), cells.end(), [&](const string &s) { return B.contains(s); })) {
            n += 1;
        }
    }
    return n;
}
void write_to_file(const string &file, const unordered_set<string> &a, const unordered_set<string> &b, int cutsize) {
    ofstream fp(file);
    fp << "Cutsize = " << cutsize << "\n";
    fp << "G1 " << a.size() << "\n";
    for (const auto &i : a) {
        fp << i << " ";
    }
    fp << ";\n";
    fp << "G2 " << b.size() << "\n";
    for (const auto &i : b) {
        fp << i << " ";
    }
    fp << ";\n";
    fp.close();
}
void initBlock() {
    cell_distr.clear();
    for (const auto &[a, b] : net_array) {
        for (const auto &m : b) {
            if (inLeftBlock(m))
                cell_distr[a].first += 1;
            else
                cell_distr[a].second += 1;
        }
    }
}

void partition() {
    int total_gain = 0;
    int max_gain = numeric_limits<int>::min();
    optional<pair<int, string>> a_total_gain, b_total_gain;
    int cell_count = cell_array.size();

    for (int i = 0; i < cell_count; ++i) {
        a_total_gain = bucket_top(bucketA);
        b_total_gain = bucket_top(bucketB);
        pair<int, string> default_val = {numeric_limits<int>::min(), ""};
        pair<int, string> left = default_val, right = default_val;

        if (a_total_gain && A.size() - 1 >= LIMIT_LEFT) {
            left = a_total_gain.value();
        }
        if (b_total_gain && A.size() + 1 <= LIMIT_RIGHT) {
            right = b_total_gain.value();
        }
        // nothing to choose
        if (left.second == right.second) {
            break;
        }

        string select_cell;
        int dgain;
        if (left >= right) {
            select_cell = left.second;
            dgain = left.first;
        } else {
            select_cell = right.second;
            dgain = right.first;
        }
        total_gain += dgain;

        bool inLeft = inLeftBlock(select_cell);

        for (const auto &n : cell_array[select_cell]) {
            int fn = Fn(select_cell, n);
            int tn = Tn(select_cell, n);

            if (tn == 0) {
                for (const auto &m : net_array[n])
                    add_gain(m, 1);
            } else if (tn == 1) {
                for (const auto &m : net_array[n])
                    if ((inLeft != inLeftBlock(m)))
                        add_gain(m, -1);
            }
            --fn;
            ++tn;
            if (fn == 0) {
                for (const auto &m : net_array[n])
                    add_gain(m, -1);
            } else if (fn == 1) {
                for (const auto &m : net_array[n])
                    if (inLeft == inLeftBlock(m))
                        add_gain(m, 1);
            }
        }
        lock_cell(inLeft, select_cell);

        if (total_gain > max_gain && A.size() >= LIMIT_LEFT && A.size() <= LIMIT_RIGHT) {
            if (signbit(total_gain))
                max_gain = total_gain * 0.99;
            else
                max_gain = total_gain * 1.01;
            finalA = A;
            finalB = B;
        }
    }
}
// 6707
int main(int argc, char *argv[]) {
    Timer tmr;
    read_and_preprocess_file(argv[1]);
    print(tmr.elapsed());
    print("limit", LIMIT_LEFT, LIMIT_RIGHT);
    for (const auto &[a, _] : cell_array) {
        A.insert(a);
    }
    initBlock();
    initial_gains();
    partition();
    int cutsize = cut_size(finalA, finalB);
    write_to_file(argv[2], finalA, finalB, cutsize);
    print(tmr.elapsed());
    // exit();
    // while (true) {
    //     A.swap(finalA);
    //     B.swap(finalB);
    //     initBlock();
    //     initial_gains();
    //     partition();
    //     cutsize = cut_size(finalA, finalB);
    //     if (cutsize < mincutsize) {
    //         mincutsize = cutsize;
    //         write_to_file(argv[2], finalA, finalB, mincutsize);
    //     } else {
    //         break;
    //     }
    //     print((clock() - st1) / (double)CLOCKS_PER_SEC);
    // }

    return 0;
}