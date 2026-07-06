#define pr_fmt(fmt) ":%s: " fmt, __func__

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/string.h>

#include "fops.h"
#include "main.h"

int ioctl_open(struct inode *inode, struct file *filp)
{
	pr_debug("invoked\n");

	filp->private_data =
		container_of(inode->i_cdev, struct ioctl_dev, cdev);

	return 0;
}

ssize_t ioctl_read(struct file *filp, char __user *buff, size_t count,
		   loff_t *f_pos)
{
	struct ioctl_dev *ioctl_dev = filp->private_data;
	int howmany = 0, offset = 0;

	pr_debug("invoked\n");

	if (ioctl_dev->buf_len == 0)
		return 0;

	howmany = *f_pos / ioctl_dev->buf_len;
	offset = *f_pos % ioctl_dev->buf_len;

	pr_debug("howmany = %d, offset=%d, many=%d\n", howmany, offset,
		 ioctl_dev->howmany);

	if (howmany >= ioctl_dev->howmany)
		return 0;

	if (count > ioctl_dev->buf_len - offset)
		count = ioctl_dev->buf_len - offset;

	if (copy_to_user(buff, ioctl_dev->buff, count)) {
		pr_debug("Error occurs shen copy to userspace\n");
		return -EFAULT;
	}

	*f_pos += count;
	return count;
}

static int ioctl_reset(struct ioctl_dev *dev)
{
	pr_debug("invoked\n");

	int size = ARRAY_SIZE(DEFAULT_MESSAGE);

	dev->howmany = DEFAULT_HOWMANY;
	dev->buf_len = size;
	memset(dev->buff, 0, BUFF_SIZE);
	memcpy(dev->buff, DEFAULT_MESSAGE, size);

	return 0;
}

static int ioctl_howmany(struct ioctl_dev *dev, unsigned long arg)
{
	pr_debug("invoked\n");

	// if (capable(CAP_SYS_ADMIN)) {
	// 	pr_warn("User is'nt admin, run with 'sudo'\n");
	// 	return -EPERM;
	// }

	dev->howmany = arg;
	pr_debug("set howmany = %d\n", dev->howmany);

	return dev->howmany;
}

static int ioctl_message(struct ioctl_dev *dev, void __user *arg)
{
	pr_debug("invoked\n");

	struct ioctl_msg_arg msg_arg;

	if (copy_from_user(&msg_arg, (struct ioctl_msg_arg *)arg,
			   sizeof(msg_arg))) {
		pr_debug("copy arguments from user error\n");
		return -EFAULT;
	}

	if (msg_arg.len > BUFF_SIZE) {
		pr_debug("message length (%d bytes) exceeds the limit\n",
			 msg_arg.len);
		return -ENOMEM;
	}

	if (copy_from_user(dev->buff, (void __user *)msg_arg.msg,
			   msg_arg.len)) {
		pr_debug("copy message from user error\n");
		return -EFAULT;
	}

	memset(dev->buff + msg_arg.len, 0, BUFF_SIZE - msg_arg.len);
	dev->buf_len = msg_arg.len;

	return 0;
}

long ioctl_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	pr_debug("invoked\n");

	int ret;
	struct ioctl_dev *ioctl_dev = filp->private_data;

	if (_IOC_TYPE(cmd) != IOCTL_IOC_MAGIC) {
		pr_warn("magic mismatch IOCTL failed\n");
		return -ENOTTY;
	}

	if (_IOC_NR(cmd) > IOCTL_MAXNR) {
		pr_warn("invalid ioctl call\n");
		return -ENOTTY;
	}

	ret = access_ok((void __user *)arg, _IOC_SIZE(cmd));
	if (!ret)
		return -EFAULT;

	switch (cmd) {
	case IOCTL_RESET:
		pr_debug("ioctl -> cmd: reset\n");
		ret = ioctl_reset(ioctl_dev);
		break;
	case IOCTL_HOWMANY:
		pr_debug("ioctl -> cmd: set howmany\n");
		ret = ioctl_howmany(ioctl_dev, (unsigned long)arg);
		break;
	case IOCTL_MESSAGE:
		pr_debug("ioctl -> cmd: set print message\n");
		ret = ioctl_message(ioctl_dev, (void *__user)arg);
		break;
	default:
		return -ENOTTY;
	}

	return ret;
}