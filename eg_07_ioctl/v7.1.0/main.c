#define pr_fmt(fmt) ":%s: " fmt, __func__

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/string.h>
#include <linux/stringify.h>

#include "main.h"
#include "fops.h"

static struct ioctl_dev *ioctl_dev;
static dev_t dev__t;
static struct class *ioctl_class;
static struct device *ioctl_device;

static struct file_operations fops = {
	.open = ioctl_open,
	.read = ioctl_read,
	.unlocked_ioctl = ioctl_ioctl,
};

static int __init m_init(void)
{
	int ret;

	pr_info(MODULE_NAME " loaded\n");
	ioctl_dev = kzalloc(sizeof(*ioctl_dev), GFP_KERNEL);
	if (!ioctl_dev) {
		pr_debug("Cannot alloc memory!\n");
		return -ENOMEM;
	}

	ioctl_dev->howmany = DEFAULT_HOWMANY;
	memcpy(ioctl_dev->buff, DEFAULT_MESSAGE, ARRAY_SIZE(DEFAULT_MESSAGE));
	ioctl_dev->buf_len = ARRAY_SIZE(DEFAULT_MESSAGE);

	ret = alloc_chrdev_region(&dev__t, 0, IOCTL_DEV_NR, MODULE_NAME);
	if (ret) {
		pr_debug("Can't get major!\n");
		goto free_mem;
	}
	cdev_init(&ioctl_dev->cdev, &fops);
	ioctl_dev->cdev.owner = THIS_MODULE;

	ret = cdev_add(&ioctl_dev->cdev, dev__t, IOCTL_DEV_NR);
	if (ret) {
		pr_debug("Error when adding ioctl dev");
		goto unreg_chrdev;
	}

	ioctl_class = class_create(__stringify(ioctl_class));
	if (IS_ERR(ioctl_class)) {
		pr_err("Class creation failed\n");
		ret = PTR_ERR(ioctl_class);
		goto cdev_del;
	}

	ioctl_device = device_create(ioctl_class, NULL, dev__t, NULL,
				     __stringify(ioctl_device));
	if (IS_ERR(ioctl_device)) {
		ret = PTR_ERR(ioctl_device);
		goto cls_del;
	}

	return 0;

cls_del:
	class_destroy(ioctl_class);
cdev_del:
	cdev_del(&ioctl_dev->cdev);
unreg_chrdev:
	unregister_chrdev_region(dev__t, IOCTL_DEV_NR);
free_mem:
	kfree(ioctl_dev);
    return ret;
}

static void __exit m_exit(void)
{
	pr_info(MODULE_NAME " unloaded\n");

	device_destroy(ioctl_class, dev__t);
	class_destroy(ioctl_class);
	cdev_del(&ioctl_dev->cdev);
	unregister_chrdev_region(dev__t, IOCTL_DEV_NR);
    kfree(ioctl_dev);
}


module_init(m_init);
module_exit(m_exit);

MODULE_AUTHOR("d0u9");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("An ioctl() example");
