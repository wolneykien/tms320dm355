#include "common.h"
#include <asm/io.h>

#define DEF_SIZE SZ_4K
#define MAX_MAPS 10
static int last_map = -1;
static struct memmap memmap[MAX_MAPS];

/* Returns a mapped address for a given base address */
static void *get_va(unsigned long base)
{
  int i;

  for(i = 0; i <= last_map; i++) {
    if (memmap[i].pa == base) {
      return memmap[i].va;
    }
  }
  last_map++;
  if (last_map < MAX_MAPS) {
    memmap[last_map].pa = base;
    DBG("Remap base address 0x%lx\n", base);
    if ((memmap[last_map].va = ioremap(base, DEF_SIZE)) == NULL) {
      ERROR("Unable to map base address 0x%lx\n", base);
    }
    return memmap[last_map].va;
  } else {
    ERROR("Memory map size exceeded\n");
    return NULL;
  }
}

/* Unmaps the memory map with the kernel */
static void unmap_addrs()
{
  int i;

  for(i = 0; i < last_map; i++) {
    DBG("Unmap base address 0x%lx\n", memmap[i].pa);
    iounmap(memmap[i].va);
  }
}

/* Returns the value of a given register (offset) */
static unsigned long read_reg(unsigned long base, unsigned long offs)
{
  unsigned long val;
  val = ioread32(get_va(base) + offs);
  DBG("(0x%lx)>: 0x%lx\n", base + offs, val);
  return val;
}

/* Sets the value of a given register (offset) */
static void write_reg(unsigned long base,
		      unsigned long offs,
		      unsigned long val)
{
  DBG("(0x%lx)<: 0x%lx\n", base + offs, val);
  iowrite32(val, get_va(base) + offs);
}

/* Sets and return the value of a given register (offset) */
static unsigned long write_read_reg(unsigned long base,
				    unsigned long offs,
				    unsigned long val)
{
  write_reg(base, offs, val);
  return read_reg(base, offs);
}

/* Tests the value of a given register (offset) with a bit-mask */
static int test_reg(unsigned long base,
		    unsigned long offs,
		    unsigned long mask)
{
  return read_reg(base, offs) & mask;
}

/* Shift a given value left to match a given mask */
static unsigned long shvall(unsigned long mask,
			    unsigned long val)
{
  if (mask) {
    while (! (mask & 0x01)) {
      mask = mask >> 1;
      val = val << 1;
    }
  }
  return val;
}

/* Shift a given value matching a given mask right to lower bits */
static unsigned long shvalr(unsigned long mask,
			    unsigned long val)
{
  if (mask) {
    while (! (mask & 0x01)) {
      mask = mask >> 1;
      val = val >> 1;
    }
  }
  return val;
}

/* Reads the partial value of a register (offset) */
static unsigned long read_reg_part(unsigned long base,
				   unsigned long offs,
				   unsigned long mask)
{
  return shvalr(mask, read_reg(base, offs) & mask);
}

/* Sets the partial value of a given register (offset) */
static void write_reg_part(unsigned long base,
			   unsigned long offs,
			   unsigned long mask,
			   unsigned long val)
{
  unsigned long oldval;
  unsigned long newval;

  oldval = read_reg(base, offs);
  newval = (oldval & ~mask) | (shvall(mask, val) & mask);
  write_reg(base, offs, newval);
}

/* Sets and returns the partial value of a given register (offset) */
static unsigned long write_read_reg_part(unsigned long base,
					 unsigned long offs,
					 unsigned long mask,
					 unsigned long val)
{
  write_reg_part(base, offs, mask, val);
  return read_reg_part(base, offs, mask);
}

/* Sets the partial value of a given register (offset) and returns
 * the whole value */
static unsigned long write_reg_part_read(unsigned long base,
					 unsigned long offs,
					 unsigned long mask,
					 unsigned long val)
{
  write_reg_part(base, offs, mask, val);
  return read_reg(base, offs);
}

/* Releases the specified device */
static int misc_release(struct inode * inode, struct file * filp)
{
        DBG("Release the device\n");
	return 0;
}

/* Opens the specified device */
static int misc_open(struct inode * inode, struct file * filp)
{
        DBG("Open the device\n");
        return 0;
}
