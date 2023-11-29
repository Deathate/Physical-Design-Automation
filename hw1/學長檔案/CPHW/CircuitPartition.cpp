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

int indexdevice(ifstream &file, map<string, bool> &Groupmap) {
	stringstream ss;
	string s;
	int Cutsize = 0;
	bool Groupflag = false;
    getline(file, s);
    bool netforG1 = false;
    bool netforG2 = false;
	while (getline(file, s)) {
        string currentcheck;
        ss << s;
        while (ss >> currentcheck) {
            if (currentcheck == "NET") {
                ss >> currentcheck;
            }  
            else if (currentcheck == ";") {
                if(netforG2 && netforG1){
                    Cutsize = Cutsize + 1;
                }
                netforG1 = false;
                netforG2 = false;
            }
            else if (currentcheck == " "){

            }
            else {
                if (Groupmap.find(currentcheck) == Groupmap.end()) {
                    if (Groupflag) {
                        netforG2 = true;
                    }
                    else {
                        netforG1 = true;
                    }
                    Groupmap[currentcheck] = Groupflag;
                    Groupflag = !Groupflag;
                }
                else {
                    if (Groupmap[currentcheck]) {
                        netforG2 = true;
                    }
                    else {
                        netforG1 = true;
                    }
                }
            }
        }
		ss.clear();
		ss.str("");
	}
	return Cutsize;
}

int main(int argc, char *argv[])
{
	ifstream file1(argv[1]);
	ofstream file2(argv[2]);
	map<string, bool> Groupmap;
	int Cutsize = indexdevice(file1, Groupmap);
    file2 << "Cutsize = " << Cutsize << endl;
    int inc = Groupmap.size() % 2;
    file2 << "G1 " << (Groupmap.size() / 2) + inc << endl;
    for (const auto& g : Groupmap) {
        if (g.second == false) {
            file2 << g.first << " ";
        }
    }
    file2 << ";" << endl;
    file2 << "G2 " << (Groupmap.size() / 2) << endl;
    for (const auto& g : Groupmap) {
        if (g.second == true) {
            file2 << g.first << " ";
        }
    }
    file2 << ";" << endl;
    file1.close();
    file2.close();
}