#define pr_fmt(fmt) "%s: " fmt, __func__

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>

#include "main.h"
#include "fops.h"

static dev_t dev__t;
static struct async_notify_dev *async_notify_dev;
static struct class *class_fasync;
static struct device *device_fasync;

static struct file_operations fops = {
	.open = async_notify_open,
	.read = async_notify_read,
	.write = async_notify_write,
	.fasync = async_notify_fasync,
	.release = async_notify_release,
};

static int __init m_init(void)
{
	int ret;
	pr_info(MODULE_NAME " is loaded\n");

	async_notify_dev = kzalloc(sizeof(struct async_notify_dev), GFP_KERNEL);
	if (!async_notify_dev) {
		pr_debug("Cannot alloc memory!\n");
		return -ENOMEM;
	}

	ret = alloc_chrdev_region(&dev__t, 0, ASYNC_NOTIFY_DEV_NR, MODULE_NAME);
	if (ret < 0) {
		pr_debug("Can't get major!\n");
		goto free_mem;
	}

	mutex_init(&async_notify_dev->mutex);
	cdev_init(&async_notify_dev->cdev, &fops);
	async_notify_dev->cdev.owner = THIS_MODULE;

	ret = cdev_add(&async_notify_dev->cdev, dev__t, ASYNC_NOTIFY_DEV_NR);
	if (ret) {
		pr_err("Error when adding ioctl dev");
		goto unreg_chardev;
	}

	class_fasync = class_create(MODULE_NAME);
	if (IS_ERR(class_fasync)) {
		ret = PTR_ERR(class_fasync);
		pr_err("class creation failed\n");
		goto del_cdev;
	}
	device_fasync =
		device_create(class_fasync, NULL, dev__t, NULL, MODULE_NAME);
	if (IS_ERR(device_fasync)) {
		ret = PTR_ERR(device_fasync);
		pr_err("device creation failed\n");
		goto del_class;
	}
	return 0;
del_class:
	class_destroy(class_fasync);
del_cdev:
	cdev_del(&async_notify_dev->cdev);
unreg_chardev:
	unregister_chrdev_region(dev__t, ASYNC_NOTIFY_DEV_NR);
free_mem:
	kfree(async_notify_dev);
	return ret;
}

static void __exit m_exit(void)
{
	printk(KERN_WARNING MODULE_NAME " unloaded\n");

	device_destroy(class_fasync, dev__t);
	class_destroy(class_fasync);
	cdev_del(&async_notify_dev->cdev);
	unregister_chrdev_region(dev__t, ASYNC_NOTIFY_DEV_NR);
	kfree(async_notify_dev);
}

module_init(m_init);
module_exit(m_exit);

MODULE_AUTHOR("d0u9");
MODULE_AUTHOR("ankit");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Asynchronous notification for non-block IO");
