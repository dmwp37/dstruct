#ifndef GDB_PRINT_H
#define GDB_PRINT_H

#ifndef NDEBUG
# define GDB_PRINT(name) gdb_print(# name)
#else
# define GDB_PRINT(name)
#endif

void gdb_print(const char* name);

#endif

