#define pr_fmt(fmt) "%s: " fmt, __func__

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/poll.h>
#include <linux/uaccess.h>

#include "main.h"
#include "fops.h"

int poll_open(struct inode *inode, struct file *filp)
{
	pr_debug("%s() is invoked\n", __FUNCTION__);

	filp->private_data = container_of(inode->i_cdev, struct poll_dev, cdev);

	return 0;
}

ssize_t poll_read(struct file *filp, char __user *buff, size_t count,
		  loff_t *f_pos)
{
	int retval;
	struct poll_dev *dev = filp->private_data;

	pr_debug("invoked\n");

	if (mutex_lock_interruptible(&dev->mutex))
		return -ERESTARTSYS;

	while (dev->buf_len == 0) {
		mutex_unlock(&dev->mutex);

		if (filp->f_flags & O_NONBLOCK)
			return -EAGAIN;

		if (wait_event_interruptible(dev->inq, dev->buf_len > 0))
			return -ERESTARTSYS;

		if (mutex_lock_interruptible(&dev->mutex))
			return -ERESTARTSYS;
	}

	if (count > dev->buf_len) {
		count = dev->buf_len;
	}

	if (copy_to_user(buff, dev->buff, count)) {
		retval = -EFAULT;
		goto cpy_user_error;
	}

	memmove(dev->buff, dev->buff + count, dev->buf_len - count);
	dev->buf_len -= count;
	retval = count;

	wake_up_interruptible(&dev->outq);
cpy_user_error:
	mutex_unlock(&dev->mutex);
	return retval;
}

ssize_t poll_write(struct file *filp, const char __user *buff, size_t count,
		   loff_t *f_pos)
{
	int retval;
	struct poll_dev *dev = filp->private_data;

	pr_debug("invoked\n");

	if (mutex_lock_interruptible(&dev->mutex))
		return -ERESTARTSYS;

	while (dev->buf_len == BUFF_SIZE) {
		mutex_unlock(&dev->mutex);

		if (filp->f_flags & O_NONBLOCK)
			return -EAGAIN;

		if (wait_event_interruptible(dev->outq,
					     dev->buf_len < BUFF_SIZE))
			return -ERESTARTSYS;

		if (mutex_lock_interruptible(&dev->mutex))
			return -ERESTARTSYS;
	}

	if (count > (BUFF_SIZE - dev->buf_len))
		count = BUFF_SIZE - dev->buf_len;

	if (copy_from_user(dev->buff + dev->buf_len, buff, count)) {
		retval = -EFAULT;
		goto cpy_user_error;
	}

	dev->buf_len += count;
	retval = count;

	wake_up_interruptible(&dev->inq);
cpy_user_error:
	mutex_unlock(&dev->mutex);
	return retval;
}

unsigned int poll_poll(struct file *filp, poll_table *wait)
{
	struct poll_dev *dev = filp->private_data;
	unsigned int mask = 0;

	pr_debug("invoked\n");

	mutex_lock(&dev->mutex);

	poll_wait(filp, &dev->inq, wait);
	poll_wait(filp, &dev->outq, wait);

	if (dev->buf_len > 0) {
		pr_debug("Now fd can be read\n");
		mask |= POLLIN | POLLRDNORM;
	}

	if (dev->buf_len < BUFF_SIZE) {
		pr_debug("fd can be written\n");
		mask |= POLLOUT | POLLWRNORM;
	}
	mutex_unlock(&dev->mutex);

	pr_debug("return mask = 0x%x\n", mask);
	return mask;
}
