#include "io.h"
#include <stdio.h>

uint8 *file_read_binary(const char *path, usize *size)
{
	FILE *file = fopen(path, "rb");
	fseek(file, 0, SEEK_END);
	usize len = ftell(file);
	fseek(file, 0, SEEK_SET);
	uint8 *content = (uint8 *)malloc(len);
	fread(content, len, 1, file);
	fclose(file);
	*size = len;
	return (content);
}

void file_write_binary(const char *path, uint8 *data, usize size)
{
	FILE *file = fopen(path, "wb");
	fwrite(data, sizeof(uint8), size, file);
	fclose(file);
}
