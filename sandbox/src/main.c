#include "../../rcvm/src/rcvm.h"
#include "time.h"
int main()
{
	usize size;
	uint8 *data = file_read_binary("test", &size);
	rcvm_t *vm = rcvm_init();
	rcvm_load_program_data(vm, size, data);
	clock_t start = clock();
	rcvm_execute_program(vm, size);
	clock_t end = clock();
	rcvm_print_regs(vm);
	rcvm_print_memory_from_to(vm, 0, 0x20);
	f64 time_taken = (f64)(end - start)/CLOCKS_PER_SEC;
	printf(";Time taken: %f s\n", time_taken);
}
