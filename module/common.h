#ifdef DEBUG
#define DBG(fmt,args...) printk(KERN_DEBUG DRIVER_NAME ": " fmt , ## args)
#else
#define DBG(fmt,args...) do { } while (0)
#endif

#define ERROR(fmt,args...) printk(KERN_ERR DRIVER_NAME ": " fmt , ## args)

/* Holds the memory map entry */
struct memmap {
  unsigned long pa;
  void *va;
};
