
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/miscdevice.h>
#include "../ioctl.h"
#include <linux/device.h>
#include <linux/kernel.h>

#include <asm/system.h>
#include <asm/delay.h>
#include <asm/arch/hardware.h>

#define DRIVER_NAME "apm"

#ifdef DEBUG
#define DBG(fmt,args...) printk(KERN_DEBUG DRIVER_NAME ": " fmt , ## args)
#define DBG_REG_FROM(name,val0) \
  printk(KERN_DEBUG DRIVER_NAME ": 0x%x 0x%lx ->", name, val0)
#define DBG_REG_TO(val) printk(" 0x%lx\n", val)
#else
#define DBG(fmt,args...) do { } while (0)
#define DBG_REG_FROM(name,val0) do { } while (0)
#define DBG_REG_TO(val) do { } while (0)
#endif

#define ERROR(fmt,args...) printk(KERN_ERR DRIVER_NAME ": " fmt , ## args)

/*
 * The apm_bios device is one of the misc char devices.
 * This is its minor number.
 */
#define APM_MINOR_DEV	134

static const char driver_version[] = "1.1";	/* no spaces */

/*
 * apm_ioctl - handle APM ioctl
 *
 */

#define PDCTL1_ADDR 0x01c40900
#define PDCTL1      __REG(PDCTL1_ADDR)
#define PLL1_ADDR   0x01c40910
#define PLL1_PLLM   __REG(PLL1_ADDR)

// PDCTL register bits
#define PLLEN		0
#define PLLPWRDN	1
#define PLLRST		3
#define PLLDIS		4
#define PLLENSRC	5
#define CLKMODE		8

/*
 *  sprueh7d.pdf 
 *  table 22, page 46
 */
#define SDRCR_ADDR  0x2000000c
#define SDRCR       __REG(SDRCR_ADDR)

/*
 *  sprufb3.pdf
 *  table 9-3, page 117
 */
#define USBCTL_ADDR 0x01c40034
#define USBCTL      __REG(USBCTL_ADDR)

static int
apm_ioctl(struct inode * inode, struct file *filp, u_int cmd, u_long arg)
{
        DBG("I/O cmd %d\n", cmd);

	switch (cmd) {
	case APM_IOC_DDRSR_ON:
		SDRCR &= ~(1 << 23);
		SDRCR |= (1 << 31);
		SDRCR |= (1 << 30);
		break;
	case APM_IOC_DDRSR_OFF:
		SDRCR &= ~(1 << 30);
		SDRCR &= ~(1 << 31);
		break;
	case APM_IOC_PLLM_BYPASS:
		PDCTL1 &= ~(1 << PLLEN);
		udelay(10);
		PDCTL1 |= (1 << PLLRST);
		break;
	case APM_IOC_PLLM_PLL:
		PDCTL1 &= ~(1 << CLKMODE);
		PDCTL1 &= ~(1 << PLLENSRC);
		PDCTL1 &= ~(1 << PLLEN);
		udelay(10);
		PDCTL1 |= (1 << PLLRST);
		PDCTL1 |= (1 << PLLDIS);
		PDCTL1 &= ~(1 << PLLPWRDN);
		PDCTL1 &= ~(1 << PLLDIS);
		udelay(10);
		PDCTL1 &= ~(1 << PLLRST);
		udelay(300);
		PDCTL1 |= (1 << PLLEN);;
		break;
	case APM_IOC_PLLM_91:
		PDCTL1 &= ~(1 << PLLEN );
		udelay(10);
		PDCTL1 |= (1 << PLLRST);
		PLL1_PLLM = 91;
		udelay(10);
		PDCTL1 &= ~(1 << PLLRST);
		udelay(300);
		PDCTL1 |= (1 << PLLEN);
		break;
	case APM_IOC_PLLM_179:
		PDCTL1 &= ~(1 << PLLEN );
		udelay(10);
		PDCTL1 |= (1 << PLLRST);
		PLL1_PLLM = 179;
		udelay(10);
		PDCTL1 &= ~(1 << PLLRST);
		udelay(300);
		PDCTL1 |= (1 << PLLEN);
		break;
	case APM_IOC_USB_OFF:
		USBCTL |= 1;
		break;
	case APM_IOC_USB_ON:
		USBCTL &= ~( 1);
		break;
	default:
	  return -EINVAL;
	}

	return 0;
}

static int apm_release(struct inode * inode, struct file * filp)
{
        DBG("Release the device\n");
	return 0;
}

static int apm_open(struct inode * inode, struct file * filp)
{
        DBG("Open the device\n");
        return 0;
}

static struct file_operations apm_bios_fops = {
	.owner		= THIS_MODULE,
	.ioctl		= apm_ioctl,
	.open		= apm_open,
	.release	= apm_release,
};

static struct miscdevice apm_device = {
	.minor		= APM_MINOR_DEV,
	.name		= "apm_bios",
	.fops		= &apm_bios_fops
};


static int __init apm_init(void)
{
	int ret;

	DBG("Register the device\n");
	if ((ret = misc_register(&apm_device)) != 0) {
	  ERROR("Unable to register the device\n");
	}

	return ret;
}

static void __exit apm_exit(void)
{
        DBG("Unregister the device\n");
	misc_deregister(&apm_device);
}

module_init(apm_init);
module_exit(apm_exit);

MODULE_AUTHOR("Peter Saveliev");
MODULE_DESCRIPTION("Advanced Power Management for ARM DaVinci");
MODULE_LICENSE("GPL");
