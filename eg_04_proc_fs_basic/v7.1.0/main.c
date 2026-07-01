#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>

#include "main.h"

static struct proc_dir_entry *parent = NULL;

static int proc_open(struct inode *inode, struct file *filp);
static int proc_release(struct inode *inode, struct file *filp);

static struct proc_ops proc_ops = {
	.proc_open = proc_open,
	.proc_read = seq_read,
	.proc_lseek = seq_lseek,
	.proc_release = proc_release,
};

static int proc_show(struct seq_file *m, void *v)
{
	long c = (m->private) ? (long)m->private : 1;

	for (int i = 0; i < c; ++i)
        seq_printf(m, "hello world\n");

    return 0;
}

static int proc_open(struct inode *inode, struct file *filp)
{
	return single_open(filp, proc_show, pde_data(inode));
}

static int proc_release(struct inode *inode, struct file *filp)
{
    return single_release(inode, filp);
}

static int __init m_init(void)
{
	printk(KERN_WARNING MODULE_NAME " is loaded\n");

    int ret;
	parent = proc_mkdir(SUB_DIR_NAME, 0);
	if (!parent) {
		pr_err("failed to create %s\n", SUB_DIR_NAME);
		return -ENOMEM;
	}
	if (!proc_create(PROC_FS_NAME, 0, parent, &proc_ops)) {
		pr_err("failed to create %s\n", PROC_FS_NAME);
		ret = -ENOMEM;
		goto del_prarent;
	}

	if (!proc_create_data(PROC_FS_NAME_MUL, 0, parent, &proc_ops,
			      (void *)PRINT_NR)) {
		pr_err("failed to create %s\n", PROC_FS_NAME_MUL);
		ret = -ENOMEM;
		goto del_fs;
	}

	return 0;
del_fs:
del_prarent:
	remove_proc_subtree(SUB_DIR_NAME, NULL);
	return ret;
}

static void __exit m_exit(void)
{
	printk(KERN_WARNING MODULE_NAME " unloaded\n");

	remove_proc_subtree(SUB_DIR_NAME, NULL);
}

module_init(m_init);
module_exit(m_exit);

MODULE_AUTHOR("d0u9");
MODULE_AUTHOR("Ankit khushwaha");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("A very basic example to /proc fs manipulation");
