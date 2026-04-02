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
search_index_struct search_index;

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

void createIndex(const string& attribute,const bool& first) {
    if(search_index.count(attribute)){
        cout<<"Index already exists."<<endl;
        return;
    }
    ifstream file(DB_PATH, ios::binary);
    if (!file) return;
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
    if(first) cout<<"Index created for attribute "<<attribute<<endl;
}

#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <string>

using namespace std;
typedef unordered_map<string, unordered_map<string, vector<streampos>>> SearchIndex;

void saveIndexes(search_index_struct& index, const string& filename) {
    ofstream out(filename, ios::binary);
    
    size_t size1 = index.size();
    out.write(reinterpret_cast<const char*>(&size1), sizeof(size1));

    for (const auto& [file, inner_map] : index) {
        size_t len = file.size();
        out.write(reinterpret_cast<const char*>(&len), sizeof(len));
        out.write(file.data(), len);

        size_t size2 = inner_map.size();
        out.write(reinterpret_cast<const char*>(&size2), sizeof(size2));

        for (const auto& [word, positions] : inner_map) {
            size_t word_len = word.size();
            out.write(reinterpret_cast<const char*>(&word_len), sizeof(word_len));
            out.write(word.data(), word_len);

            size_t vec_size = positions.size();
            out.write(reinterpret_cast<const char*>(&vec_size), sizeof(vec_size));
            for (streampos pos : positions) {
                long long p = static_cast<long long>(pos);
                out.write(reinterpret_cast<const char*>(&p), sizeof(p));
            }
        }
    }
}

void loadIndexes(search_index_struct& index, const string& filename) {
    ifstream in(filename, ios::binary);
    index.clear();

    size_t size1;
    in.read(reinterpret_cast<char*>(&size1), sizeof(size1));

    for (size_t i = 0; i < size1; ++i) {
        size_t len;
        in.read(reinterpret_cast<char*>(&len), sizeof(len));
        string file(len, ' ');
        in.read(&file[0], len);

        size_t size2;
        in.read(reinterpret_cast<char*>(&size2), sizeof(size2));
        
        auto& inner_map = index[file];
        for (size_t j = 0; j < size2; ++j) {
            size_t word_len;
            in.read(reinterpret_cast<char*>(&word_len), sizeof(word_len));
            string word(word_len, ' ');
            in.read(&word[0], word_len);

            size_t vec_size;
            in.read(reinterpret_cast<char*>(&vec_size), sizeof(vec_size));
            vector<streampos>& positions = inner_map[word];
            positions.reserve(vec_size);

            for (size_t k = 0; k < vec_size; ++k) {
                long long p;
                in.read(reinterpret_cast<char*>(&p), sizeof(p));
                positions.push_back(static_cast<streampos>(p));
            }
        }
    }
}
