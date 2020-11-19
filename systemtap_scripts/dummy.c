#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <sys/ioctl.h>

#include "kmalloc_ioctl.h"

// Calls into the Kmalloc kernel module so SystemTap will
// not ignore instrumenting the module.
void do_dummy(int fd)
{
	kmalloc_arg_t q;
	if (ioctl(fd, PERFORM_DUMMY, &q) == -1){perror("dummy failed");}
}

int main()
{
	char *file_name = "/dev/kmalloc";
	int fd;
	fd = open(file_name, O_RDWR);
	if (fd == -1)
	{
		perror("kmalloc_app already open");
		return 2;
	}

	while (true) {
		sleep(10);
		do_dummy(fd);
	}

	close (fd);

	return 0;
}