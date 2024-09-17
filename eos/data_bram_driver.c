#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/cdev.h>
#include <linux/kdev_t.h>
#include <linux/uaccess.h>
#include <linux/errno.h>
#include <linux/signal.h>
#include <linux/device.h>
#include <linux/poll.h>
#include <linux/of.h>
#include <linux/math64.h>

#include <linux/io.h>
#include <linux/slab.h>
#include <linux/platform_device.h>
#include <linux/ioport.h>

#include <linux/interrupt.h>

#define BUFF_SIZE 40
#define INSTR_BRAM_SIZE 1024
#define DRIVER_NAME "data_bram"
MODULE_LICENSE("Dual BSD/GPL");

struct instr_info {
  unsigned long mem_start;
  unsigned long mem_end;
  void __iomem *base_addr;
};

dev_t my_dev_id;
static struct class *my_class;
static struct device *my_device;
static struct cdev *my_cdev;
static struct instr_info *lp = NULL;

int endRead = 0;

static int instr_probe(struct platform_device *pdev);
static int instr_remove(struct platform_device *pdev);
int instr_open(struct inode *pinode, struct file *pfile);
int instr_close(struct inode *pinode, struct file *pfile);
ssize_t instr_read(struct file *pfile, char __user *buffer, size_t length, loff_t *offset);
ssize_t instr_write(struct file *pfile, const char __user *buffer, size_t length, loff_t *offset);
static int __init instr_init(void);
static void __exit instr_exit(void);

struct file_operations my_fops =
{
	.owner = THIS_MODULE,
	.open = instr_open,
	.read = instr_read,
	.write = instr_write,
	.release = instr_close,
};

static struct of_device_id instr_of_match[] = {
  //{ .compatible = "xlnx,axi-bram-ctrl-4.1", },
    { .compatible = "xlnx,axi-bram-ctrl-4.1", .data = (void *)0x42000000 },  
{ /* end of list */ },
};

static struct platform_driver instr_driver = {
  .driver = {
    .name = DRIVER_NAME,
    .owner = THIS_MODULE,
    .of_match_table	= instr_of_match,
  },
  .probe		= instr_probe,
  .remove		= instr_remove,
};


MODULE_DEVICE_TABLE(of, instr_of_match);

static int instr_probe(struct platform_device *pdev)
{
  struct resource *r_mem;
  int rc = 0;
  r_mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);
  if (!r_mem) {
    printk(KERN_ALERT "Failed to get resource\n");
    return -ENODEV;
  }
  lp = (struct instr_info *) kmalloc(sizeof(struct instr_info), GFP_KERNEL);
  if (!lp) {
    printk(KERN_ALERT "Could not allocate cpu device\n");
    return -ENOMEM;
  }

  lp->mem_start = r_mem->start;
  lp->mem_end = r_mem->end;
  printk(KERN_INFO "Start address:%x \t end address:%x", r_mem->start, r_mem->end);

  if (!request_mem_region(lp->mem_start,lp->mem_end - lp->mem_start + 1,        DRIVER_NAME))
  {
    printk(KERN_ALERT "Could not lock memory region at %p\n",(void *)lp->mem_start);
    rc = -EBUSY;
    goto error1;
  }

  lp->base_addr = ioremap(lp->mem_start, lp->mem_end - lp->mem_start + 1);
  if (!lp->base_addr) {
    printk(KERN_ALERT "Could not allocate memory\n");
    rc = -EIO;
    goto error2;
  }

  printk(KERN_WARNING "data_mem platform driver registered\n");
  printk(KERN_INFO "Starting address is: %X8 \n", lp->mem_start);
  return 0;//ALL OK

error2:
  release_mem_region(lp->mem_start, lp->mem_end - lp->mem_start + 1);
error1:
  return rc;
}

static int instr_remove(struct platform_device *pdev)
{
  printk(KERN_WARNING "Data bram platform driver removed\n");
  iowrite32(0, lp->base_addr);
  iounmap(lp->base_addr);
  printk(KERN_INFO "Half way removing");
  kfree(lp);
  return 0;
}

int instr_open(struct inode *pinode, struct file *pfile) 
{
	printk(KERN_INFO "Succesfully opened data bram\n");
	return 0;
}

int instr_close(struct inode *pinode, struct file *pfile) 
{
	printk(KERN_INFO "Succesfully closed data bram\n");
	return 0;
}

ssize_t instr_read(struct file *pfile, char __user *buffer, size_t length, loff_t *offset) 
{
	int ret;
	unsigned int addr = 0;
	int len = 0;
	u32 value = 0;
	int i = 0;
	char buff[BUFF_SIZE];
	if (endRead){
		endRead = 0;
		return 0;
	}

	while (addr < INSTR_BRAM_SIZE) {
		value = ioread32(lp->base_addr + addr);

		for(i=0;i<32;i++)
		{
			if((value >> i) & 0x01)
				buff[31-i] = '1';
			else
				buff[31-i] = '0';
		}

		buff[32] = '\n';
		len = 33;
		ret = copy_to_user(buffer, buff, len);
		if(ret)
			return -EFAULT;

		addr += 4;
	}
	
	printk(KERN_INFO "Succesfully read\n");
	endRead = 1;

	return len;
}

ssize_t instr_write(struct file *pfile, const char __user *buffer, size_t length, loff_t *offset)
{
    char buff[BUFF_SIZE];
    int ret;
    unsigned int value;
    unsigned int addr;

    if (length >= BUFF_SIZE) {
        printk(KERN_ERR "Input too large\n");
        return -EINVAL;
    }

    ret = copy_from_user(buff, buffer, length);
    if (ret) {
        return -EFAULT;
    }
    //buff[length] = '\0'; // Null-terminate the buffer

    ret = sscanf(buff, "%u %u", &addr, &value);
    if (ret != 2) {
        printk(KERN_ERR "Invalid input format\n");
        return -EINVAL;
    }

    if (addr >= INSTR_BRAM_SIZE) {
        printk(KERN_ERR "Address out of range\n");
        return -EINVAL;
    }

    iowrite32((u32)value, lp->base_addr + addr);
    printk(KERN_INFO "Successfully wrote value %#x to address %#x", value, addr);

    return length;
}


static int __init instr_init(void)
{
   int ret = 0;

	//Initialize array

   ret = alloc_chrdev_region(&my_dev_id, 0, 1, DRIVER_NAME);
   if (ret){
      printk(KERN_ERR "failed to register char device\n");
      return ret;
   }
   printk(KERN_INFO "char device region allocated\n");

   my_class = class_create(THIS_MODULE, "instr_class");
   if (my_class == NULL){
      printk(KERN_ERR "failed to create class\n");
      goto fail_0;
   }
   printk(KERN_INFO "class created\n");
   
   my_device = device_create(my_class, NULL, my_dev_id, NULL, DRIVER_NAME);
   if (my_device == NULL){
      printk(KERN_ERR "failed to create device\n");
      goto fail_1;
   }
   printk(KERN_INFO "device created\n");

	my_cdev = cdev_alloc();	
	my_cdev->ops = &my_fops;
	my_cdev->owner = THIS_MODULE;
	ret = cdev_add(my_cdev, my_dev_id, 1);
	if (ret)
	{
      printk(KERN_ERR "failed to add cdev\n");
		goto fail_2;
	}
   printk(KERN_INFO "cdev added\n");
   printk(KERN_INFO "Hello world\n");

  return platform_driver_register(&instr_driver);

   fail_2:
      device_destroy(my_class, my_dev_id);
   fail_1:
      class_destroy(my_class);
   fail_0:
      unregister_chrdev_region(my_dev_id, 1);
   return -1;
}

static void __exit instr_exit(void)
{
   platform_driver_unregister(&instr_driver);
   cdev_del(my_cdev);
   device_destroy(my_class, my_dev_id);
   class_destroy(my_class);
   unregister_chrdev_region(my_dev_id,1);
   printk(KERN_INFO "Goodbye, cruel world\n");
}


module_init(instr_init);
module_exit(instr_exit);
