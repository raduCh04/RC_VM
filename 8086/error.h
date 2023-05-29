#include <stdio.h>

/// @brief Error codes for 8086 Simulator
typedef enum sim_8086_error 
{
	ERROR_OK = 0,
	ERROR_INVALID_EXPR,
	ERROR_OUT_OF_MEMORY,
	ERROR_COUNT,
} sim_8086_error;

const char *sim_8086_error_str(sim_8086_error error);


