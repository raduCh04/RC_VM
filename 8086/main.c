#include "io.h"
#include "vm.h"

int main()
{
	usize size;
	uint8 *data = file_read_binary("test", &size);

	vm_t *vm = vm_init();
	load_program_data(vm, size, data);
	execute_program(vm, size);
	print_regs(vm);
	print_memory_from_to(vm, 0x100, 0x200);
	return 0;
}

