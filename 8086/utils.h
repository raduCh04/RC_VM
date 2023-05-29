#pragma once

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#define ASSERT(expr)
#define ASSERT_MSG(expr, ...)
#define LOG_ERROR(...)
#define LOG_ERROR_RETURN(R, ...)
#define ARRAY_LEN(array) (sizeof(array) / sizeof(array[0]))

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long long uint64;

typedef signed char int8;
typedef signed short int16;
typedef signed int int32;
typedef signed long long int64;

typedef float f32;
typedef double f64;

typedef unsigned long long usize;
typedef signed long long ssize;

void secure_memcpy(void *dest, void *src, usize size);
void secure_memset(void *ptr, ssize value, usize size);
void secure_strcpy(char *dest, const char *str, usize dest_size);
void secure_memcmp(const void *ptr1, const void *ptr2, usize size);
void secure_malloc(usize size);
