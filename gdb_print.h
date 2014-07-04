#ifndef GDB_PRINT_H
#define GDB_PRINT_H

#ifndef NDEBUG
# define GDB_PRINT(name)  gdb_print(# name)
# define GDB_PRINTX(name) gdb_print("\\x"# name)
#else
# define GDB_PRINT(name)
# define GDB_PRINTX(name)
#endif

void gdb_print(const char* name);

#endif

