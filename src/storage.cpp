#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <unordered_map>
#include <string>
#include <filesystem>
#include <memory>


#include "structs.h"
#include "storage.h"
#include "btree.h"

namespace fs = std::filesystem;

static fs::path resolveDBPath() {
    auto cwd = fs::current_path();
    std::vector<fs::path> candidates = {
        cwd / "data" / "database.csv",
        cwd / ".." / "data" / "database.csv",
        cwd / ".." / ".." / "data" / "database.csv"
    };
    for (auto const& p : candidates) {
        if (fs::exists(p)) {
            return fs::absolute(p);
        }
    }
    return fs::absolute(candidates.front());
}

fs::path getDBPath() {
    return resolveDBPath();
}

fs::path getTmpPath() {
    return getDBPath().parent_path() / "database.tmp";
}

namespace {
class IndexManager {
private:
    using IndexData = std::unordered_map<std::string, std::vector<std::streampos>>;
    std::unordered_map<std::string, std::unique_ptr<BTreeIndex>> indexes_;
    std::unordered_map<std::string, IndexData> index_data_;

public:
    void createIndex(const std::string& index_name) {
        indexes_[index_name] = std::make_unique<BTreeIndex>();
        index_data_[index_name];
    }

    void insert(const std::string& index_name, const std::string& key, std::streampos pos) {
        auto it = indexes_.find(index_name);
        if (it == indexes_.end()) {
            return;
        }

        it->second->insert(key, pos);
        index_data_[index_name][key].push_back(pos);
    }

    bool hasIndex(const std::string& index_name) const {
        return index_data_.find(index_name) != index_data_.end();
    }

    std::vector<std::streampos> find(const std::string& index_name, const std::string& key) const {
        auto tree_it = indexes_.find(index_name);
        if (tree_it == indexes_.end()) {
            return {};
        }

        auto value = tree_it->second->find(key);
        if (!value.has_value()) {
            return {};
        }

        return value.value();
    }

    std::vector<std::string> getIndexNames() const {
        std::vector<std::string> names;
        names.reserve(index_data_.size());
        for (const auto& [name, _] : index_data_) {
            names.push_back(name);
        }
        return names;
    }

    void saveToFile(const std::string& filename) const {
        std::ofstream out(filename, std::ios::binary);
        if (!out) {
            return;
        }

        size_t index_count = index_data_.size();
        out.write(reinterpret_cast<const char*>(&index_count), sizeof(index_count));

        for (const auto& [attribute, values] : index_data_) {
            size_t attribute_len = attribute.size();
            out.write(reinterpret_cast<const char*>(&attribute_len), sizeof(attribute_len));
            out.write(attribute.data(), attribute_len);

            size_t value_count = values.size();
            out.write(reinterpret_cast<const char*>(&value_count), sizeof(value_count));

            for (const auto& [value, positions] : values) {
                size_t value_len = value.size();
                out.write(reinterpret_cast<const char*>(&value_len), sizeof(value_len));
                out.write(value.data(), value_len);

                size_t positions_count = positions.size();
                out.write(reinterpret_cast<const char*>(&positions_count), sizeof(positions_count));
                for (std::streampos pos : positions) {
                    long long p = static_cast<long long>(pos);
                    out.write(reinterpret_cast<const char*>(&p), sizeof(p));
                }
            }
        }
    }

    void loadFromFile(const std::string& filename) {
        std::ifstream in(filename, std::ios::binary);
        if (!in) {
            return;
        }

        index_data_.clear();
        indexes_.clear();

        size_t index_count = 0;
        in.read(reinterpret_cast<char*>(&index_count), sizeof(index_count));

        for (size_t i = 0; i < index_count; ++i) {
            size_t attribute_len = 0;
            in.read(reinterpret_cast<char*>(&attribute_len), sizeof(attribute_len));
            std::string attribute(attribute_len, ' ');
            in.read(&attribute[0], attribute_len);

            size_t value_count = 0;
            in.read(reinterpret_cast<char*>(&value_count), sizeof(value_count));

            IndexData values;
            for (size_t j = 0; j < value_count; ++j) {
                size_t value_len = 0;
                in.read(reinterpret_cast<char*>(&value_len), sizeof(value_len));
                std::string value(value_len, ' ');
                in.read(&value[0], value_len);

                size_t positions_count = 0;
                in.read(reinterpret_cast<char*>(&positions_count), sizeof(positions_count));

                std::vector<std::streampos> positions;
                positions.reserve(positions_count);
                for (size_t k = 0; k < positions_count; ++k) {
                    long long p = 0;
                    in.read(reinterpret_cast<char*>(&p), sizeof(p));
                    positions.push_back(static_cast<std::streampos>(p));
                }

                values[value] = std::move(positions);
            }

            index_data_[attribute] = std::move(values);
            indexes_[attribute] = std::make_unique<BTreeIndex>();
            for (const auto& [value, positions] : index_data_[attribute]) {
                for (std::streampos pos : positions) {
                    indexes_[attribute]->insert(value, pos);
                }
            }
        }
    }
};

IndexManager& getIndexManager() {
    static IndexManager manager;
    return manager;
}
}

bool hasIndex(const std::string& attribute) {
    return getIndexManager().hasIndex(attribute);
}

std::vector<std::streampos> findIndexedPositions(const std::string& attribute, const std::string& value) {
    return getIndexManager().find(attribute, value);
}

std::vector<std::string> getIndexNames() {
    return getIndexManager().getIndexNames();
}




std::vector<std::string> getHeaders() {
    std::ifstream file(getDBPath()); 
    std::vector<std::string> headers;
    std::string line;
    if (!file) return headers;

    // file.seekg(0);

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
    std::ifstream file(getDBPath());

    std::vector<Record> records;
    std::string line;

    if (!file) {
        std::cout << "Error opening DB file: " << getDBPath().string() << "\n";
        return records;
    }

    auto headers = getHeaders();
    
    // file.seekg(0);

    std::getline(file, line);


    while (std::getline(file, line)) {
        Record r = parseRow(line, headers);
        records.push_back(r);
    }

    return records;
}

void createIndex(const std::string& attribute, const bool& first) {
    auto& indexManager = getIndexManager();
    indexManager.createIndex(attribute);

    std::ifstream file(getDBPath(), std::ios::binary);
    if (!file) {
        std::cout << "Error opening DB file for index creation: " << getDBPath().string() << "\n";
        return;
    }

    auto headers = getHeaders();
    std::string headerLine;
    if (!std::getline(file, headerLine)) {
        return;
    }

    std::string line;
    while (true) {
        std::streampos pos = file.tellg();
        if (!std::getline(file, line)) {
            break;
        }

        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        if (line.empty()) {
            continue;
        }

        Record record = parseRow(line, headers);
        auto it = record.fields.find(attribute);
        if (it != record.fields.end()) {
            indexManager.insert(attribute, it->second, pos);
        }
    }

    if (first) {
        std::cout << "Index created for attribute " << attribute << std::endl;
    }
}

void saveIndexes(const std::string& filename) {
    getIndexManager().saveToFile(filename);
}

void loadIndexes(const std::string& filename) {
    getIndexManager().loadFromFile(filename);
}
