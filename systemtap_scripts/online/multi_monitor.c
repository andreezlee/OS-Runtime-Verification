#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>

#include "monitor_module.h"

// Keeps track of events seen so far
static int event_counter = 0;

int main()
{
	// Connect to monitor module device
	char *file_name = "/dev/rv_monitor";
	int fd;
	fd = open(file_name, O_RDWR);
	if (fd == -1)
	{
		perror("monitors already open");
		return 2;
	}

	while(1){
		monitor_arg_t q;
		if (ioctl(fd, READ_NEW_EVENT, &q) < 0){
			perror("cannot read new event");
		}else{
			event_counter = q.index;
			printf("received new event #%d\n", event_counter);
		}

		if (ioctl(fd, CONFIRMATION, &q) < 0){
			perror("confirmation not accepted");
		}
	}

	
	close (fd);

	return 0;
}
