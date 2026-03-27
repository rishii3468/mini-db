#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include "structs.h"
#include "storage.h"

using namespace std;

const string DB_PATH = "data/database.csv";


vector<string> getHeaders() {
    ifstream file(DB_PATH);

    vector<string> headers;
    string line;

    if (!file) {
        cout << "Error opening DB file\n";
        return headers;
    }

    getline(file, line);

    stringstream ss(line);
    string col;

    while (getline(ss, col, ',')) {
        headers.push_back(col);
    }

    return headers;
}


Record parseRow(string line, const vector<string>& headers) {
    stringstream ss(line);
    string value;
    Record r;

    int i = 0;
    while (getline(ss, value, ',')) {
        if (i < headers.size()) {
            r.fields[headers[i]] = value;
        }
        i++;
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