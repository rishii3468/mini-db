#ifndef QUERY_H
#define QUERY_H

#include<map>
#include <string>
#include <map>
#include<iostream>
#include<unordered_map>
#include<vector>
#include<fstream>

struct Query {
    std::string type;
    std::map<std::string, std::string> data;
    std::map<std::string, std::string> newData;
    std::string index_column;
};

struct Record{
    std::map<std::string,std::string> fields;
};

#endif