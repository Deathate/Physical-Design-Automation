#include <iostream>
#include <sstream>
#include <string>
#include <stdio.h>
#include <cstdlib>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <map>

using namespace std;

void indexdevice(ifstream &file, map<string, bool> &Groupmap, map<string, vector<string>> &Netmap) {
	stringstream ss;
	string s;
    getline(file, s);
    vector<string> netcell;
    string Netname;
	while (getline(file, s)) {
        string currentcheck;
        ss << s;
        while (ss >> currentcheck) {
            if (currentcheck == "NET") {
                ss >> currentcheck;
                Netname = currentcheck;
                vector<string> netcell;
            }  
            else if (currentcheck == ";") {
                Netmap[Netname] = netcell;
                netcell.clear();
            }
            else if (currentcheck == " "){

            }
            else {
                if (Groupmap.find(currentcheck) == Groupmap.end()) {
                    Groupmap[currentcheck] = false;
                }
                else {

                }
                netcell.push_back(currentcheck);
            }
        }
		ss.clear();
		ss.str("");
	}
}

void dumpresult(ofstream &outputfile, map<string, bool> Groupmap, int cutsize) {
    outputfile << "Cutsize = " << cutsize << endl;
    int G1_size = 0, G2_size = 0;
    for (const auto& g : Groupmap) {
        if (g.second == false) {
            G1_size = G1_size + 1;
        }
        else {
            G2_size = G2_size + 1;
        }
    }
    outputfile << "G1 " << G1_size << endl;
    for (const auto& g : Groupmap) {
        if (g.second == false) {
            outputfile << g.first << " ";
        }
    }
    outputfile << ";" << endl;
    outputfile << "G2 " << G2_size << endl;
    for (const auto& g : Groupmap) {
        if (g.second == true) {
            outputfile << g.first << " ";
        }
    }
    outputfile << ";" << endl;
}

int cutcircuit(map<string, bool> &Groupmap, map<string, vector<string>> Netmap) {
    int it = 0;
    for (const auto& n : Netmap) {
        vector<string> netcell;
        netcell = n.second;
        for (const auto& net : netcell) {
            if (it <= Groupmap.size()/2) {
                if (Groupmap[net] == false) {
                    Groupmap[net] = true;
                    it = it + 1;
                }
            }
        }

    }

    int cutsize = 0;

    for (const auto& n : Netmap) {
        vector<string> netcell;
        netcell = n.second;
        bool netflag;
        bool first = true;
        bool diffnetflag = false;
        for (const auto& net : netcell) {
            if (first) {
                netflag = Groupmap[net];
                first = false;
            }
            if (Groupmap[net] != netflag) {
                diffnetflag = true;
            }
        }
        if (diffnetflag) {
            cutsize = cutsize + 1;
        }
    }
    return cutsize;
}

int main(int argc, char *argv[])
{
	ifstream file1(argv[1]);
	ofstream file2(argv[2]);
	map<string, bool> Groupmap;
    map<string, vector<string>> Netmap;
	indexdevice(file1, Groupmap, Netmap);
    int Cutsize = cutcircuit(Groupmap, Netmap);
    dumpresult(file2, Groupmap, Cutsize);
    file1.close();
    file2.close();
}