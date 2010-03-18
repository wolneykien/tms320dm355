
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
#include "pll.h"

#define DRIVER_NAME "pll"

/* Common procedures */
#include "common.c"

static const char driver_version[] = "1.0";

/* Transites the PLL controller to a new state */
static int pll_transaction(unsigned long refclk,
			   int bypass,
			   byte mul)
{
  /* In PLLCTL, write CLKMODE = 0 (internal oscillator) or 1
   * (input clock) to select the type of reference clock. Write
   * CLKMODE = 0 for internal oscillator. Write CLKMODE = 1 for square
   * wave input clock. */
  write_reg_part(PLLC1_BASE, PLLCTL, PLLCTL_CLKMODE, refclk);
  /* In PLLCTL, write PLLENSRC = 0 (enable PLL enable). The bit PLLEN
   * in PLLCTL has no effect unless you write PLLENSRC = 0. */
  write_reg_part(PLLC1_BASE, PLLCTL, PLLCTL_PLLENSRC, 0);
  /* In PLLCTL, write PLLEN = 0 (bypass mode). */
  write_reg_part(PLLC1_BASE, PLLCTL, PLLCTL_PLLEN, 0);
  /* Wait at least 4 reference clock cycles for the PLLEN mux
   * to change.*/
  do { udelay(400) } while (!test_reg(PLLC1_BASE, PLLCTL, PLLCTL_PLLEN));
  /* In PLLCTL, write PLLRST = 1 (assert PLL reset). */
  write_reg_part(PLLC1_BASE, PLLCTL, PLLCTL_PLLRST, 1);
  /* In PLLCTL, write PLLDIS = 1 (assert PLL disable). */
  write_reg_part(PLLC1_BASE, PLLCTL, PLLCTL_PLLDIS, 1);
  /* In PLLCTL, write PLLPWRDN = 0 (power up the PLL). */
  write_reg_part(PLLC1_BASE, PLLCTL, PLLCTL_PLLPWRDN, 0);
  /* In PLLCTL, write PLLDIS = 0 (de-assert PLL disable). */
  write_reg_part(PLLC1_BASE, PLLCTL, PLLCTL_PLLDIS, 0);
  /* If necessary, write PREDIV, POSTDIV, and PLLM to set divider
   * and multiplier values. */
  write_reg_part(PLLC1_BASE, PLLM, PLLM_PLLM, mul);
  /*  If necessary, write PLLDIV to set PLLDIVn dividers. Note that
   * you must apply the GO operation to change these dividers to new
   * ratios. */
  // TODO PLLDIVn, etc.
  /* Wait at least 5 miro-seconds for the PLL reset. */
  udelay(5);
  /* In PLLCTL, write PLLRST = 0 (de-assert PLL reset). */
  write_reg_part(PLLC1_BASE, PLLCTL, PLLCTL_PLLRST, 0);
  /* Wait at least 8000 reference clock cycles for the PLL to lock. */
  mdelay(100); // TODO wait for what?
  /* In PLLCTL, write PLLEN = 1 to (switch from bypass mode to PLL mode). */
  if (!bypass) {
    write_reg_part(PLLC1_BASE, PLLCTL, PLLCTL_PLLEN, 1);
  }

  return 0;
}

/*
 * psc_ioctl - handle PSC ioctl
 *
 */
static int
psc_ioctl(struct inode * inode, struct file *filp, u_int cmd, u_long arg)
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

static int misc_release(struct inode * inode, struct file * filp)
{
        DBG("Release the device\n");
	return 0;
}

static int misc_open(struct inode * inode, struct file * filp)
{
        DBG("Open the device\n");
        return 0;
}

static struct file_operations psc_fops = {
	.owner		= THIS_MODULE,
	.ioctl		= psc_ioctl,
	.open		= misc_open,
	.release	= misc_release,
};

static struct miscdevice psc_device = {
	.minor		= MISC_DYNAMIC_MINOR,
	.name		= "psc",
	.fops		= &psc_fops
};


static int __init psc_init(void)
{
	int ret;

	DBG("Register the device\n");
	if ((ret = misc_register(&psc_device)) != 0) {
	  ERROR("Unable to register the device\n");
	}

	return ret;
}

static void __exit psc_exit(void)
{
        DBG("Unregister the device\n");
	misc_deregister(&psc_device);
}

module_init(psc_init);
module_exit(psc_exit);

MODULE_AUTHOR("Paul Wolneykien");
MODULE_DESCRIPTION("Power and clock control for ARM DaVinci");
MODULE_LICENSE("GPL");
