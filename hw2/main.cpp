
#include <algorithm>
#include <fstream>
#include <iostream>
#include <list>
#include <random>
#include <ranges>
#include <sstream>
#ifdef DEBUG
#include "cpp_utility/print.cpp"
#endif
using namespace std;
enum CutType { VERTICAL,
               HORIZONTAL,
               BLOCK,
               NOTHING };
// Binary Tree Node
struct Node {
    string name;
    list<pair<pair<int, int>, pair<int, int>>> meta_shape;
    bool rotated = false;
    CutType type = BLOCK;
    Node *left = nullptr;
    Node *right = nullptr;
    Node *parent = nullptr;
    int width = 0, height = 0;
    int x = 0, y = 0;
    int best_idx = 0;
    int changed = true;

    Node(string name, int width, int height) : name(name), width(width), height(height) {
        meta_shape.emplace_back(make_pair(width, height), make_pair(-1, -1));
        meta_shape.emplace_back(make_pair(height, width), make_pair(-1, -1));
    }
    Node(CutType type) : type(type) {}
    // Overload << operator to customize cout for Node
    friend std::ostream &operator<<(std::ostream &os, const Node &node) {
        if (node.type == BLOCK) {
            os << node.name;
            if (node.rotated)
                os << "R";
        } else
            os << (node.type == VERTICAL ? "V" : "H");
        return os;
    }
    Node *insert(Node &value, bool left) {
        if (left) {
            this->left = &value;
            this->left->parent = this;
        } else {
            this->right = &value;
            this->right->parent = this;
        }
        return &value;
    }
    // 可以只用一邊來判斷
    bool isLeaf() { return this->left == nullptr && this->right == nullptr; }
    bool isVertical() { return this->type == VERTICAL; }
    bool isHorizontal() { return this->type == HORIZONTAL; }
    pair<pair<int, int>, pair<int, int>> const &getOptimalShape() {
        return *next(this->meta_shape.begin(), this->best_idx);
    }
    pair<int, int> const &getWidthNHeight() {
        return getOptimalShape().first;
    }
    float getArea() {
        auto [w, h] = getOptimalShape().first;
        return static_cast<float>(w * h);
    }
    float getRatio() {
        auto [w, h] = getOptimalShape().first;
        return static_cast<float>(w) / h;
    }
};
// ostream &operator<<(ostream &out, const Node *v) {
//     out << *v;
//     return out;
// }
void removeRedundantShape(auto &v) {
    v.sort();
    for (auto it = v.begin(); it != v.end(); ++it) {
        for (auto jt = next(it); jt != v.end();) {
            if (it->first.first <= jt->first.first && it->first.second <= jt->first.second) {
                auto tmp = next(jt);
                v.erase(jt);
                jt = tmp;
            } else {
                ++jt;
            }
        }
    }
}
void postOrderTraversal(Node *root, vector<Node *> &search_sequence) {
    if (root != nullptr) {
        postOrderTraversal(root->left, search_sequence);
        postOrderTraversal(root->right, search_sequence);
        search_sequence.emplace_back(root);
    }
}
vector<Node *> postOrderTraversal(Node *root) {
    vector<Node *> search_sequence;
    postOrderTraversal(root, search_sequence);
    return search_sequence;
}

void inOrderTraversal(Node *root, vector<Node *> &search_sequence) {
    if (root != nullptr) {
        inOrderTraversal(root->left, search_sequence);
        search_sequence.emplace_back(root);
        inOrderTraversal(root->right, search_sequence);
    }
}
vector<Node *> inOrderTraversal(Node *root) {
    vector<Node *> search_sequence;
    inOrderTraversal(root, search_sequence);
    return search_sequence;
}
mt19937 gen(42);
vector<Node> generatePolishExpression(unordered_map<string, Node> &nodes) {
    auto block_names_view = views::keys(nodes);
    vector<string> block_names{block_names_view.begin(), block_names_view.end()};
    vector<Node> polish_expression;
    int n = block_names.size();
    polish_expression.reserve(2 * n - 1);
    int operand_num = 0;
    int operator_num = 0;
    uniform_real_distribution<float> distribution(0, 1);
    string last_operand;
    for (int i = 0; i < 2 * n - 1; i++) {
        bool must_choose_operand = operand_num <= operator_num + 1;
        bool must_choose_operator = operand_num == n;
        if (!must_choose_operand && distribution(gen) > 0.5 || must_choose_operator) {
            string cut = distribution(gen) > 0.5 ? "V" : "H";
            if (last_operand == "V") {
                cut = "H";
            } else if (last_operand == "H") {
                cut = "V";
            }
            polish_expression.emplace_back(cut == "V" ? VERTICAL : HORIZONTAL);
            ++operator_num;
            last_operand = cut;
        } else {
            uniform_int_distribution<int> distribution(0, block_names.size() - 1);
            int operand_idx = distribution(gen);
            string operand = block_names[operand_idx];
            polish_expression.emplace_back(nodes.at(operand));
            block_names[operand_idx] = block_names.back();
            block_names.pop_back();
            ++operand_num;
            last_operand = operand;
        }
    }
    return polish_expression;
}

Node &buildTreeHelper(vector<Node> &postorder, int &pos) {
    Node &root = postorder[postorder.size() - 1 - pos];
    ++pos;
    if (root.type != BLOCK) {
        root.insert(buildTreeHelper(postorder, pos), false);
        root.insert(buildTreeHelper(postorder, pos), true);
    }
    return root;
}
Node &buildTreeHelper(vector<Node> &postorder) {
    int i = 0;
    buildTreeHelper(postorder, i);
    return postorder.back();
}
Node *leftMostBlock(Node *root) {
    if (root->left)
        return leftMostBlock(root->left);
    else
        return root;
}
void constructFloorPalnning(Node *root) {
    if (!root->isLeaf()) {
        constructFloorPalnning(root->left);
        constructFloorPalnning(root->right);
        auto verticalCombine = [](int v, int u1, int w, int u2) {
            int p1x = v + w;
            int p1y = max(u1, u2);
            return make_pair(p1x, p1y);
        };
        auto horizontalCombine = [](int v, int u1, int w, int u2) {
            int p1x = max(v, w);
            int p1y = u1 + u2;
            return make_pair(p1x, p1y);
        };
        // print("calculate", root->changed, root->left->changed, root->right->changed);
        // print(root, root->left, root->right);
        if (root->changed || root->left->changed || root->right->changed) {
            root->meta_shape.clear();
            if (root->isVertical()) {
                int i = 0;
                for (auto &[w1, h1] : root->left->meta_shape | views::keys) {
                    int j = 0;
                    for (auto &[w2, h2] : root->right->meta_shape | views::keys) {
                        root->meta_shape.emplace_back(verticalCombine(w1, h1, w2, h2), make_pair(i, j));
                        ++j;
                    }
                    ++i;
                }
            } else if (root->isHorizontal()) {
                int i = 0;
                for (auto &[w1, h1] : root->left->meta_shape | views::keys) {
                    int j = 0;
                    for (auto &[w2, h2] : root->right->meta_shape | views::keys) {
                        root->meta_shape.emplace_back(horizontalCombine(w1, h1, w2, h2), make_pair(i, j));
                        ++j;
                    }
                    ++i;
                }
            }
            removeRedundantShape(root->meta_shape);
            root->left->changed = false;
            root->right->changed = false;
            root->changed = true;
        }
    }
}
void selectShape(Node *tree) {
    if (tree->isLeaf()) {
        if (tree->best_idx == 1) {
            tree->rotated = true;
        } else {
            tree->rotated = false;
        }
    } else {
        const auto &[shape, index] = tree->getOptimalShape();
        tree->width = shape.first;
        tree->height = shape.second;
        tree->left->best_idx = index.first;
        tree->right->best_idx = index.second;
        selectShape(tree->left);
        selectShape(tree->right);
    }
}
void retrieveCoordinate(Node *tree, int current_x = 0, int current_y = 0) {
    selectShape(tree);
    auto in_order_sequence = inOrderTraversal(tree);
    for (const auto &node : in_order_sequence) {
        if (node->isLeaf()) {
            node->x = current_x;
            node->y = current_y;
        } else {
            current_x = node->x = node->left->x;
            current_y = node->y = node->left->y;
            auto &[w, h] = node->left->getWidthNHeight();
            if (node->isVertical())
                current_x += w;
            else
                current_y += h;
        }
    }
}
int calculate(Node *tree) {
    constructFloorPalnning(tree);
    tree->changed = false;
    auto best_choice = ranges::min_element(tree->meta_shape, {}, [](const auto &a) {
        return a.first.first * a.first.second;
    });
    tree->best_idx = distance(tree->meta_shape.begin(), best_choice);
    return tree->getArea();
}
int disturb(vector<Node> &post_order_seq) {
    int m = uniform_int_distribution<int>(0, 2)(gen);
    // M1
    if (m == 0) {
        // print("M1");
        auto cut_elements = post_order_seq | views::filter([](const auto &v) { return v.type == BLOCK; });
        int N = ranges::distance(cut_elements);
        int choice1 = uniform_int_distribution<int>(0, N - 2)(gen);
        int choice2 = uniform_int_distribution<int>(0, N - 1)(gen);
        if (choice1 == choice2)
            choice2 = N - 1;

        auto &b1 = *ranges::next(cut_elements.begin(), choice1);
        auto &b2 = *ranges::next(cut_elements.begin(), choice2);
        b1.changed = true;
        b2.changed = true;
        swap(b1, b2);
    }
    // M2
    else if (m == 1) {
        // print("M2");
        auto cut_elements = post_order_seq | views::filter([](const auto &v) { return v.type != BLOCK; });
        int choice = uniform_int_distribution<int>(0, ranges::distance(cut_elements.begin(), cut_elements.end()) - 1)(gen);
        auto b1 = *ranges::next(cut_elements.begin(), choice);
        b1.type = b1.type == VERTICAL ? HORIZONTAL : VERTICAL;
        b1.changed = true;
    }
    // M3
    else {
        // print("M3");
        int i = 0;
        while (++i < 20) {
            int choice = uniform_int_distribution<int>(0, post_order_seq.size() - 2)(gen);
            auto &b1 = post_order_seq[choice];
            auto &b2 = post_order_seq[choice + 1];
            if ((b1.type == BLOCK && b2.type != BLOCK) || (b1.type != BLOCK && b2.type == BLOCK)) {
                int i = ranges::count_if(post_order_seq | views::take(choice), [](const auto &v) { return v.type == BLOCK; });
                int j = ranges::count_if(post_order_seq | views::take(choice), [](const auto &v) { return v.type != BLOCK; });
                if (i - j > 1) {
                    b1.parent->changed = true;
                    b2.parent->changed = true;
                    b1.changed = true;
                    b2.changed = true;
                    swap(b1, b2);
                    buildTreeHelper(post_order_seq);
                    break;
                }
            }
        }
    }
    return m;
}
void output(string file_path, Node *tree) {
    retrieveCoordinate(tree);
    ofstream output_file(file_path);
    output_file << "A = " << tree->getArea() << "\n";
    output_file << "R = " << tree->getRatio() << "\n";
    auto in_order_sequence = inOrderTraversal(tree);
    for (const auto &node : in_order_sequence) {
        if (node->isLeaf()) {
            output_file << node->name << " " << node->x << " " << node->y << " ";
            if (node->rotated)
                output_file << "R";
            output_file << "\n";
        }
    }
}

int main(int argc, char const *argv[]) {
#ifdef DEBUG
    Timer tmr;
#endif
    string input_file_path = argv[1];
    string output_file_path = argv[2];
    ifstream input_file(input_file_path);
    stringstream ss({istreambuf_iterator<char>{input_file}, {}});
    float rl, ru;  // r_lowerbound, r_upperbound
    vector<pair<string, array<int, 2>>> blocks;
    unordered_map<string, Node> nodes;
    string line;
    int total_area = 0;
    bool fist_line = true;
    while (getline(ss, line)) {
        string block_name;
        int block_w, block_h;
        stringstream ss_line(line);
        if (fist_line) {
            ss_line >> rl >> ru;
            fist_line = false;
        } else {
            ss_line >> block_name >> block_w >> block_h;
            nodes.insert({block_name, Node(block_name, block_w, block_h)});
            total_area += block_w * block_h;
            blocks.emplace_back(block_name, array<int, 2>{block_w, block_h});
        }
    }

    int roughly_width = static_cast<int>(sqrt(total_area * ru));
    ranges::sort(blocks, ranges::greater(), [](const auto &v) { return max(v.second[0], v.second[1]); });
    vector<Node> polish_expression;
    polish_expression.reserve(2 * blocks.size() - 1);
    int operator_num = 0;
    int i = 0;
    int k = 0;
    for (const auto &block : blocks) {
        const auto &name = block.first;
        const auto &w = block.second[0];
        const auto &h = block.second[1];
        polish_expression.emplace_back(nodes.at(name));
        i += max(w, h);
        k += 1;
        if (i > roughly_width) {
            for (int j = 0; j < k - 1; ++j)
                polish_expression.emplace_back(VERTICAL);
            i = k = 0;
        }
    }

    for (int i = k - 1; i > 0; --i) {
        polish_expression.emplace_back(VERTICAL);
    }
    for (int i = 2 * blocks.size() - 1 - polish_expression.size(); i > 0; --i) {
        polish_expression.emplace_back(HORIZONTAL);
    }

    buildTreeHelper(polish_expression);
    int cost_e = calculate(&polish_expression.back());
    output(output_file_path, &polish_expression.back());
#ifdef DEBUG
    print("White space:", total_area / polish_expression.back().getArea());
    print("time:", tmr.elapsed());
#endif

    // // Simulated Annealing
    // vector<Node> s_old = polish_expression;
    // int lowest_area = cost_e;
    // double T = 1;
    // int MT = 0;
    // int reject = 0;
    // int uphill = 0;
    // int N = polish_expression.size() * 5;
    // output(output_file_path, &polish_expression.back());
    // int r = 0;
    // do {
    //     MT = uphill = reject = 0;
    //     while (uphill < N && MT < 2 * N) {
    //         ++r;
    //         MT += 1;
    //         s_old = polish_expression;
    //         int m = disturb(polish_expression);
    //         int cost_ne = calculate(&polish_expression.back());
    //         int cost_delta = cost_ne - cost_e;
    //         if (cost_delta <= 0) {
    //             cost_e = cost_ne;
    //             if (cost_ne < lowest_area) {
    //                 lowest_area = cost_ne;
    //                 print(r, polish_expression.back().getArea());
    //                 // print(polish_expression.back().getOptimalShape());
    //                 output(output_file_path, &polish_expression.back());
    //             } else {
    //                 reject += 1;
    //             }
    //         } else {
    //             float p = exp(-cost_delta / T);
    //             float r = uniform_real_distribution<float>(0, 1)(gen);
    //             if (r < p) {
    //                 cost_e = cost_ne;
    //                 uphill += 1;
    //                 reject += 1;
    //             } else {
    //                 polish_expression = s_old;
    //             }
    //         }
    //     }
    //     T *= 0.85;
    // } while (((1.0 * reject / MT) < 0.95) && T > 0.1);
    // print("time:", tmr.elapsed(), r);
    return 0;
}
