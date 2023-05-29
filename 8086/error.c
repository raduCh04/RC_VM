#include "utils.h"
#include "error.h"

static const char *SIM_8086_ERROR[] = 
{
	"ERROR_OK",
	"ERROR_INVALID_EXPR",
	"ERROR_OUT_OF_MEMORY",
};

const char *sim_8086_error_str(sim_8086_error error)
{
	ASSERT_MSG(error < ERROR_COUNT, "Not a valid error!\n");
	return (SIM_8086_ERROR[error]);
}

