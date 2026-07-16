#define pr_fmt(fmt) ":%s: " fmt, __func__

#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/mutex.h>
#include <linux/wait.h>
#include <linux/cdev.h>
#include <linux/err.h>

#include "main.h"
#include "fops.h"

static dev_t dev__t;
static struct pipe_dev *pipe_dev[PIPE_DEV_NR];
static struct class *pipe_class;
static struct device *pipe_device;

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = pipe_open,
	.read = pipe_read,
	.write = pipe_write,
};

static void __init init_pipe_dev(struct pipe_dev *dev)
{
	mutex_init(&dev->mutex);

	cdev_init(&dev->cdev, &fops);
	dev->cdev.owner = THIS_MODULE;
	init_waitqueue_head(&dev->rd_queue);
	init_waitqueue_head(&dev->wr_queue);
}

static int __init m_init(void)
{
	int i, ret;

	pr_info(MODULE_NAME " is loaded\n");

	ret = alloc_chrdev_region(&dev__t, 0, PIPE_DEV_NR, MODULE_NAME);
	if (ret < 0) {
		pr_debug("Can't get major!\n");
		return ret;
	}

	pipe_class = class_create(MODULE_NAME);
	if (IS_ERR(pipe_class)) {
		pr_err("Class creation failed\n");
		ret = PTR_ERR(pipe_class);
		goto unreg_chrdev;
	}

	for (i = 0; i < PIPE_DEV_NR; i++) {
		pipe_dev[i] = kzalloc(sizeof(struct pipe_dev), GFP_KERNEL);
		if (!pipe_dev[i]) {
			pr_debug("Error(%d): kmalloc failed on pipe%d\n", ret,
				 i);
			ret = -ENOMEM;
			goto free_mem;
		}

		init_pipe_dev(pipe_dev[i]);

		ret = cdev_add(&pipe_dev[i]->cdev, dev__t + i, 1);
		if (ret) {
			pr_debug("Error(%d): Adding pipe%d error\n", ret, i);
			kfree(pipe_dev[i]);
			pipe_dev[i] = NULL;
		}

		pipe_device = device_create(pipe_class, NULL, dev__t + i, NULL,
					    MODULE_NAME "-%d", i);
		if (IS_ERR(pipe_device)) {
			pr_err("Device create failed\n");
			ret = PTR_ERR(pipe_device);

			cdev_del(&pipe_dev[i]->cdev);
			kfree(pipe_dev[i]);
			pipe_dev[i] = NULL;
			goto cdev_del;
		}
	}

	return 0;

cdev_del:
free_mem:
	while (--i >= 0) {
		device_destroy(pipe_class, dev__t + i);
		cdev_del(&pipe_dev[i]->cdev);
		kfree(pipe_dev[i]);
		pipe_dev[i] = NULL;
	}
	class_destroy(pipe_class);
unreg_chrdev:
	unregister_chrdev_region(dev__t, PIPE_DEV_NR);
	return ret;
}

static void __exit m_exit(void)
{
	int i;

	pr_info(MODULE_NAME " unloaded\n");

	for (i = 0; i < PIPE_DEV_NR; i++) {
		device_destroy(pipe_class, dev__t + i);
		cdev_del(&pipe_dev[i]->cdev);
		kfree(pipe_dev[i]);
		pipe_dev[i] = NULL;
	}

	class_destroy(pipe_class);
	unregister_chrdev_region(dev__t, PIPE_DEV_NR);
}

module_init(m_init);
module_exit(m_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("d0u9");
MODULE_AUTHOR("ankit");
MODULE_DESCRIPTION("A pipe like device to illustrate the skill of how to put"
		   "the read/write process into sleep");
