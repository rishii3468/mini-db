#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <filesystem>
#include "structs.h"
#include "storage.h"
#include "engine.h"
#include "parser.h"

namespace fs = std::filesystem;


const std::string DB_PATH = "data/database.csv";
const std::string TMP_PATH = "data/database.tmp";



void insertRecord(const Query& query) {

    std::ofstream file(DB_PATH, std::ios::app);

    if (!file) {
        std::cout << "Error opening database file!\n";
        return;
    }

    std::vector<std::string> headers = getHeaders();
    
    bool first = true;
    for (auto& header : headers) {
        if (!first) file << ",";

        // safer: check existence
        if (query.data.count(header)) {
            file << query.data.at(header);
        } else {
            file << ""; // empty if missing
        }

        first = false;
    }

    file << "\n";
  
}

void selectRecord(const Query& query) {
    if (query.data.empty()) {
        std::cout << "SELECT requires at least one condition (e.g., select id=1)\n";
        return;
    }
    
    std::vector<Record> records = readAll();
    auto headers = getHeaders();
    bool found = false;
    if(query.data.size() == 1){
        for(auto& [key,value] : query.data){
            if(search_index.count(key)){
                if(search_index[key].count(value)){
                    std::ifstream file(DB_PATH, std::ios::binary);
                    if (!file) {
                        std::cout << "Error opening file\n";
                        return;
                    }
                    std::vector<std::streampos> positions = search_index[key][value];
                    file.clear();


                    for (auto pos : positions) {
                        file.seekg(pos);
                        std::string line;
                        if (!std::getline(file, line) || line.empty()) continue;
                        
                        // Remove line endings in binary mode
                        if (!line.empty() && line.back() == '\r') line.pop_back();

                        Record r = parseRow(line, headers);

             
                        for (const std::string& h : headers) {
                          
                            if (r.fields.count(h)) {
                                std::cout << h << "=" << r.fields.at(h) << " ";
                            }
                        }
                        std::cout << std::endl;
                    }
                    return;
                   
                }else{
                    std::cout<<"No Records found.\n";
                    return;
                }
            }else{
                
                std::cout << "[DEBUG] Index not found, doing full scan\n";
                
            }
        }
    }
    for (auto& record : records) {
        bool match = true;
        for (const auto& [key, value] : query.data) {
            if (record.fields.count(key) == 0 ||
                record.fields.at(key) != value) {
                match = false;
                break;
            }
        }

        if (match) {
            found = true;

            for (const auto& [key, value] : record.fields) {
                std::cout << key << "=" << value << " ";
            }
            std::cout << "\n";
        }
    }

    if (!found) {
        std::cout << "No records found.\n";
    }
}


void deleteRecord(const Query& query) {
    if (query.data.empty()) {
        std::cout << "DELETE requires conditions\n";
        return;
    }

    std::vector<std::string> headers = getHeaders();
    std::vector<Record> records = readAll();

    std::ofstream file(TMP_PATH);

    if (!file) {
        std::cout << "Error creating temp file!\n";
        return;
    }

    for (size_t i = 0; i < headers.size(); i++) {
        file << headers[i];
        if (i != headers.size() - 1) file << ",";
    }
    file << "\n";

    for (auto& record : records) {
        bool match = true;

        for (const auto& [key, value] : query.data) {
            if (record.fields.count(key) == 0 ||
                record.fields.at(key) != value) {
                match = false;
                break;
            }
        }

        if (!match) {
            for (size_t i = 0; i < headers.size(); i++) {
                if (record.fields.count(headers[i])) {
                    file << record.fields.at(headers[i]);
                } else {
                    file << "";
                }

                if (i != headers.size() - 1) file << ",";
            }
            file << "\n";
        }
    }

    file.close();

    try {
        if (fs::exists(DB_PATH)) {
            fs::remove(DB_PATH);
        }
        fs::rename(TMP_PATH, DB_PATH);
    } catch (const fs::filesystem_error& e) {
        std::cout << "File error: " << e.what() << "\n";
    }

}

void updateRecord(const Query& q){
    if(q.data.empty()){
        std::cout<< "Update command requires conditions.";
        return;
    }
    if(q.newData.empty()){
        std::cout<<"Update command requires new values.";
    }
    auto headers = getHeaders();
    auto records = readAll();

    std::ofstream file(TMP_PATH);

    for(int i=0;i<headers.size();i++){
        file << headers[i];
        if(i != headers.size()-1) file << ",";
    }
    file << "\n";

    for(auto& record : records){
        bool match = false;
        for(auto& [key,value] : q.data){
            if(record.fields.count(key) && record.fields.at(key) == value){
                match = true;
                break;
            }
        }
        if(!match){
            for(int i=0;i<headers.size();i++){
                file << record.fields.at(headers[i]);
                if(i != headers.size()-1) file << ",";
            }
        }else{
            for(int i=0;i<headers.size();i++){
                if(q.newData.count(headers[i])){
                    file << q.newData.at(headers[i]);
                }else{
                    file << record.fields.at(headers[i]);
                }
                if(i != headers.size()-1) file << ",";
            }   
        }
        file <<"\n";
    }

    file.close();

    try {
        if (fs::exists(DB_PATH)) {
            fs::remove(DB_PATH);
        }
        fs::rename(TMP_PATH, DB_PATH);
    } catch (const fs::filesystem_error& e) {
        std::cout << "File error: " << e.what() << "\n";
    }

}