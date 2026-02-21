
#pragma once

#include "data_types.h"
#include <stdbool.h>

void wrapper_write(short sectorIndex, void *content);
bool is_directory_accessible(const char* path);
void normalize_path(const char* input_path, char* output_path);
