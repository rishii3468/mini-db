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

using namespace std;
namespace fs = std::filesystem;


const string DB_PATH = "data/database.csv";
const string TMP_PATH = "data/database.tmp";


void insertRecord(const Query& query) {
    ofstream file(DB_PATH, ios::app);

    if (!file) {
        cout << "Error opening database file!\n";
        return;
    }

    vector<string> headers = getHeaders();

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
    vector<Record> records = readAll();
    bool found = false;

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
                cout << key << "=" << value << " ";
            }
            cout << "\n";
        }
    }

    if (!found) {
        cout << "No records match the query.\n";
    }
}


void deleteRecord(const Query& query) {
    if (query.data.empty()) {
        cout << "DELETE requires conditions\n";
        return;
    }

    vector<string> headers = getHeaders();
    vector<Record> records = readAll();

    ofstream file(TMP_PATH);

    if (!file) {
        cout << "Error creating temp file!\n";
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
        cout << "File error: " << e.what() << "\n";
    }
}