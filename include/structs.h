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
    string index_column;
};

struct Record{
    map<string,string> fields;
};

using search_index_struct = unordered_map<string, unordered_map<string, vector<streampos>>>;
extern search_index_struct search_index;
#endif