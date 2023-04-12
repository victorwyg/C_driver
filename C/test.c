#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(void)
{
	int fd;
	fd = open("./123",O_RDWR);
	if(-1 == fd)
	{
		printf("open error\n");
		return 1;
	}
	printf("open succe\n");
	close(fd);
	return 0;
}
