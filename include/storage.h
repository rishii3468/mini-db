#ifndef STORAGE_H
#define STORAGE_H

#include <vector>
#include <string>
#include <map>
#include <filesystem>
#include "structs.h"


std::vector<std::string> getHeaders();

Record parseRow(std::string line, const std::vector<std::string>& headers);

std::vector<Record> readAll();

void createIndex(const std::string& attribute,const bool& first=false);
bool hasIndex(const std::string& attribute);
std::vector<std::streampos> findIndexedPositions(const std::string& attribute, const std::string& value);
std::vector<std::string> getIndexNames();

void saveIndexes(const std::string& filename);

void loadIndexes(const std::string& filename);

std::filesystem::path getDBPath();
std::filesystem::path getTmpPath();

#endif