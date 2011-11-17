#include "simulate.h"
#include "state.h"
#include "utility.h"
#include <iostream>
using namespace std;

unsigned int ExtractInput (index_t bitString, vector<int>* targetMap);

int main(int argc, char* argv[])
{
    if (argc != 2) return 0;
    string in = argv[1];
    index_t bit = 10;
    vector<int> *tMap = new vector<int>;
    tMap->push_back(2);
    tMap->push_back(1);
    cout << "input Bit: ";
    printIntBin(bit);
    cout << endl << "output:";
    cout << endl;
}

