#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/uaccess.h>
#include <linux/string.h>
#include "comm.h"
#include "memory.h"
#include "process.h"

#define DEVICE_NAME "TearGame"
#define AUTH_KEY    "YourSecretKey123"   // 修改成你自己的密钥

static bool is_verified = false;

static int dispatch_open(struct inode *node, struct file *file)
{
	return 0;
}

static int dispatch_close(struct inode *node, struct file *file)
{
	return 0;
}

static long dispatch_ioctl(struct file *const file, unsigned int const cmd, unsigned long const arg)
{
	static COPY_MEMORY cm;
	static MODULE_BASE mb;
	static char key[0x100] = {0};
	static char name[0x100] = {0};

	if (cmd == OP_INIT_KEY) {
		if (copy_from_user(key, (void __user *)arg, sizeof(key) - 1) != 0)
			return -EFAULT;
		if (strncmp(key, AUTH_KEY, strlen(AUTH_KEY)) == 0)
			is_verified = true;
		else
			return -EACCES;
		return 0;
	}

	if (!is_verified)
		return -EPERM;

	switch (cmd) {
	case OP_READ_MEM:
		if (copy_from_user(&cm, (void __user *)arg, sizeof(cm)) != 0)
			return -EFAULT;
		if (!read_process_memory(cm.pid, cm.addr, cm.buffer, cm.size))
			return -EFAULT;
		break;

	case OP_WRITE_MEM:
		if (copy_from_user(&cm, (void __user *)arg, sizeof(cm)) != 0)
			return -EFAULT;
		if (!write_process_memory(cm.pid, cm.addr, cm.buffer, cm.size))
			return -EFAULT;
		break;

	case OP_MODULE_BASE:
		if (copy_from_user(&mb, (void __user *)arg, sizeof(mb)) != 0)
			return -EFAULT;
		if (copy_from_user(name, (void __user *)mb.name, sizeof(name) - 1) != 0)
			return -EFAULT;
		mb.base = get_module_base(mb.pid, name);
		if (copy_to_user((void __user *)arg, &mb, sizeof(mb)) != 0)
			return -EFAULT;
		break;

	default:
		return -ENOTTY;
	}
	return 0;
}

static struct file_operations dispatch_functions = {
	.owner = THIS_MODULE,
	.open = dispatch_open,
	.release = dispatch_close,
	.unlocked_ioctl = dispatch_ioctl,
};

static struct miscdevice misc = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = DEVICE_NAME,
	.fops = &dispatch_functions,
};

static int __init driver_entry(void)
{
	int ret;
	printk(KERN_INFO "[TearGame] Loading driver...\n");
	ret = misc_register(&misc);
	if (ret)
		printk(KERN_ERR "[TearGame] misc_register failed: %d\n", ret);
	else
		printk(KERN_INFO "[TearGame] /dev/%s registered\n", DEVICE_NAME);
	return ret;
}

static void __exit driver_unload(void)
{
	misc_deregister(&misc);
	printk(KERN_INFO "[TearGame] Driver removed\n");
}

module_init(driver_entry);
module_exit(driver_unload);

MODULE_DESCRIPTION("TearGame Memory Driver for 5.10.199");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Adapted by You");
