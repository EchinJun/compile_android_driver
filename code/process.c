#include "process.h"
#include <linux/sched.h>
#include <linux/sched/mm.h>
#include <linux/sched/task.h>
#include <linux/mm.h>
#include <linux/pid.h>
#include <linux/fs.h>
#include <linux/dcache.h>
#include <linux/string.h>
#include <linux/mmap_lock.h>

#define ARC_PATH_MAX 256

uintptr_t get_module_base(pid_t pid, char *name)
{
	struct pid *pid_struct;
	struct task_struct *task;
	struct mm_struct *mm;
	struct vm_area_struct *vma;
	uintptr_t base_addr = 0;

	pid_struct = find_get_pid(pid);
	if (!pid_struct)
		return 0;

	task = get_pid_task(pid_struct, PIDTYPE_PID);
	put_pid(pid_struct);
	if (!task)
		return 0;

	mm = get_task_mm(task);
	put_task_struct(task);
	if (!mm)
		return 0;

	mmap_read_lock(mm);

	for (vma = mm->mmap; vma; vma = vma->vm_next) {
		if (vma->vm_file) {
			char buf[ARC_PATH_MAX];
			char *path_nm;
			path_nm = d_path(&vma->vm_file->f_path, buf, ARC_PATH_MAX - 1);
			if (!IS_ERR(path_nm)) {
				const char *basename = strrchr(path_nm, '/');
				basename = basename ? basename + 1 : path_nm;
				if (strcmp(basename, name) == 0) {
					base_addr = vma->vm_start;
					break;
				}
			}
		}
	}

	mmap_read_unlock(mm);
	mmput(mm);
	return base_addr;
}
