#ifndef __INV_ARGV_H
#define __INV_ARGV_H

#include <map>
#include <vector>
#include <string>
#include <sstream>
using namespace std;

namespace inv
{

class INV_Argv
{
public:

    INV_Argv(){};

    void decode(int argc, char *argv[]);

    bool hasParam(const string &sName);

    string getValue(const string &sName);

    map<string, string>& getMulti();

    vector<string>& getSingle();

protected:

    void parse(const string &s);

protected:
    
    map<string, string> _mParam;

    vector<string>      _vSingle;
};

}

#endif

