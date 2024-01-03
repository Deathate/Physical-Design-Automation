#include <algorithm>
#include <boost/range/adaptors.hpp>
#include <fstream>
#include <iostream>
#include <numeric>
#include <ranges>
#include <sstream>
#include <unordered_map>
#include <vector>
#ifdef DEBUG
#include <cpp_utility/print.cpp>
#endif
using namespace std;
struct Pos {
    long int x, y;
};
ostream& operator<<(ostream& os, const Pos& pos) {
    os << "(" << pos.x << ", " << pos.y << ")";
    return os;
}
struct Node {
    string name, in, body, out;
    int width;
};
ostream& operator<<(ostream& os, const Node& node) {
    os << "[" << node.name << " " << node.in << " " << node.body << " " << node.out
       << " " << node.width << "]";
    return os;
}
int main(int argc, char* argv[]) {
#ifdef DEBUG
    Timer tmr;
#endif
    unordered_map<string, vector<Node>> gate_map;
    unordered_map<string, vector<Pos>> pin_position_map;
    string input_file_path = argv[1];
    string output_file_path = argv[2];
    ifstream input_file(input_file_path);
    stringstream ss({istreambuf_iterator<char>(input_file), {}});
    string line;
    getline(ss, line);
    vector<Node> pmos, nmos;
    while (getline(ss, line)) {
        if (line == ".ENDS") break;
        stringstream ss_line(line);
        string name, in, body, out, temp, model, width;
        ss_line >> name >> out >> body >> in >> temp >> model >> width;
        name.erase(0, 1);
        width = width.substr(width.find("=") + 1);
        width.pop_back();
        Node node{name, in, body, out, stoi(width)};
        if (model == "pmos_rvt") {
            pmos.push_back(node);
            gate_map[body].insert(gate_map[body].begin(), node);
        } else {
            nmos.push_back(node);
            gate_map[body].emplace_back(node);
        }
        // print(name, out, body, in, temp, model, width);
    }
    ranges::sort(pmos, {}, &Node::body);
    ranges::sort(nmos, {}, &Node::body);

    string pmos_instances, nmos_instances;
    string pmos_pins, nmos_pins;
    for (const auto& value : pmos | boost::adaptors::indexed(0)) {
        const Node& pmos_node = value.value();
        pmos_instances += pmos_node.name + " Dummy ";
        pmos_pins += pmos_node.in + " " + pmos_node.body + " " + pmos_node.out + " Dummy ";
        pin_position_map[pmos_node.in].push_back({value.index() * 6, 0});
        pin_position_map[pmos_node.out].push_back({value.index() * 6 + 2, 0});
    }
    for (const auto& value : nmos | boost::adaptors::indexed(0)) {
        const Node& nmos_node = value.value();
        nmos_instances += nmos_node.name + " Dummy ";
        nmos_pins += nmos_node.in + " " + nmos_node.body + " " + nmos_node.out + " Dummy ";
        pin_position_map[nmos_node.in].push_back({value.index() * 6, 1});
        pin_position_map[nmos_node.out].push_back({value.index() * 6 + 2, 1});
    }
    string_view pmos_instances_view{pmos_instances.begin(), pmos_instances.end() - 6}, nmos_instances_view{nmos_instances.begin(), nmos_instances.end() - 6};
    string_view pmos_pins_view{pmos_pins.begin(), pmos_pins.end() - 6}, nmos_pins_view{nmos_pins.begin(), nmos_pins.end() - 6};

    int pmos_height = pmos[0].width;
    int nmos_height = nmos[0].width;
    vector<int> rooms(pmos.size() * 6);
    int i = 0;
    for (int _ = 0; _ < pmos.size(); ++_) {
        rooms[i] = 34;
        rooms[i + 1] = 20;
        rooms[i + 2] = 34;
        rooms[i + 3] = 20;
        rooms[i + 4] = 34;
        rooms[i + 5] = 20;
        i += 6;
    }
    rooms[0] = 25;
    rooms[rooms.size() - 4] = 25;
#ifdef DEBUG
    print("rooms:", rooms.size() - 3);
    auto s = (rooms | ranges::views::take(rooms.size() - 3));
    print(vector<int>{s.begin(), s.end()});

#endif
    int HPWL{0};
    for (auto& [pin, positions] : pin_position_map) {
        double length{0};
        const auto [minx, maxx] = ranges::minmax_element(positions, {}, &Pos::x);
        const auto [miny, maxy] = ranges::minmax_element(positions, {}, &Pos::y);
        if (minx->x != maxx->x) {
            length += accumulate(rooms.begin() + minx->x, rooms.begin() + maxx->x + 1, 0);
        }
        if (miny->y != maxy->y) {
            length += pmos_height / 2.0 + nmos_height / 2.0 + 27;
        }
        if (length != 0 && minx->x != maxx->x)
            length -= rooms[minx->x] / 2.0 + rooms[maxx->x] / 2.0;
#ifdef DEBUG
        print(pin, *minx, *maxx, *miny, *maxy);
        print("length", length);
#endif
        HPWL += length;
    }

#ifdef DEBUG
    print("--------------------");
    print("HPWL");
    print(HPWL);
    print("pmos");
    print(pmos_instances_view);
    print(pmos_pins_view);
    print("nmos");
    print(nmos_instances_view);
    print(nmos_pins_view);
    print("pin_position_map");
    print(pin_position_map);
    print("time");
    print(tmr.elapsed());
#endif
    ofstream output_file(output_file_path);
    output_file << HPWL << endl;
    output_file << pmos_instances_view << endl;
    output_file << pmos_pins_view << endl;
    output_file << nmos_instances_view << endl;
    output_file << nmos_pins_view << endl;
    // print(pmos_pins);
    // print(nmos_pins);
    return 0;
}
