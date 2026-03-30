#ifndef QUERY_H
#define QUERY_H

#include<map>
#include <string>
#include <map>
#include<iostream>
#include<unordered_map>
#include<vector>
#include<fstream>
using namespace std;

struct Query {
    string type;
    map<string, string> data;
    map<string, string> newData;
};

struct Record{
    map<string,string> fields;
};

extern unordered_map<string, unordered_map<string, vector<streampos>>> search_index;
#endif