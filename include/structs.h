#ifndef QUERY_H
#define QUERY_H

#include<map>
#include <string>
#include <map>
#include<iostream>
#include<unordered_map>
using namespace std;

struct Query {
    string type;
    map<string, string> data;
    map<string, string> newData;
};

struct Record{
    map<string,string> fields;
};

extern unordered_map<string, vector<int>> index;
#endif