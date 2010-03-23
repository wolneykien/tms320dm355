
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/miscdevice.h>
#include <linux/device.h>
#include <linux/kernel.h>

#include <asm/system.h>
#include <linux/delay.h>
#include <mach/hardware.h>

/* Include PSC memory map definitions */
#include "psc.h"

#define DRIVER_NAME "psc"
#define PSC_MINOR 240

/* Common procedures */
#include "common.c"

static const char driver_version[] = "1.2";

#define test_psc(offs, mask) \
  test_reg(DAVINCI_PWR_SLEEP_CNTRL_BASE, offs, mask)
#define read_psc_part(offs, mask) \
  read_reg_part(DAVINCI_PWR_SLEEP_CNTRL_BASE, offs, mask)
#define write_psc_part(offs, mask, val)			\
  write_reg_part(DAVINCI_PWR_SLEEP_CNTRL_BASE, offs, mask, val)

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

static struct file_operations psc_fops = {
	.owner		= THIS_MODULE,
	.ioctl		= psc_ioctl,
	.open		= misc_open,
	.release	= misc_release,
};

static struct miscdevice psc_device = {
	.minor		= PSC_MINOR,
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
	unmap_addrs();
}

module_init(psc_init);
module_exit(psc_exit);

MODULE_AUTHOR("Paul Wolneykien");
MODULE_DESCRIPTION("Power control for ARM DaVinci");
MODULE_LICENSE("GPL");
