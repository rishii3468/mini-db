#ifndef STORAGE_H
#define STORAGE_H

#include <vector>
#include <string>
#include <map>
#include "structs.h"


std::vector<std::string> getHeaders();

Record parseRow(std::string line, const std::vector<std::string>& headers);

std::vector<Record> readAll();

void createIndex(const std::string& attribute,const bool& first=false);

void saveIndexes(search_index_struct& index, const std::string& filename);

void loadIndexes(search_index_struct& index, const std::string& filename);

#endif