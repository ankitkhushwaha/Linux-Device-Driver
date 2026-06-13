#ifndef _MAIN_H_
#define _MAIN_H_

#include <linux/list.h>

#define MODULE_NAME "scull"
#define CLASS_NAME MODULE_NAME "_class"
#define SCULL_NR_DEVS 3
#define SCULL_BLOCK_SIZE PAGE_SIZE // one page per block

struct scull_block {
	loff_t offset;
	char data[SCULL_BLOCK_SIZE];
	struct list_head block_list;
};

struct scull_dev {
	int block_counter; //record how many blocks now in the list
	struct mutex mutex;
	struct cdev cdev;
	struct device *device__t;
	struct list_head block_list; //list of storage blocks
};

#endif
