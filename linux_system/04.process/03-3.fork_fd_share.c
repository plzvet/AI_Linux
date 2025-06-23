#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#define BUF_SIZE	128

int main() {
	int fd;
	pid_t pid;
	char buf[BUF_SIZE];
	char *data="Good Morning";
	
	fd=open("file01", O_RDWR | O_APPEND, 0666);
	
	pid=fork();
	if(pid == 0){
		printf("Child FD = %d\n",fd);
		write(fd, data, strlen(data));
		sleep(1);
		lseek(fd, 0, SEEK_SET);
		read(fd, buf, sizeof(buf));
		printf("Child : %s\n", buf);
	}else{
		getchar();
		printf("Parent FD = %d\n",fd);
		lseek(fd, 12, SEEK_SET);
		read(fd, buf, sizeof(buf));
		printf("Parent : %s\n", buf);
	}
	return 0;
}

/*
* Initially, file01 assumes to contain "hello world" by echoing "hello world > file01"
* then, the output of both parent and chile would be "good morning"
*
* Above example shows that both write and read system call move file offset 
* to the point of ending of writing or reading.
* To see offset, check the return value of lseek.
*
* Any files opened after fork would not be shared between a child and a parent
*/
