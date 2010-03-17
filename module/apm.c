
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/miscdevice.h>
#include "../ioctl.h"
#include <linux/device.h>
#include <linux/kernel.h>

#include <asm/system.h>
#include <asm/delay.h>
#include <mach/hardware.h>
#include <mach/io.h>

/* Include PSC memory map definitions */
#include "psc.h"

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
#define __REG(x)        (*((volatile unsigned long *)IO_ADDRESS(x)))

/* Returns the value of a given PSC register (offset) */
static unsigned long read_psc(unsigned long offs)
{
  return __REG(DAVINCI_PWR_SLEEP_CNTRL_BASE + offs);
}

/* Sets the value of a given PSC register (offset) */
static void write_psc(unsigned long offs, unsigned long val)
{
  __REG(DAVINCHI_PWR_SLEEP_CNTRL_BASE + offs) = val;
}

/* Sets and return the value of a given PSC register (offset) */
static unsigned long write_read_psc(unsigned long offs, unsigned long val)
{
  write_psc(offs, val);
  return read_psc(offs);
}

/* Tests the value of a given register (offset) with a bit-mask */
static int test_psc(unsigned long offs, unsigned long mask)
{
  return read_psc(offs) & mask;
}

/* Reads the partial value of a PSC register (offset) */
static unsigned long read_psc_part(unsigned long offs, unsigned long mask)
{
  return read_psc(offs) & mask;
}

/* Sets the partial value of a given PSC register (offset) */
static void write_psc_part(unsigned long offs,
			   unsigned long mask,
			   unsigned long val)
{
  write_psc(offs, read_psc(offs) & ~mask | val & mask);
}

/* Sets and returns the partial value of a given PSC register (offset) */
static unsigned long write_read_psc_part(unsignedlong offs,
					 unsigned long mask,
					 unsigned long val)
{
  write_psc_part(offs, mask, val);
  return read_psc_part(offs, mask);
}

/* Sets the partial value of a given PSC register (offset) and returns
 * the whole value */
static unsigned long write_psc_part_read(unsignedlong offs,
					 unsigned long mask,
					 unsigned long val)
{
  write_psc_part(offs, mask, val);
  return read_psc(offs);
}

/* Waits for a module state transition to finish up */
static int wait_for_transitions()
{
  while (test_psc(PTSTAT, PTSTAT_GOSTAT)) {
    mdelay(100);
  }

  return 0;
}

/* Sets the value for next state a given module (number) */
static void set_next_module_state(int mdnum, unsigned long mdstate)
{
  write_psc_part(MDCTL + REGISTER_SIZE * mdnum, MDCTL_NEXT, mdstate);
}

/* Initiates the module state transition */
static void start_module_state_transition()
{
  write_psc_part(PTCMD, PTCMD_GO, 0x1);
}

/* Transites a module (number) to a given state */
static int change_module_state(int mdnum, unsigned long mdstate)
{
  int rc;
  /* Wait for the GOSTATx bit in PTSTAT to clear to 0x0. You must wait
   * for any previously initiated transitions to finish before
   * initiating a new transition. */
  if ((rc = wait_for_transitions()) == 0) {
    /* Set the NEXT bit in MDCTL[x] to SwRstDisable (0x0),
     * SyncReset (0x1), Disable (0x2), or Enable (0x3). */
    set_next_module_state(mdnum, mdstate);
    /* Set the GOx bit in PTCMD to 0x1 to initiate the transition(s). */
    start_module_state_transition();
    /* Wait for the GOSTATx bit in PTSTAT to clear to 0x0. The module
     * is only safely in the new state after the GOSTATx bit in
     * PTSTAT clears to 0x0. */
    rc = wait_for_transitions();
  }

  return rc;
}

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
