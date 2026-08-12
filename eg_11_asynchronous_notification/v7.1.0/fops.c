#define pr_fmt(fmt) "%s: " fmt, __func__

#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/mutex.h>

#include "main.h"
#include "fops.h"

int async_notify_open(struct inode *inode, struct file *filp)
{
	pr_debug(" invoked\n");

	filp->private_data =
		container_of(inode->i_cdev, struct async_notify_dev, cdev);

	return 0;
}

ssize_t async_notify_read(struct file *filp, char __user *buff, size_t count,
			  loff_t *f_pos)
{
	int ret;
	struct async_notify_dev *dev = filp->private_data;

	pr_debug(" invoked\n");

	if (mutex_lock_interruptible(&dev->mutex))
		return -ERESTARTSYS;

	if (count > dev->buf_len - *f_pos)
		count = dev->buf_len - *f_pos;

	if (copy_to_user(buff, dev->buff + *f_pos, count)) {
		ret = -EFAULT;
		goto cpy_user_error;
	}

	*f_pos += count;
	ret = count;

cpy_user_error:
	mutex_unlock(&dev->mutex);
	return ret;
}

ssize_t async_notify_write(struct file *filp, const char __user *buff,
			   size_t count, loff_t *f_pos)
{
	int retval;
	struct async_notify_dev *dev = filp->private_data;

	pr_debug("%s() is invoked\n", __FUNCTION__);

	if (mutex_lock_interruptible(&dev->mutex))
		return -ERESTARTSYS;

	if (count > BUFF_SIZE - *f_pos)
		count = BUFF_SIZE - *f_pos;

	if (copy_from_user(dev->buff + *f_pos, buff, count)) {
		retval = -EFAULT;
		goto cpy_user_error;
	}

	*f_pos += count;
	dev->buf_len = *f_pos;
	retval = count;

	if (dev->async_queue) {
	    kill_fasync(&dev->async_queue, SIGIO, POLL_IN);
	}

cpy_user_error:
	mutex_unlock(&dev->mutex);
	return retval;
}

int async_notify_fasync(int fd, struct file *filp, int mode)
{
	struct async_notify_dev *dev = filp->private_data;

	return fasync_helper(fd, filp, mode, &dev->async_queue);
}

int async_notify_release(struct inode *inode, struct file *filp)
{
	// remove the async_queue from the file
	struct async_notify_dev *dev = filp->private_data;

	return fasync_helper(-1, filp, 0, &dev->async_queue);
}
