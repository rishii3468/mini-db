#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include<unordered_map>
#include<string>
#include "structs.h"
#include "storage.h"

const std::string DB_PATH = "data/database.csv";
search_index_struct search_index;

std::vector<std::string> getHeaders() {
    std::ifstream file(DB_PATH); 
    std::vector<std::string> headers;
    std::string line;
    if (!file) return headers;

    std::getline(file, line);

    if (!line.empty() && line.back() == '\r') line.pop_back();

    std::stringstream ss(line);
    std::string col;
    while (std::getline(ss, col, ',')) {
        if (!col.empty() && col.back() == '\r') col.pop_back();
        if (!col.empty()) headers.push_back(col);
    }

    return headers;
}


Record parseRow(std::string line, const std::vector<std::string>& headers) {
    std::stringstream ss(line);
    std::string value;
    Record r;

    std::vector<std::string> values;

    while (std::getline(ss, value, ',')) {
        values.push_back(value);
    }

    if (values.size() != headers.size()) {
        std::cout << "[ERROR] Corrupted row: " << line << std::endl;
        return r;
    }

    for (int i = 0; i < headers.size(); i++) {
        r.fields[headers[i]] = values[i];
    }

    return r;
}


std::vector<Record> readAll() {
    std::ifstream file(DB_PATH);

    std::vector<Record> records;
    std::string line;

    if (!file) {
        std::cout << "Error opening DB file\n";
        return records;
    }

    auto headers = getHeaders();


    std::getline(file, line);


    while (std::getline(file, line)) {
        Record r = parseRow(line, headers);
        records.push_back(r);
    }

    return records;
}

void createIndex(const std::string& attribute,const bool& first) {
    // Clear the existing index for this attribute
    search_index[attribute].clear();
    
    std::ifstream file(DB_PATH, std::ios::binary);
    if (!file) return;
    auto headers = getHeaders();
    std::string dummy;
    std::getline(file, dummy); // Skip the header line

    std::string line;
    while (true) {
        std::streampos pos = file.tellg(); // This is the start of the current record
        if (!std::getline(file, line)) break;
        
        // Remove line endings in binary mode
        if (!line.empty() && line.back() == '\r') line.pop_back();
        
        if (line.empty()) continue;

        Record r = parseRow(line, headers);
        if (r.fields.count(attribute)) {
            search_index[attribute][r.fields.at(attribute)].push_back(pos);
        }
    }
    if(first) std::cout<<"Index created for attribute "<<attribute<<std::endl;
}

#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <string>

void saveIndexes(search_index_struct& index, const std::string& filename) {
    std::ofstream out(filename, std::ios::binary);
    
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
            for (std::streampos pos : positions) {
                long long p = static_cast<long long>(pos);
                out.write(reinterpret_cast<const char*>(&p), sizeof(p));
            }
        }
    }
}

void loadIndexes(search_index_struct& index, const std::string& filename) {
    std::ifstream in(filename, std::ios::binary);
    index.clear();

    size_t size1;
    in.read(reinterpret_cast<char*>(&size1), sizeof(size1));

    for (size_t i = 0; i < size1; ++i) {
        size_t len;
        in.read(reinterpret_cast<char*>(&len), sizeof(len));
        std::string file(len, ' ');
        in.read(&file[0], len);

        size_t size2;
        in.read(reinterpret_cast<char*>(&size2), sizeof(size2));
        
        auto& inner_map = index[file];
        for (size_t j = 0; j < size2; ++j) {
            size_t word_len;
            in.read(reinterpret_cast<char*>(&word_len), sizeof(word_len));
            std::string word(word_len, ' ');
            in.read(&word[0], word_len);

            size_t vec_size;
            in.read(reinterpret_cast<char*>(&vec_size), sizeof(vec_size));
            std::vector<std::streampos>& positions = inner_map[word];
            positions.reserve(vec_size);

            for (size_t k = 0; k < vec_size; ++k) {
                long long p;
                in.read(reinterpret_cast<char*>(&p), sizeof(p));
                positions.push_back(static_cast<std::streampos>(p));
            }
        }
    }
}
