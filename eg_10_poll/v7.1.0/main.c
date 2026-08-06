#define pr_fmt(fmt) "%s: " fmt, __func__

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/device.h>

#include "main.h"
#include "fops.h"

static struct poll_dev *poll_dev;
static dev_t dev__t;
struct class *class_poll;
struct device *device_poll;

static struct file_operations fops = {
	.open = poll_open,
	.read = poll_read,
	.write = poll_write,
	.poll = poll_poll,
};

static void __init init_dev(struct poll_dev *dev)
{
	mutex_init(&dev->mutex);
	init_waitqueue_head(&dev->inq);
	init_waitqueue_head(&dev->outq);

	cdev_init(&dev->cdev, &fops);
	dev->cdev.owner = THIS_MODULE;

	dev->buf_len = ARRAY_SIZE(DFT_MSG);
	memcpy(dev->buff, DFT_MSG, dev->buf_len);
}

static int __init m_init(void)
{
	int ret;

	pr_info(" is loaded\n");

	poll_dev = kzalloc(sizeof(*poll_dev), GFP_KERNEL);
	if (!poll_dev) {
		pr_debug("Cannot alloc memory!\n");
		return -ENOMEM;
	}
	ret = alloc_chrdev_region(&dev__t, 0, POLL_DEV_NR, MODULE_NAME);
	if (ret) {
		pr_debug("Can't get major!\n");
		goto free_poll;
	}

	init_dev(poll_dev);
	ret = cdev_add(&poll_dev->cdev, dev__t, POLL_DEV_NR);
	if (ret) {
		pr_err("Cdev add failed\n");
		goto unreg_chrdev;
	}

	class_poll = class_create(MODULE_NAME);
	if (IS_ERR(class_poll)) {
		pr_err("Class creation failed\n");
		ret = PTR_ERR(class_poll);
		goto cdev_del;
	}

	device_poll =
		device_create(class_poll, NULL, dev__t, NULL, MODULE_NAME);
	if (IS_ERR(device_poll)) {
		pr_err("Device create failed\n");
		ret = PTR_ERR(device_poll);
		goto class_del;
	}

	return 0;
class_del:
	class_destroy(class_poll);
cdev_del:
	cdev_del(&poll_dev->cdev);
unreg_chrdev:
	unregister_chrdev_region(dev__t, POLL_DEV_NR);
free_poll:
	kfree(poll_dev);
	return ret;
}

static void __exit m_exit(void)
{
	pr_info(" is unloaded\n");
	device_destroy(class_poll, dev__t);
	class_destroy(class_poll);
	cdev_del(&poll_dev->cdev);
	unregister_chrdev_region(dev__t, POLL_DEV_NR);
	kfree(poll_dev);
}

module_init(m_init);
module_exit(m_exit);

MODULE_AUTHOR("d0u9");
MODULE_AUTHOR("ankit");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("A simple poll example");
