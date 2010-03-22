
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/miscdevice.h>
#include <linux/device.h>
#include <linux/kernel.h>

#include <asm/system.h>
#include <linux/delay.h>
#include <mach/hardware.h>
#include <mach/io.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/stat.h>

/* Include PSC memory map definitions */
#include "pll.h"

#define DRIVER_NAME "pll"
#define PLL_MINOR 241

/* Common procedures */
#include "common.c"

static const char driver_version[] = "1.0";

/* Maximum number of wait cycles */ //TODO: module param
static int maxwait = 10;

/* Wait cycle period */
static int udelay_value = 400; //TODO: module param

/* Lock cycle period */
static int mdelay_value = 10; //TODO: module param

/* Transites the PLL controller to a new state */
static int pll_transaction(unsigned long base,
			   unsigned long refclk,
			   unsigned short mul)
{
  int n = 0;
  int rc;

  DBG("PLLCTL: 0x%lx\n", read_reg(base, PLLCTL));
  /* In PLLCTL, write CLKMODE = 0 (internal oscillator) or 1
   * (input clock) to select the type of reference clock. Write
   * CLKMODE = 0 for internal oscillator. Write CLKMODE = 1 for square
   * wave input clock. */
  DBG("Set CLKMODE to 0x%lx\n", refclk);
  write_reg_part(base, PLLCTL, PLLCTL_CLKMODE, refclk);
  /* In PLLCTL, write PLLENSRC = 0 (enable PLL enable). The bit PLLEN
   * in PLLCTL has no effect unless you write PLLENSRC = 0. */
  DBG("Set PLLENSRC to 0 (enable PLL enable)\n");
  write_reg_part(base, PLLCTL, PLLCTL_PLLENSRC, 0);
  /* In PLLCTL, write PLLEN = 0 (bypass mode). */
  DBG("Set PLLEN to 0 (bypass mode)\n");
  write_reg_part(base, PLLCTL, PLLCTL_PLLEN, 0);
  /* Wait at least 4 reference clock cycles for the PLLEN mux
   * to change.*/
  n = 0;
  do {
    DBG("Wait for the register to change\n");
    udelay(udelay_value);
    n++;
  } while ((rc = test_reg(base, PLLCTL, PLLCTL_PLLEN))
	   && n < maxwait);
  DBG("PLLCTL: 0x%lx\n", read_reg(base, PLLCTL));
  if (rc == 0) {
    /* In PLLCTL, write PLLRST = 1 (assert PLL reset). */
    DBG("Set PLLRST to 1 (assert PLL reset)\n");
    write_reg_part(base, PLLCTL, PLLCTL_PLLRST, 1);
    /* In PLLCTL, write PLLDIS = 1 (assert PLL disable). */
    DBG("Set PLLDIS to 1 (assert PLL disable)\n");
    write_reg_part(base, PLLCTL, PLLCTL_PLLDIS, 1);
    /* In PLLCTL, write PLLPWRDN = 0 (power up the PLL). */
    DBG("Set PLLPWRDN to 0 (power up the PLL)\n");
    write_reg_part(base, PLLCTL, PLLCTL_PWRDN, 0);
    /* In PLLCTL, write PLLDIS = 0 (de-assert PLL disable). */
    DBG("Set PLLDIS to 0 (de-assert PLL disable)\n");
    write_reg_part(base, PLLCTL, PLLCTL_PLLDIS, 0);
    /* If necessary, write PREDIV, POSTDIV, and PLLM to set divider
     * and multiplier values. */
    if (mul > 0) {
      DBG("Set PLLM to 0x%x\n", mul);
      write_reg_part(base, PLLM, PLLM_PLLM, mul);
    }
    /*  If necessary, write PLLDIV to set PLLDIVn dividers. Note that
     * you must apply the GO operation to change these dividers to new
     * ratios. */
    // TODO PLLDIVn, etc.
    /* Wait at least 5 miro-seconds for the PLL reset. */
    DBG("Wait for 5 us for PLL to reset\n");
    udelay(5);
    /* In PLLCTL, write PLLRST = 0 (de-assert PLL reset). */
    DBG("Set PLLRST to 0 (de-assert PLL reset)\n");
    write_reg_part(base, PLLCTL, PLLCTL_PLLRST, 0);
    /* Wait at least 8000 reference clock cycles for the PLL to lock. */
    DBG("Wait...\n");
    mdelay(mdelay_value); // TODO wait for what?
  } else {
    ERROR("PLLEN in not clear\n");
  }
  /* In PLLCTL, write PLLEN = 1 to (switch from bypass mode to PLL mode). */
  if (mul > 0) {
    DBG("Set PLLEN to 1 (PLL mode)\n");
    write_reg_part(base, PLLCTL, PLLCTL_PLLEN, 1);
  }

  DBG("Calibrating the delays...\n");
  calibrate_delay();
  DBG("BogoMIPS\t: %lu.%02lu\n\n",
      loops_per_jiffy / (500000UL/HZ),
      (loops_per_jiffy / (5000UL/HZ)) % 100);

  return 0;
}

/* Returns the current multiplicator value of a given PLLC (base) */
static unsigned long read_current_mul(unsigned long base)
{
  DBG("Read PLLM register\n");

  return read_reg_part(base, PLLM, PLLM_PLLM);
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
	.minor		= PLL_MINOR,
	.name		= "pll",
	.fops		= &pll_fops
};

struct kobject *pll_kobj;
struct kobject *pll1_kobj;
struct kobject *pll2_kobj;

static ssize_t power_state_show(struct kobject *kobj,
				struct kobj_attribute *attr,
				char *buf)
{
  unsigned long state;
  DBG("Read PLLPWRDN register\n");
  if (kobj->parent == pll1_kobj) {
    state = read_reg_part(PLLC1_BASE, PLLCTL, PLLCTL_PWRDN);
  } else if (kobj->parent == pll2_kobj) {
    state = read_reg_part(PLLC2_BASE, PLLCTL, PLLCTL_PWRDN);
  } else {
    ERROR("No base address for PLLC %s\n", kobj->name);
    return -ENOENT;
  }

  return sprintf(buf, "%d\n", state == 0 ? 1 : 0);
}

static ssize_t power_state_store(struct kobject *kobj,
				 struct kobj_attribute *attr,
				 const char *buf, size_t count)
{
  unsigned long state;

  if (count > 0) {
    if (count > 2) {
      return -ENOENT;
    }
    if (strncmp(buf, "0", 1) == 0) {
      state = 1;
    } else {
      state = 0;
    }
    DBG("Write PLLPWRDN register: %d\n", state);
    if (kobj->parent == pll1_kobj) {
      write_reg_part(PLLC1_BASE, PLLCTL, PLLCTL_PWRDN, state);
    } else if (kobj->parent == pll2_kobj) {
      write_reg_part(PLLC2_BASE, PLLCTL, PLLCTL_PWRDN, state);
    } else {
      ERROR("No base address for PLLC %s\n", kobj->name);
      return -ENOENT;
    }
  }

  return count;
}

static struct kobject* register_pll_kobj(struct kobject *parent,
					 const char *name)
{
  struct kobject *pll_kobj;
  struct kobject *power;
  static struct kobj_attribute power_state =
    __ATTR(state, 0644, power_state_show, power_state_store);
  static struct attribute * power_attrs[] =
    {
      &power_state.attr,
      NULL
    };
  static struct attribute_group power_attr_group =
    {
      .attrs = power_attrs,
    };

  DBG("Register the %s PLL kernel object\n", name);
  if ((pll_kobj = kobject_create_and_add(name, parent))) {
    DBG("Register the %s PLL power kernel object\n", name);
    if ((power = kobject_create_and_add("power", pll_kobj))) {
      DBG("Register %s PLL power attributes\n", name);
      if (sysfs_create_group(power,  &power_attr_group) != 0) {
	ERROR("Unable to register %s PLL power attributes\n", name);
      }
    } else {
      ERROR("Unable to create %s PLL power kernel object\n", name);
    }
  } else {
    ERROR("Unable to create %s PLL kernel object\n", name);
  }

  return pll_kobj;
}

static int __init pll_init(void)
{
	int ret;

	DBG("Register the device\n");
	if ((ret = misc_register(&pll_device)) != 0) {
	  ERROR("Unable to register the device\n");
	}
	DBG("Register the main PLL kernel object\n");
	if ((pll_kobj = kobject_create_and_add("pll", NULL))) {
	  pll1_kobj = register_pll_kobj(pll_kobj, "pllc1");
	  pll2_kobj = register_pll_kobj(pll_kobj, "pllc2");
	  if (pll1_kobj == NULL || pll2_kobj == NULL) {
	    ret = -ENOMEM;
	  }
	} else {
	  ERROR("Unable to create PLL kernel object\n");
	  ret = -ENOMEM;
	}

	return ret;
}

static void __exit pll_exit(void)
{
        DBG("Unregister the device\n");
	misc_deregister(&pll_device);
	if (pll_kobj) {
	  DBG("Unregister the PLL kernel object\n");
	  kobject_put(pll_kobj);
	  pll_kobj = NULL;
	}
}

module_init(pll_init);
module_exit(pll_exit);

MODULE_AUTHOR("Paul Wolneykien");
MODULE_DESCRIPTION("Clock control for ARM DaVinci");
MODULE_LICENSE("GPL");
