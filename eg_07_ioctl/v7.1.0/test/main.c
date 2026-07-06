#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#define DEV_FILE "/dev/ioctl_device"

#define NEW_MSG "Hello, Linux!!\n"

#define IOCTL_IOC_MAGIC 'd'

#define IOCTL_RESET _IO(IOCTL_IOC_MAGIC, 0)
#define IOCTL_HOWMANY _IOWR(IOCTL_IOC_MAGIC, 1, int)
#define IOCTL_MESSAGE _IOW(IOCTL_IOC_MAGIC, 2, long)

struct ioctl_msg_arg {
	int len;
	char *msg;
};

int main(int argc, char *argv[])
{
	int err, fd;
	char *msg = NEW_MSG;

    struct ioctl_msg_arg msg_arg = { .len = sizeof(NEW_MSG), .msg = msg};

    fd = open(DEV_FILE, O_RDONLY);
	if (fd < 0) {
		printf("failed to open file: " DEV_FILE);
		return fd;
	}
	// err = ioctl(fd, IOCTL_RESET);	//reset
	// err = ioctl(fd, IOCTL_HOWMANY, 5);
    err = ioctl(fd, IOCTL_MESSAGE, &msg_arg);

	printf("retval = %d\n", err);

    return 0;
}