#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>

#include "main.h"
#include "fops.h"


static const struct proc_ops proc_fops = {
    .proc_open = proc_open,
	.proc_read = seq_read,
    .proc_lseek = seq_lseek,
    .proc_release = seq_release,
};

static int __init m_init(void)
{
	pr_info(MODULE_NAME " is loaded\n");

	if (!proc_create(PROC_FILE_NAME, 0666, NULL, &proc_fops))
		return -ENOMEM;

	return 0;
}

static void __exit m_exit(void)
{
	pr_info(MODULE_NAME " unloaded\n");
	remove_proc_entry(PROC_FILE_NAME, NULL);
}

module_init(m_init);
module_exit(m_exit);

MODULE_AUTHOR("ankit");
MODULE_AUTHOR("d0u9");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Implement proc file interface via seq_file iterator");
