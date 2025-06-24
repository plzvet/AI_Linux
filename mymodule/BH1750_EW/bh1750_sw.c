#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

int main(int argc, char **argv)
{
  int fd = open("/dev/bhdev", O_RDONLY);
  int lux10;

  if (fd < 0)
  {
    perror("open");
    return 1;
  }
  
    while(1)
    {
            if (read(fd, &lux10, sizeof(lux10)) != sizeof(lux10)) {
            perror("read");
            close(fd);
            return 1;

        }
        printf("조도: %d lux\n", lux10);
    }

    close(fd);

  return 0;

}