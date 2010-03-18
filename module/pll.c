
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/miscdevice.h>
#include <linux/device.h>
#include <linux/kernel.h>

#include <asm/system.h>
#include <asm/arch/hardware.h>
#include <linux/delay.h>

/* Include PSC memory map definitions */
#include "pll.h"

#define DRIVER_NAME "pll"

/* Common procedures */
#include "common.c"

static const char driver_version[] = "1.0";

/* Transites the PLL controller to a new state */
static int pll_transaction(unsigned long base,
			   unsigned long refclk,
			   unsigned short mul)
{
  /* In PLLCTL, write CLKMODE = 0 (internal oscillator) or 1
   * (input clock) to select the type of reference clock. Write
   * CLKMODE = 0 for internal oscillator. Write CLKMODE = 1 for square
   * wave input clock. */
  write_reg_part(base, PLLCTL, PLLCTL_CLKMODE, refclk);
  /* In PLLCTL, write PLLENSRC = 0 (enable PLL enable). The bit PLLEN
   * in PLLCTL has no effect unless you write PLLENSRC = 0. */
  write_reg_part(base, PLLCTL, PLLCTL_PLLENSRC, 0);
  /* In PLLCTL, write PLLEN = 0 (bypass mode). */
  write_reg_part(base, PLLCTL, PLLCTL_PLLEN, 0);
  /* Wait at least 4 reference clock cycles for the PLLEN mux
   * to change.*/
  do { udelay(400); } while (!test_reg(base, PLLCTL, PLLCTL_PLLEN));
  /* In PLLCTL, write PLLRST = 1 (assert PLL reset). */
  write_reg_part(base, PLLCTL, PLLCTL_PLLRST, 1);
  /* In PLLCTL, write PLLDIS = 1 (assert PLL disable). */
  write_reg_part(base, PLLCTL, PLLCTL_PLLDIS, 1);
  /* In PLLCTL, write PLLPWRDN = 0 (power up the PLL). */
  write_reg_part(base, PLLCTL, PLLCTL_PWRDN, 0);
  /* In PLLCTL, write PLLDIS = 0 (de-assert PLL disable). */
  write_reg_part(base, PLLCTL, PLLCTL_PLLDIS, 0);
  /* If necessary, write PREDIV, POSTDIV, and PLLM to set divider
   * and multiplier values. */
  if (mul > 0) {
    write_reg_part(base, PLLM, PLLM_PLLM, mul);
  }
  /*  If necessary, write PLLDIV to set PLLDIVn dividers. Note that
   * you must apply the GO operation to change these dividers to new
   * ratios. */
  // TODO PLLDIVn, etc.
  /* Wait at least 5 miro-seconds for the PLL reset. */
  udelay(5);
  /* In PLLCTL, write PLLRST = 0 (de-assert PLL reset). */
  write_reg_part(base, PLLCTL, PLLCTL_PLLRST, 0);
  /* Wait at least 8000 reference clock cycles for the PLL to lock. */
  mdelay(100); // TODO wait for what?
  /* In PLLCTL, write PLLEN = 1 to (switch from bypass mode to PLL mode). */
  if (mul == 0) {
    write_reg_part(base, PLLCTL, PLLCTL_PLLEN, 1);
  }

  return 0;
}

/*
 * pll_ioctl - handle PLL ioctl
 *
 */
static int
pll_ioctl(struct inode * inode, struct file *filp, u_int cmd, u_long arg)
{
  int rc;

  if (cmd == 1) {
    DBG("Start transaction for the PLLC1\n");
    rc = pll_transaction(PLLC1_BASE, 1, arg);
  } else {
    DBG("Start transaction for the PLLC2\n");
    rc = pll_transaction(PLLC2_BASE, 1, arg);
  }

  return rc;
}


static struct file_operations pll_fops = {
	.owner		= THIS_MODULE,
	.ioctl		= pll_ioctl,
	.open		= misc_open,
	.release	= misc_release,
};

static struct miscdevice pll_device = {
	.minor		= MISC_DYNAMIC_MINOR,
	.name		= "pll",
	.fops		= &pll_fops
};


static int __init pll_init(void)
{
	int ret;

	DBG("Register the device\n");
	if ((ret = misc_register(&pll_device)) != 0) {
	  ERROR("Unable to register the device\n");
	}

	return ret;
}

static void __exit pll_exit(void)
{
        DBG("Unregister the device\n");
	misc_deregister(&pll_device);
}

module_init(pll_init);
module_exit(pll_exit);

MODULE_AUTHOR("Paul Wolneykien");
MODULE_DESCRIPTION("Clock control for ARM DaVinci");
MODULE_LICENSE("GPL");
