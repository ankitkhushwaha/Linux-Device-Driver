#define pr_fmt(fmt) ":%s: " fmt, __func__

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/stringify.h>
#include <linux/completion.h>
#include <linux/uaccess.h>

#include "main.h"

static struct completion_dev completion_dev;
static dev_t dev__t;

static struct class *completion_class;
static struct device *completion_device;

#ifdef STRING
#define CHAR_BLOCK_SIZE 1024
static char char_block[CHAR_BLOCK_SIZE];
#endif

static int completion_open(struct inode *inode, struct file *filp)
{
	pr_info("invoked\n");

	filp->private_data =
		container_of(inode->i_cdev, struct completion_dev, cdev);

	return 0;
}

static ssize_t completion_read(struct file *filp, char __user *buf,
			       size_t count, loff_t *pos)
{
	pr_info("invoked\n");

	struct completion_dev *dev = filp->private_data;
    
	pr_debug("process %d(%s) going to sleep\n", current->pid,
		 current->comm);
	wait_for_completion(&dev->completion);
	pr_debug("awoken %d(%s)\n", current->pid, current->comm);

#ifdef STRING
	int ret;
	if (*pos >= CHAR_BLOCK_SIZE)
		return 0;

	if (count > CHAR_BLOCK_SIZE - *pos)
		count = CHAR_BLOCK_SIZE - *pos;

	ret = copy_to_user(buf, char_block + *pos, count);
	if (ret != 0) {
		pr_err("copy to userspace failed\n");
		return -EFAULT;
	}

	*pos += count;
	return count;
#else
	return 0;
#endif
}

static ssize_t completion_write(struct file *filp, const char __user *buf,
				size_t count, loff_t *pos)
{
	pr_debug("invoked\n");

	struct completion_dev *dev = filp->private_data;
    
	pr_debug("process %d(%s) awakening the readers...\n", current->pid,
        current->comm);

        #ifdef STRING    
    int ret;
	if (*pos >= CHAR_BLOCK_SIZE)
		return -ENOSPC;

	if (count > CHAR_BLOCK_SIZE - *pos)
		count = CHAR_BLOCK_SIZE - *pos;

	ret = copy_from_user(char_block + *pos, buf, count);
	if (ret != 0) {
		pr_err("copy from userspace failed\n");
		return -EFAULT;
	}
	*pos += count;
#endif
	complete(&dev->completion);
	return count;
}

static const struct file_operations completion_fops = {
	.owner = THIS_MODULE,
	.open = completion_open,
	.read = completion_read,
	.write = completion_write,
};

static int __init m_init(void)
{
	int ret;

	pr_info(MODULE_NAME " is loaded\n");

	init_completion(&completion_dev.completion);

	ret = alloc_chrdev_region(&dev__t, 0, 1, MODULE_NAME);
	if (ret) {
		pr_debug("Error: %d -Cant't get major\n", ret);
		return ret;
	}
	cdev_init(&completion_dev.cdev, &completion_fops);

	ret = cdev_add(&completion_dev.cdev, dev__t, 1);
	if (ret) {
		pr_debug("Error(%d): Adding completion device error\n", ret);
		goto unreg_chrdev;
	}

	completion_class = class_create(__stringify(completion_class));
	if (IS_ERR(completion_class)) {
		pr_err("Class creation failed\n");
		ret = PTR_ERR(completion_class);
		goto cdev_del;
	}

	completion_device = device_create(completion_class, NULL, dev__t, NULL,
					  __stringify(completion_device));
	if (IS_ERR(completion_device)) {
		ret = PTR_ERR(completion_device);
		goto dev_del;
	}
	return 0;

dev_del:
	class_destroy(completion_class);
cdev_del:
	cdev_del(&completion_dev.cdev);
unreg_chrdev:
	unregister_chrdev_region(dev__t, 1);
	return ret;
}

static void __exit m_exit(void)
{
	pr_info(MODULE_NAME " unloaded\n");

	device_destroy(completion_class, dev__t);
	class_destroy(completion_class);
	cdev_del(&completion_dev.cdev);
	unregister_chrdev_region(dev__t, 1);
}

module_init(m_init);
module_exit(m_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ankit");
MODULE_AUTHOR("d0u9");
MODULE_DESCRIPTION("Example of Kernel's completion mechanism");
