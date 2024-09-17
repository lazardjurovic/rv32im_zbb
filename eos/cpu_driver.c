include <linux/kernel.h>
#include <linux/string.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/cdev.h>
#include <linux/kdev_t.h>
#include <linux/uaccess.h>
#include <linux/errno.h>
#include <linux/device.h>
#include <linux/of.h>
#include <linux/io.h> //iowrite ioread
#include <linux/slab.h>//kmalloc kfree
#include <linux/platform_device.h>//platform driver
#include <linux/ioport.h>//ioremap
#define BUFF_SIZE 2
#define WRITE_BUFF_SIZE 16
#define DRIVER_NAME "cpu"

MODULE_LICENSE("Dual BSD/GPL");

struct cpu_info {
  unsigned long mem_start;
  unsigned long mem_end;
  void __iomem *base_addr;
};

dev_t my_dev_id;
static struct class *my_class;
static struct device *my_device;
static struct cdev *my_cdev;
static struct cpu_info *lp = NULL;

int endRead = 0;


static int cpu_probe(struct platform_device *pdev);
static int cpu_remove(struct platform_device *pdev);
int cpu_open(struct inode *pinode, struct file *pfile);
int cpu_close(struct inode *pinode, struct file *pfile);
ssize_t cpu_read(struct file *pfile, char __user *buffer, size_t length, loff_t *offset);
ssize_t cpu_write(struct file *pfile, const char __user *buffer, size_t length, loff_t *offset);
static int __init cpu_init(void);
static void __exit cpu_exit(void);

struct file_operations my_fops =
{
	.owner = THIS_MODULE,
	.open = cpu_open,
	.read = cpu_read,
	.write = cpu_write,
	.release = cpu_close,
};

static struct of_device_id cpu_of_match[] = {
  { .compatible = "xlnx,risc-v-cpu-1.0", },
  { /* end of list */ },
};

static struct platform_driver cpu_driver = {
  .driver = {
    .name = DRIVER_NAME,
    .owner = THIS_MODULE,
    .of_match_table	= cpu_of_match,
  },
  .probe		= cpu_probe,
  .remove		= cpu_remove,
};


MODULE_DEVICE_TABLE(of, cpu_of_match);

static int cpu_probe(struct platform_device *pdev)
{
  struct resource *r_mem;
  int rc = 0;
  r_mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);
  if (!r_mem) {
    printk(KERN_ALERT "Failed to get resource\n");
    return -ENODEV;
  }
  lp = (struct cpu_info *) kmalloc(sizeof(struct cpu_info), GFP_KERNEL);
  if (!lp) {
    printk(KERN_ALERT "Could not allocate cpu device\n");
    return -ENOMEM;
  }

  lp->mem_start = r_mem->start;
  lp->mem_end = r_mem->end;
  //printk(KERN_INFO "Start address:%x \t end address:%x", r_mem->start, r_mem->end);

  if (!request_mem_region(lp->mem_start,lp->mem_end - lp->mem_start + 1,	DRIVER_NAME))
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

  printk(KERN_WARNING "cpu platform driver registered\n");
  return 0;//ALL OK

error2:
  release_mem_region(lp->mem_start, lp->mem_end - lp->mem_start + 1);
error1:
  return rc;
}

static int cpu_remove(struct platform_device *pdev)
{
  printk(KERN_WARNING "cpu platform driver removed\n");
  iowrite32(0, lp->base_addr);
  iounmap(lp->base_addr);
  release_mem_region(lp->mem_start, lp->mem_end - lp->mem_start + 1);
  kfree(lp);
  return 0;
}



int cpu_open(struct inode *pinode, struct file *pfile) 
{
		//printk(KERN_INFO "Succesfully opened cpu\n");
		return 0;
}

int cpu_close(struct inode *pinode, struct file *pfile) 
{
		//printk(KERN_INFO "Succesfully closed cpu\n");
		return 0;
}

ssize_t cpu_read(struct file *pfile, char __user *buffer, size_t length, loff_t *offset) 
{
	int ret;
	u32 stop_flag = 0;
	char buff[BUFF_SIZE];
	
	if(endRead){
		endRead = 0;
		return 0;
	}	

	stop_flag = ioread32(lp->base_addr+0xC); // read STOP flag

    if(stop_flag == 0xFFFFFFFF){ // flag set
        buff[0] = 's';
    }else if(stop_flag == 0x00000000){ // flag not set
        buff[0] = 'r';
    }else{
        printk(KERN_INFO "Something went wrong \n");
        return -EFAULT;
    }

	ret = copy_to_user(buffer, buff, 1);
	if(ret)
		return -EFAULT;
	printk(KERN_INFO "Succesfully read STOP flag\n");
	endRead = 1;
	return 4;
}

ssize_t cpu_write(struct file *pfile, const char __user *buffer, size_t length, loff_t *offset) 
{
	char buff[2] = {'\0','\0'};
	int ret;
	u32 reset_reg;
	ret = 0;
    	
	ret = copy_from_user(buff, buffer, length);
	if(ret)
		return -EFAULT;
	
	   printk(KERN_INFO "Received command: %s\n", buff);

    if(buff[0] ==  'r'){
        reset_reg = 0x00000000;
    }else if(buff[0] == 's'){ 
        reset_reg = 0xFFFFFFFF;
    }else{
        printk(KERN_INFO "Wrong value\n");
        return -EFAULT;
    }

	if (!ret)
	{
		iowrite32((u32)reset_reg, lp->base_addr);
		//printk(KERN_INFO "Succesfully wrote value %#x",(u32)cpu_val); 
	}
	else
	{
		printk(KERN_INFO "Wrong command format\n"); 
	}

	return length;
}

static int __init cpu_init(void)
{
   int ret = 0;

	//Initialize array

   ret = alloc_chrdev_region(&my_dev_id, 0, 1, DRIVER_NAME);
   if (ret){
      printk(KERN_ERR "failed to register char device\n");
      return ret;
   }
   printk(KERN_INFO "char device region allocated\n");

   my_class = class_create(THIS_MODULE, "cpu_class");
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
   printk(KERN_INFO "Hello RISC-V\n");

  return platform_driver_register(&cpu_driver);

   fail_2:
      device_destroy(my_class, my_dev_id);
   fail_1:
      class_destroy(my_class);
   fail_0:
      unregister_chrdev_region(my_dev_id, 1);
   return -1;
}

static void __exit cpu_exit(void)
{
   platform_driver_unregister(&cpu_driver);
   cdev_del(my_cdev);
   device_destroy(my_class, my_dev_id);
   class_destroy(my_class);
   unregister_chrdev_region(my_dev_id,1);
   printk(KERN_INFO "Goodbye, RISC_V\n");
}


module_init(cpu_init);
module_exit(cpu_exit);
