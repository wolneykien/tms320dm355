#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

#define DEV "/dev/misc/pll"

int main (int narg, char **argv, char **arge)
{
  int fd;
  int pllnum;
  unsigned long mul;
  int rc;

  if (narg >= 2) {
    pllnum = atoi(argv[1]);
    if (narg > 2) {
      mul = (unsigned long)atoi(argv[2]);
    } else {
      mul = 0;
    }
    fd = open(DEV, O_NONBLOCK | O_RDWR);
    if (fd <= 0) {
      printf("Can not open %s (%d)\n", DEV, fd);
      rc = fd;
    }
    rc = ioctl(fd, pllnum, mul);
    printf("Result: 0x%lx\n", rc);
  } else {
    rc = 0xff;
    printf("Usage: <PLL-number> [<multiplier>]\n");
  }
}
