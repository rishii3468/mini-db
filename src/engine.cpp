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

std::vector<std::string> getHeaders();

std::vector<Record> readAll();

Record parseRow(std::string line, const std::vector<std::string>& headers);

void insertRecord(const Query& query) {

    std::ofstream file(getDBPath(), std::ios::app);

    if (!file) {
        std::cout << "Error opening database file: " << getDBPath().string() << "\n";
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
            if(hasIndex(key)){
                std::cout<<"[DEBUG]: Using BTrees."<<std::endl;
                auto positions = findIndexedPositions(key, value);
                if (!positions.empty()){
                    std::ifstream file(getDBPath(), std::ios::binary);
                    if (!file) {
                        std::cout << "Error opening file: " << getDBPath().string() << "\n";
                        return;
                    }
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



// void deleteRecord(const Query& query) {
//     if (query.data.empty()) {
//         std::cout << "DELETE requires conditions\n";
//         return;
//     }

//     // 1. Fetch data and completely let go of the original file
//     std::vector<std::string> headers = getHeaders();
//     std::vector<Record> records = readAll();

//     // 2. Write to temp file inside an isolated scope
//     {
//         std::ofstream tempFile(getTmpPath());

//         if (!tempFile) {
//             std::cout << "Error creating temp file: " << getTmpPath().string() << "\n";
//             return;
//         }

//         // Write headers
//         for (size_t i = 0; i < headers.size(); i++) {
//             tempFile << headers[i];
//             if (i != headers.size() - 1) tempFile << ",";
//         }
//         tempFile << "\n";

//         // Write non-matching records
//         for (auto& record : records) {
//             bool match = true;

//             for (const auto& [key, value] : query.data) {
//                 if (record.fields.count(key) == 0 || record.fields.at(key) != value) {
//                     match = false;
//                     break;
//                 }
//             }

//             if (!match) {
//                 for (size_t i = 0; i < headers.size(); i++) {
//                     if (record.fields.count(headers[i])) {
//                         tempFile << record.fields.at(headers[i]);
//                     } // Empty string implicitly handled by commas
                    
//                     if (i != headers.size() - 1) tempFile << ",";
//                 }
//                 tempFile << "\n";
//             }
//         }
//     } 

//     try {
//         fs::path targetDb = getDBPath();
//         fs::path tempDb = getTmpPath();


//         if (fs::exists(targetDb)) {
//             fs::remove(targetDb);
//         }
        
//         fs::rename(tempDb, targetDb);
//         std::cout << "Record deleted successfully.\n";
//     } catch (const fs::filesystem_error& e) {
//         std::cout << "File error: " << e.what() << "\n";
//     }
// }


void deleteRecord(const Query& query) {
    if (query.data.empty()) {
        std::cout << "DELETE requires conditions\n";
        return;
    }


    std::vector<std::string> headers = getHeaders();

    fs::path dbPath = getDBPath();
    fs::path tmpPath = getTmpPath();

    std::ifstream srcFile(dbPath);
    if (!srcFile) {
        std::cout << "Error opening database file: " << dbPath.string() << "\n";
        return;
    }

    std::ofstream tmpFile(tmpPath);
    if (!tmpFile) {
        std::cout << "Error creating temp file: " << tmpPath.string() << "\n";
        return;
    }

    std::string headerLine;
    if (!std::getline(srcFile, headerLine)) {
        return; 
    }
    tmpFile << headerLine << "\n";


    std::string line;
    while (std::getline(srcFile, line)) {
        if (line.empty()) continue;

        Record record = parseRow(line, headers);

        
        bool match = true;
        for (const auto& [key, value] : query.data) {
            auto it = record.fields.find(key);
            if (it == record.fields.end() || it->second != value) {
                match = false;
                break;
            }
        }

        
        if (!match) {
            tmpFile << line << "\n";
        }
    }

    
    srcFile.close();
    tmpFile.close();


    try {
        if (fs::exists(dbPath)) {
            fs::remove(dbPath); 
        }
        fs::rename(tmpPath, dbPath);
        std::cout << "Delete operation completed successfully.\n";
    } catch (const fs::filesystem_error& e) {
        std::cout << "File replacement error: " << e.what() << "\n";
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

    std::ofstream file(getTmpPath());

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
        if (fs::exists(getDBPath())) {
            fs::remove(getDBPath());
        }
        fs::rename(getTmpPath(), getDBPath());
    } catch (const fs::filesystem_error& e) {
        std::cout << "File error: " << e.what() << "\n";
    }

}