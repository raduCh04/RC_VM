#include "rcvm.h"

int main(int argc, char const *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: rcvm [filename] (make sure the binary file is in rcvm folder!)\n");
        exit(1);
    }
    usize size;
    uint8 *data = file_read_binary(argv[1], &size);

    rcvm_t *vm = rcvm_init();
    rcvm_load_program_data(vm, size, data);
    rcvm_execute_program(vm, size);

    rcvm_print_regs(vm);
    rcvm_write_memory_to_file(vm, "../../../rcvm/test.data");
    return 0;
}
