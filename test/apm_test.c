#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

#define DEV "/dev/misc/apm_bios"

int main (int narg, char **argv, char **arge)
{
  int fd;
  int mdnum;
  unsigned long mdstate;
  int rc;

  if (narg == 3) {
    mdnum = atoi(argv[1]);
    mdstate = (unsigned long)atoi(argv[2]);
    fd = open(DEV, O_NONBLOCK | O_RDWR);
    if (fd <= 0) {
      printf("Can not open %s (%d)\n", DEV, fd);
      rc = fd;
    }
    rc = ioctl(fd, mdnum, mdstate);
    if (rc != 0) {
      printf("Error calling I/O command %d 0x%lx\n", mdnum, mdstate);
    }
  } else {
    rc = 0xff;
    printf("Usage: <module-number> <module-state>\n");
  }
}
