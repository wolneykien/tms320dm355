#ifdef DEBUG
#define DBG(fmt,args...) printk(KERN_DEBUG DRIVER_NAME ": " fmt , ## args)
#define DBG_REG_FROM(name,val0) \
  printk(KERN_DEBUG DRIVER_NAME ": 0x%lx 0x%lx ->", \
	 (unsigned long)name, \
	 (unsigned long)val0)
#define DBG_REG_TO(val) printk(" 0x%lx\n", (unsigned long)val)
#else
#define DBG(fmt,args...) do { } while (0)
#define DBG_REG_FROM(name,val0) do { } while (0)
#define DBG_REG_TO(val) do { } while (0)
#endif

#define ERROR(fmt,args...) printk(KERN_ERR DRIVER_NAME ": " fmt , ## args)

#define __REG(x)        (*((volatile unsigned long *)IO_ADDRESS(x)))
