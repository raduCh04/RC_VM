#pragma once

#include "utils.h"

uint8 *file_read_binary(const char *path, usize *size);
uint8 *file_read_ascii(const char *path, usize *size);
void file_write_binary(const char *path, uint8 *data, usize size);
void file_write_ascii(const char *path, uint8 *data); 
