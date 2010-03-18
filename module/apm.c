
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/miscdevice.h>
#include <linux/device.h>
#include <linux/kernel.h>

#include <asm/system.h>
#include <linux/delay.h>
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
  __REG(DAVINCI_PWR_SLEEP_CNTRL_BASE + offs) = val;
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
  write_psc(offs, (read_psc(offs) & ~mask) | (val & mask));
}

/* Sets and returns the partial value of a given PSC register (offset) */
static unsigned long write_read_psc_part(unsigned long offs,
					 unsigned long mask,
					 unsigned long val)
{
  write_psc_part(offs, mask, val);
  return read_psc_part(offs, mask);
}

/* Sets the partial value of a given PSC register (offset) and returns
 * the whole value */
static unsigned long write_psc_part_read(unsigned long offs,
					 unsigned long mask,
					 unsigned long val)
{
  write_psc_part(offs, mask, val);
  return read_psc(offs);
}

/* Waits for a module state transition to finish up */
static int wait_for_transitions(void)
{
  int rc;

  while ((rc = test_psc(PTSTAT, PTSTAT_GOSTAT))) {
    DBG("Wait while for a current power transition to finish\n");
    mdelay(100);
  }

  return rc;
}

/* Sets the value for next state a given module (number) */
static void set_next_module_state(int mdnum, unsigned long mdstate)
{
  write_psc_part(MDCTL + REGISTER_SIZE * mdnum, MDCTL_NEXT, mdstate);
}

/* Initiates the module state transition */
static void start_module_state_transition(void)
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
    DBG("Set 0x%lx state for the module #%d\n", mdstate, mdnum);
    set_next_module_state(mdnum, mdstate);
    /* Set the GOx bit in PTCMD to 0x1 to initiate the transition(s). */
    DBG("Start power state transition for the module #%d\n", mdnum);
    start_module_state_transition();
    /* Wait for the GOSTATx bit in PTSTAT to clear to 0x0. The module
     * is only safely in the new state after the GOSTATx bit in
     * PTSTAT clears to 0x0. */
    rc = wait_for_transitions();
  }

  return rc;
}

/* Returns the power state of a module (number) */
static unsigned long get_module_state(int mdnum)
{
  return read_psc_part(MDSTAT + REGISTER_SIZE * mdnum, MDSTAT_STATE);
}

static int
apm_ioctl(struct inode * inode, struct file *filp, u_int cmd, u_long arg)
{
  int rc;

  if (arg <= PSC_MD_ENABLE) {
    DBG("Try to set state 0x%lx for the module #%d\n", arg, cmd);
    rc = change_module_state(cmd, arg);
  } else {
    DBG("Read the current state of the module #%d\n", cmd);
    rc = (int) get_module_state(cmd);
  }

  return rc;
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
