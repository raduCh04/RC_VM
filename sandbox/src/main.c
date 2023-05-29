#include "../../rcvm/src/rcvm.h"

int main()
{
	usize size;
	uint8 *data = file_read_binary("test", &size);
	rcvm_t *vm = rcvm_init();
	rcvm_load_program_data(vm, size, data);
	rcvm_execute_program(vm, size);
	rcvm_print_regs(vm);
	rcvm_print_memory_from_to(vm, 0, 0x20);
}
