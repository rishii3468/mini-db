#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "structs.h"

void insertRecord(const Query& query);
void selectRecord(const Query& query);
void deleteRecord(const Query& query);

#endif