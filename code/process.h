#ifndef _PROCESS_H_
#define _PROCESS_H_

#include <linux/kernel.h>

uintptr_t get_module_base(pid_t pid, char *name);

#endif
