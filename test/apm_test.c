#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include "../ioctl.h"

int main (int narg, char **argv, char **arge)
{
	int fd,ret,ctl;
	char arg;

	if (narg < 2)
	{
		arg = '-';
	} else {
		arg = argv[1][0];
	}

	switch (arg)
	{
		case 'b':
			ctl = APM_IOC_PLLM_BYPASS;
			break;
		case 'B':
			ctl = APM_IOC_PLLM_PLL;
			break;
		case 'c':
			ctl = APM_IOC_PLLM_91;
			break;
		case 'C':
			ctl = APM_IOC_PLLM_179;
			break;
		case 'u':
			ctl = APM_IOC_USB_OFF;
			break;
		case 'U':
			ctl = APM_IOC_USB_ON;
			break;
		case 'd':
			ctl = APM_IOC_DDRSR_ON;
			break;
		case 'D':
			ctl = APM_IOC_DDRSR_OFF;
			break;
		default:
			printf("Usage: %s <arg>. Possible args:\n\
\tb - PLL bypass mode on\n\
\tB - PLL bypass mode off\n\
\tc - minimal PLLM1 multiplier\n\
\tC - maximal PLLM1 multiplier\n\
\tu - power down USB PHY\n\
\tU - power up USB PHY\n\
\td - turn on DDR self-refresh (w/o DDR PHY power down)\n\
\tD - turn off DDR self-refresh\n\
\n",argv[0]);
			return 255;
	}

	fd = open("/dev/misc/apm_bios",O_NONBLOCK | O_RDWR);
	if (0 >= fd)
	{
		printf("can not open /dev/misc/apm_bios\n");
		return fd;
	}

	ret = ioctl(fd,ctl);
	if (0 != ret)
	{
		printf("error callin' ioctl\n");
		return ret;
	}

	return 0;
}
