#ifndef QUERY_H
#define QUERY_H

#include<map>
#include <string>
#include <map>
#include<iostream>
using namespace std;

struct Query {
    string type;
    map<string, string> data;
};

struct Record{
    map<string,string> fields;
};

#endif