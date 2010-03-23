#include "common.h"

/* Returns the value of a given register (offset) */
static unsigned long read_reg(unsigned long base, unsigned long offs)
{
  return __REG(base + offs);
}

/* Sets the value of a given register (offset) */
static void write_reg(unsigned long base,
		      unsigned long offs,
		      unsigned long val)
{
  __REG(base + offs) = val;
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
  write_reg(base,
	    offs,
	    (read_reg(base, offs) & ~mask) \
	    | (shvall(mask, val) & mask));
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
