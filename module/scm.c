
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
#include <asm/io.h>

/* Include SCM memory map definitions */
#include "scm.h"

#define DRIVER_NAME "scm"

/* Common procedures */
#include "common.c"

static const char driver_version[] = "1.0";

/* USB PHY syfs objects */

static ssize_t usb_power_state_show(struct kobject *kobj,
				    struct kobj_attribute *attr,
				    char *buf)
{
  unsigned long state;

  state = read_reg_part(SCM_BASE, USB_PHY_CTRL, USB_PHYPDWN);
  DBG("USB_PHYPWDN: 0x%lx\n", state);
  return sprintf(buf, "%d\n", (state == 0 ? 1 : 0));
}

static ssize_t usb_power_state_store(struct kobject *kobj,
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

    DBG("USB_PHYPWDN: 0x%lx -> 0x%lx\n",
	read_reg_part(SCM_BASE, USB_PHY_CTRL, USB_PHYPDWN),
	state);
    write_reg_part(SCM_BASE, USB_PHY_CTRL, USB_PHYPDWN, state);
  }

  return count;
}

struct kobject *usb_kobj;
struct kobject *usb_power;
static struct kobj_attribute usb_power_state =
  __ATTR(state, 0644, usb_power_state_show, usb_power_state_store);
static struct attribute * usb_attrs[] =
  {
    &usb_power_state.attr,
    NULL
  };
static struct attribute_group usb_attr_group =
  {
    .attrs = usb_attrs,
  };

static ssize_t wi_show(struct kobject *kobj,
		       struct kobj_attribute *attr,
		       char *buf)
{
  return sprintf(buf, "%d\n", 1);
}

void __wait_for_interrupt(void);

static ssize_t wi_store(struct kobject *kobj,
			struct kobj_attribute *attr,
			const char *buf, size_t count)
{
  if (count > 0) {
    if (count > 2) {
      return -ENOENT;
    }
    if (strncmp(buf, "0", 1) == 0) {
      DBG("Entering wait-for-interrupt mode\n");
      __wait_for_interrupt();
    }
  }

  return count;
}

struct kobject *clock_kobj;
static struct kobj_attribute clock_state =
  __ATTR(state, 0644, wi_show, wi_store);
static struct attribute * clock_attrs[] =
  {
    &clock_state.attr,
    NULL
  };
static struct attribute_group clock_attr_group =
  {
    .attrs = clock_attrs,
  };


static ssize_t ddr_power_show(struct kobject *kobj,
			      struct kobj_attribute *attr,
			      char *buf)
{
  unsigned long state;

  DBG("SDRCR: 0x%lx\n", read_reg(DDR_CTL_BASE, SDRCR));
  state = 2 - read_reg_part(DDR_CTL_BASE, SDRCR, SDRCR_SR_PD) \
    - read_reg_part(DDR_CTL_BASE, SDRCR, SDRCR_LPMODEN);
  return sprintf(buf, "%ld\n", state);
}

static ssize_t ddr_power_store(struct kobject *kobj,
			       struct kobj_attribute *attr,
			       const char *buf, size_t count)
{
  if (count > 0) {
    if (count > 2) {
      return -ENOENT;
    }
    if (strncmp(buf, "0", 1) == 0) {
      DBG("Entering DDR power-down mode\n");
      write_reg_part(DDR_CTL_BASE, SDRCR, SDRCR_SR_PD, 1);
      write_reg_part(DDR_CTL_BASE, SDRCR, SDRCR_LPMODEN, 1);
    } else if (strncmp(buf, "1", 1) == 0) {
      DBG("Entering DDR self-refresh mode\n");
      write_reg_part(DDR_CTL_BASE, SDRCR, SDRCR_SR_PD, 0);
      write_reg_part(DDR_CTL_BASE, SDRCR, SDRCR_LPMODEN, 1);
    } else {
      DBG("Entering normal DDR mode\n");
      write_reg_part(DDR_CTL_BASE, SDRCR, SDRCR_SR_PD, 0);
      write_reg_part(DDR_CTL_BASE, SDRCR, SDRCR_LPMODEN, 0);
    }
  }

  return count;
}

struct kobject *ddr_kobj;
static struct kobj_attribute ddr_power_state =
  __ATTR(state, 0644, ddr_power_show, ddr_power_store);
static struct attribute * ddr_attrs[] =
  {
    &ddr_power_state.attr,
    NULL
  };
static struct attribute_group ddr_attr_group =
  {
    .attrs = ddr_attrs,
  };

static struct kobject* register_kobj(struct kobject *parent,
				     const char *name,
				     struct attribute_group *attr_group)
{
  struct kobject *kobj;
  
  
  DBG("Register the %s kernel object\n", name);
  if ((kobj = kobject_create_and_add(name, parent))) {
    DBG("Register %s attributes\n", name);
    if (sysfs_create_group(kobj,  attr_group) != 0) {
      ERROR("Unable to register %s attributes\n", name);
    }
  } else {
    ERROR("Unable to create %s kernel object\n", name);
  }

  return kobj;
}

struct kobject *arm_kobj;

static int __init scm_init(void)
{
  struct kobject *kobj = NULL;

  DBG("Register the kernel objects\n");
  if ((arm_kobj = kobject_create_and_add("arm", NULL))) {
    if ((kobj = kobject_create_and_add("usb", arm_kobj))) {
      kobj = register_kobj(kobj, "power", &usb_attr_group);
    }
  }

  if (kobj) {
    kobj = register_kobj(arm_kobj, "clock", &clock_attr_group);
  }

  if (kobj) {
    if ((kobj = kobject_create_and_add("ddr", arm_kobj))) {
      kobj = register_kobj(kobj, "power", &ddr_attr_group);
    }
  }

  if (arm_kobj == NULL || kobj == NULL) {
    return -ENOMEM;
  } else {
    return 0;
  }
}

static void __exit scm_exit(void)
{
  DBG("Unregister the kernel objects\n");
  kobject_put(arm_kobj);
  arm_kobj = NULL;
}

module_init(scm_init);
module_exit(scm_exit);

MODULE_AUTHOR("Paul Wolneykien");
MODULE_DESCRIPTION("System control module for ARM DaVinci");
MODULE_LICENSE("GPL");
