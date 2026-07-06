#ifndef _FOPS_H_
#define _FOPS_H_

#include <linux/fs.h>

extern int ioctl_open(struct inode *, struct file *);
extern ssize_t ioctl_read(struct file *, char __user *, size_t, loff_t *);
extern long ioctl_ioctl(struct file *, unsigned int, unsigned long);

#endif