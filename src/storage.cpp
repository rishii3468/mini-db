#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include<unordered_map>
#include<string>
#include "structs.h"
#include "storage.h"

using namespace std;

const string DB_PATH = "data/database.csv";
unordered_map<string, unordered_map<string, vector<streampos>>> search_index;


vector<string> getHeaders() {
    ifstream file(DB_PATH); 
    vector<string> headers;
    string line;
    if (!file) return headers;

    getline(file, line);

    if (!line.empty() && line.back() == '\r') line.pop_back();

    stringstream ss(line);
    string col;
    while (getline(ss, col, ',')) {
        if (!col.empty() && col.back() == '\r') col.pop_back();
        if (!col.empty()) headers.push_back(col);
    }

    return headers;
}
// vector<string> getHeaders() {
//     ifstream file(DB_PATH);
//     vector<string> headers;
//     string line;
//     if (!file) return headers;

//     getline(file, line);

//     if (!line.empty() && line.back() == '\r') line.pop_back();

//     stringstream ss(line);
//     string col;
//     while (getline(ss, col, ',')) {
//         if (!col.empty() && col.back() == '\r') col.pop_back();
//         if (!col.empty()) headers.push_back(col);
//     }

//     return headers;
// }


Record parseRow(string line, const vector<string>& headers) {
    stringstream ss(line);
    string value;
    Record r;

    vector<string> values;

    while (getline(ss, value, ',')) {
        values.push_back(value);
    }

    if (values.size() != headers.size()) {
        cout << "[ERROR] Corrupted row: " << line << endl;
        return r;
    }

    for (int i = 0; i < headers.size(); i++) {
        r.fields[headers[i]] = values[i];
    }

    return r;
}


vector<Record> readAll() {
    ifstream file(DB_PATH);

    vector<Record> records;
    string line;

    if (!file) {
        cout << "Error opening DB file\n";
        return records;
    }

    auto headers = getHeaders();


    getline(file, line);


    while (getline(file, line)) {
        Record r = parseRow(line, headers);
        records.push_back(r);
    }

    return records;
}

void createIndex(const string& attribute) {
    ifstream file(DB_PATH, ios::binary);
    if (!file) return;

    search_index.clear();
    auto headers = getHeaders();
    string dummy;
    getline(file, dummy); // Skip the header line

    string line;
    while (true) {
        streampos pos = file.tellg(); // This is the start of the current record
        if (!getline(file, line)) break;
        
        // Remove line endings in binary mode
        if (!line.empty() && line.back() == '\r') line.pop_back();
        
        if (line.empty()) continue;

        Record r = parseRow(line, headers);
        if (r.fields.count(attribute)) {
            search_index[attribute][r.fields.at(attribute)].push_back(pos);
        }
    }
}