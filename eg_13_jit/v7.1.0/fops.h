#ifndef _FOPS_H_
#define _FOPS_H_

#include <linux/seq_file.h>

extern int jit_currentime(struct seq_file *m, void *p);
extern int jit_fn(struct seq_file *m, void *p);
extern int jit_timer(struct seq_file *m, void *p);
extern int jit_tasklet(struct seq_file *m, void *p);

#endif
