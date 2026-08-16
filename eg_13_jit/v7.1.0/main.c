#define pr_fmt(fmt) "%s: " fmt, __func__

#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/fs.h>

#include "main.h"
#include "fops.h"

int delay = HZ;
int tdelay = 10;

module_param(delay, int, 0);

static struct opt *opts[PROC_FILE_NR] = { NULL };
static struct proc_dir_entry *jit_proc;

static int proc_open(struct inode *inode, struct file *filp)
{
    struct opt *opt = pde_data(inode);
    return single_open(filp, opt->show, opt->args);
}

static int proc_release(struct inode *inode, struct file *filp)
{
    return single_release(inode, filp);
}

static const struct proc_ops proc_ops = {
	.proc_open = proc_open,
	.proc_read = seq_read,
	.proc_lseek = seq_lseek,
	.proc_release = proc_release,
};

static inline struct opt *new_opt(int (*show)(struct seq_file *m, void *p),
				  void *args)
{
	struct opt *opt = kmalloc(sizeof(struct opt), GFP_KERNEL);
	opt->show = show;
	opt->args = args;
	return opt;
}
static int __init m_init(void)
{
	int i = 0;
	pr_info(MODULE_NAME " is loaded\n");

	jit_proc = proc_mkdir("jit", NULL);

	opts[i] = new_opt(jit_currentime, NULL);
	proc_create_data("currentime", 0, jit_proc, &proc_ops, opts[i++]);

	opts[i] = new_opt(jit_fn, (void *)JIT_BUSY);
	proc_create_data("jitbusy", 0, jit_proc, &proc_ops, opts[i++]);
	opts[i] = new_opt(jit_fn, (void *)JIT_SCHED);
	proc_create_data("jitsched", 0, jit_proc, &proc_ops, opts[i++]);
	opts[i] = new_opt(jit_fn, (void *)JIT_QUEUE);
	proc_create_data("jitqueue", 0, jit_proc, &proc_ops, opts[i++]);
	opts[i] = new_opt(jit_fn, (void *)JIT_SCHEDTO);
	proc_create_data("jitschedto", 0, jit_proc, &proc_ops, opts[i++]);

	opts[i] = new_opt(jit_timer, NULL);
	proc_create_data("jittimer", 0, jit_proc, &proc_ops, opts[i++]);

	opts[i] = new_opt(jit_tasklet, NULL);
	proc_create_data("jittasklet", 0, jit_proc, &proc_ops, opts[i++]);

	opts[i] = new_opt(jit_tasklet, (void *)1);
	proc_create_data("jittasklethi", 0, jit_proc, &proc_ops, opts[i++]);

	return 0;
}

static void __exit m_exit(void)
{
	pr_info(MODULE_NAME " unloaded\n");

	proc_remove(jit_proc);

	for (int i = 0; i < PROC_FILE_NR; ++i)
		kfree(opts[i]);
}

module_init(m_init);
module_exit(m_exit);

MODULE_AUTHOR("d0u9");
MODULE_AUTHOR("ankit");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Delay methods in Linux kernel.");
