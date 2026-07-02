
#define pr_fmt(fmt) KBUILD_MODNAME ":%s: " fmt, __func__

#include <linux/kernel.h>
#include <linux/seq_file.h>

#include "main.h"
#include "fops.h"

static void *proc_seq_start(struct seq_file *, loff_t *);
static void *proc_seq_next(struct seq_file *, void *, loff_t *);
static void proc_seq_stop(struct seq_file *, void *);
static int proc_seq_show(struct seq_file *, void *);

static const struct seq_operations proc_seq_ops = {
	.start = proc_seq_start,
	.next = proc_seq_next,
	.stop = proc_seq_stop,
	.show = proc_seq_show,
};

int proc_open(struct inode *inode, struct file *filp)
{
	pr_debug("invoked\n");
	return seq_open(filp, &proc_seq_ops);
}

static char *data[DATA_BLOCK_NUM] = {
	"Day 1: God creates the heavens and the earth.",
	"Day 2: God creates the sky.",
	"Day 3: God creates dry land and all plant life both large and small.",
	"Day 4: God creates all the stars and heavenly bodies.",
	"Day 5: God creates all life that lives in the water.",
	"Day 6: God creates all the creatures that live on dry land.",
	"Day 7: God rests."
};

static void *proc_seq_start(struct seq_file *m, loff_t *pos)
{
	pr_debug("invoked, pos=%lld\n", *pos);
	if (*pos >= ARRAY_SIZE(data)) {
		pr_debug("position requested exceeds the maximum length\n");
		return NULL;
	}
	return *(data + *pos);
}

static void *proc_seq_next(struct seq_file *m, void *v, loff_t *pos)
{
	pr_debug("invoked, pos=%lld\n", *pos);

	(*pos)++;
	if (*pos >= ARRAY_SIZE(data)) {
		pr_debug("position requested exceeds the maximum length\n");
		return NULL;
	}

	return *(data + *pos);
}

static int proc_seq_show(struct seq_file *m, void *v)
{
	pr_debug("invoked\n");

	seq_printf(m, "%p: %s\n", v, (char *)v);
	return 0;
}

static void proc_seq_stop(struct seq_file *, void *)
{
	pr_debug("invoked\n");
}
