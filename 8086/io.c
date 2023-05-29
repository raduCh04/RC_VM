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
