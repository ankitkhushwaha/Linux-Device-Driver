#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/list.h>

#include "main.h"
#include "fops.h"

static dev_t dev__t;
static struct class *class__t;

static struct scull_dev *scull_devt[SCULL_NR_DEVS];

static struct file_operations scull_fops = {
	.owner = THIS_MODULE,
	.open = scull_open,
	.read = scull_read,
	.write = scull_write,
	.release = scull_release,
};

static void __init init_scull_dev(struct scull_dev *dev)
{
	dev->block_counter = 0;

	INIT_LIST_HEAD(&dev->block_list);
	mutex_init(&dev->mutex);

	cdev_init(&dev->cdev, &scull_fops);
	dev->cdev.owner = THIS_MODULE;
}

static int __init m_init(void)
{
	int i, ret, major;
	dev_t tmp;

	ret = alloc_chrdev_region(&dev__t, 0, SCULL_NR_DEVS, MODULE_NAME);
	if (ret < 0) {
		pr_debug("Cant't get major");
		goto out;
	}

	major = MAJOR(dev__t);

	class__t = class_create(CLASS_NAME);
	if (IS_ERR(class__t)) {
		pr_err("Class creation failed\n");
		ret = PTR_ERR(class__t);
		goto u_char_region;
	}

	for (i = 0; i < SCULL_NR_DEVS; i++) {
		scull_devt[i] = kmalloc(sizeof(struct scull_dev), GFP_KERNEL);
		if (!scull_devt[i]) {
			pr_err("scull_devt[%d] allocation failed\n", i);
			goto del_device;
		}

		init_scull_dev(scull_devt[i]);

		tmp = MKDEV(major, i);
		/*
		 * The cdev_add() function will make this char device usable
		 * in userspace. If you havn't ready to populate this device
		 * to its users, DO NOT call cdev_add()
		 */
		ret = cdev_add(&scull_devt[i]->cdev, tmp, 1);
		if (ret) {
			pr_debug("Error(%d): cdev_add: Adding %s%d error\n",
				 ret, MODULE_NAME, i);
			kfree(scull_devt[i]);
			scull_devt[i] = NULL;
			goto del_device;
		}

		scull_devt[i]->device__t = device_create(
			class__t, NULL, tmp, NULL, "scull_dev-%d", i);
		if (IS_ERR(scull_devt[i]->device__t)) {
			pr_err("Device create failed\n");
			ret = PTR_ERR(scull_devt[i]->device__t);

			cdev_del(&scull_devt[i]->cdev);
			kfree(scull_devt[i]);
			scull_devt[i] = NULL;
			goto del_device;
		}
	}

	pr_info(MODULE_NAME " is loaded\n");
	return 0;

	/* TODO: Review error handling */
del_device:
	while (--i >= 0) {
		tmp = MKDEV(major, i);
		device_destroy(class__t, tmp);
		cdev_del(&scull_devt[i]->cdev);
	}
	class_destroy(class__t);
u_char_region:
	unregister_chrdev_region(dev__t, SCULL_NR_DEVS);
out:
	return ret;
}

static void __exit m_exit(void)
{
	int i;
	int major = MAJOR(dev__t);
	for (i = 0; i < SCULL_NR_DEVS; i++) {
		device_destroy(class__t, MKDEV(major, i));
		cdev_del(&scull_devt[i]->cdev);

		scull_trim(scull_devt[i]);
		kfree(scull_devt[i]);
		scull_devt[i] = NULL;
	}
	class_destroy(class__t);
	unregister_chrdev_region(dev__t, SCULL_NR_DEVS);

	pr_info(MODULE_NAME " is unloaded\n");
}

module_init(m_init);
module_exit(m_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ankit");
MODULE_AUTHOR("d0u9");
MODULE_DESCRIPTION("A simple memory based storage device aims to demonstrate "
		   "basic concepts of char device");
