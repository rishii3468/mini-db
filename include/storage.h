#ifndef STORAGE_H
#define STORAGE_H

#include <vector>
#include <string>
#include <map>
#include "structs.h"


std::vector<std::string> getHeaders();

Record parseRow(std::string line, const std::vector<std::string>& headers);

std::vector<Record> readAll();

#endif