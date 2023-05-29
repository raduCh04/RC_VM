#include "io.h"
#include "vm.h"

int main()
{
	usize size;
	uint8 *data = file_read_binary("test", &size);

	vm_t vm = vm_init();
	vm.load_program_data(size, data);
	vm.execute_program(size);
	vm.print_regs();
	vm.print_memory_from_to(0x100, 0x200);
	return 0;
}
