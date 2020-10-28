#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>

#include "kmalloc_ioctl.h"

void* do_kmalloc(int fd, int size)
{
	kmalloc_arg_t q;
	q.size = size;
	if (ioctl(fd, PERFORM_KMALLOC, &q) == -1){perror("kmalloc_apps ioctl get");}
	return q.ptr;
}
void do_kfree(int fd, void *ptr)
{
	kmalloc_arg_t q;
	q.ptr = ptr;

	if (ioctl(fd, PERFORM_KFREE, &q) == -1){perror("kmalloc_apps ioctl set");}
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

	sleep(5);

	int i;
	for (i = 0; i < 10; i++){
		void *ptr = do_kmalloc(fd, 128);
		do_kfree(fd, ptr);
	}

	close (fd);

	return 0;
}