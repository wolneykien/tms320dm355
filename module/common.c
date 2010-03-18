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
  write_psc(base, offs, val);
  return read_psc(base, offs);
}

/* Tests the value of a given register (offset) with a bit-mask */
static int test_reg(unsigned long base,
		    unsigned long offs,
		    unsigned long mask)
{
  return read_psc(base, offs) & mask;
}

/* Reads the partial value of a register (offset) */
static unsigned long read_reg_part(unsigned long base,
				   unsigned long offs,
				   unsigned long mask)
{
  return read_reg(base, offs) & mask;
}

/* Sets the partial value of a given register (offset) */
static void write_reg_part(unsigned long base,
			   unsigned long offs,
			   unsigned long mask,
			   unsigned long val)
{
  write_psc(base, offs, (read_psc(base, offs) & ~mask) | (val & mask));
}

/* Sets and returns the partial value of a given register (offset) */
static unsigned long write_read_reg_part(unsigned long base,
					 unsigned long offs,
					 unsigned long mask,
					 unsigned long val)
{
  write_psc_part(base, offs, mask, val);
  return read_psc_part(base, offs, mask);
}

/* Sets the partial value of a given register (offset) and returns
 * the whole value */
static unsigned long write_reg_part_read(unsigned long base,
					 unsigned long offs,
					 unsigned long mask,
					 unsigned long val)
{
  write_psc_part(base, offs, mask, val);
  return read_psc(base, offs);
}
