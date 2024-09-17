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
#include <linux/of_device.h>
#include <linux/io.h>
#include <linux/slab.h>
#include <linux/platform_device.h>
#include <linux/ioport.h>

#include <linux/interrupt.h>
#define MAX_DEVICES 2
#define BUFF_SIZE 512
#define INSTR_BRAM_SIZE 1024
#define DRIVER_NAME "bram"
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
static struct instr_info *lp[MAX_DEVICES] = {NULL, NULL};

int endRead[MAX_DEVICES] = {0, 0};  // End read flags for each device

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
    { .compatible = "xlnx,axi-bram-ctrl-4.1", .data = (void *)0x40000000 },
    { .compatible = "xlnx,axi-bram-ctrl-4.1", .data = (void *)0x42000000 },
    {},
};

static struct platform_driver instr_driver = {
  .driver = {
    .name = DRIVER_NAME,
    .owner = THIS_MODULE,
    .of_match_table = instr_of_match,
  },
  .probe = instr_probe,
  .remove = instr_remove,
};

MODULE_DEVICE_TABLE(of, instr_of_match);

static int instr_probe(struct platform_device *pdev)
{
    printk(KERN_INFO "[PROBE] Started probe function\n");

    struct resource *r_mem;
    int rc;
    int i;
    unsigned long mem_start;
    unsigned long mem_end;
    void __iomem *base_addr;
    unsigned long device_data;

    rc = 0;

    // Get the resource and allocate memory
    r_mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    if (!r_mem) {
        printk(KERN_ALERT "Failed to get resource\n");
        return -ENODEV;
    }

    mem_start = r_mem->start;
    mem_end = r_mem->end;

    if (!request_mem_region(mem_start, mem_end - mem_start + 1, DRIVER_NAME)) {
        printk(KERN_ALERT "Could not lock memory region at %p\n", (void *)mem_start);
        return -EBUSY;
    }

    base_addr = ioremap(mem_start, mem_end - mem_start + 1);
    if (!base_addr) {
        printk(KERN_ALERT "Could not allocate memory\n");
        release_mem_region(mem_start, mem_end - mem_start + 1);
        return -EIO;
    }

    // Get the device-specific data
    const struct of_device_id *match = of_match_device(instr_of_match, &pdev->dev);
    if (!match)
        return -EINVAL;

    device_data = (unsigned long)match->data;

    // Find an available slot for the device
    for (i = 0; i < MAX_DEVICES; i++) {
        if (lp[i] == NULL) {
            lp[i] = kzalloc(sizeof(struct instr_info), GFP_KERNEL);
            if (!lp[i]) {
                printk(KERN_ALERT "Could not allocate memory for device info\n");
                iounmap(base_addr);
                release_mem_region(mem_start, mem_end - mem_start + 1);
                return -ENOMEM;
            }

            lp[i]->mem_start = mem_start;
            lp[i]->mem_end = mem_end;
            lp[i]->base_addr = base_addr;

            printk(KERN_INFO "[PROBE] Device %d registered @ address: %lx\n", i, mem_start);
	   /*
            if (device_data == 0x40000000) {
                printk(KERN_INFO "INSTRUCTION MEMORY\n");
            } else if (device_data == 0x42000000) {
                printk(KERN_INFO "DATA MEMORY\n");
            }
	    */
            return 0; // Successful probe
        }
    }

    printk(KERN_ALERT "No available slot for new device\n");
    iounmap(base_addr);
    release_mem_region(mem_start, mem_end - mem_start + 1);
    return -ENOMEM;
}

static int instr_remove(struct platform_device *pdev)
{
    int i;

    for (i = 0; i < MAX_DEVICES; i++) {
        if (lp[i] && lp[i]->mem_start == (unsigned long)pdev->dev.platform_data) {
            printk(KERN_WARNING "BRAM platform driver removed for device %d\n", i);
            iounmap(lp[i]->base_addr);
            release_mem_region(lp[i]->mem_start, lp[i]->mem_end - lp[i]->mem_start + 1);
            kfree(lp[i]);
            lp[i] = NULL;
            return 0;
        }
    }
    return -ENODEV;
}

int instr_open(struct inode *pinode, struct file *pfile)
{
	printk(KERN_INFO "Successfully opened BRAM\n");
	return 0;
}

int instr_close(struct inode *pinode, struct file *pfile)
{
	printk(KERN_INFO "Successfully closed BRAM\n");
	return 0;
}

ssize_t instr_read(struct file *pfile, char __user *buffer, size_t length, loff_t *offset)
{
    int ret, device_index = iminor(pfile->f_inode); // Use minor number to identify device
    unsigned int addr = *offset; // Start reading from the current offset
    u32 value = 0;
    char buff[BUFF_SIZE];
    size_t len = 0;
   
    printk(KERN_INFO "Trying to get %d bytes to user space\n",(int)length);

    // Check for valid device index
    if (device_index < 0 || device_index >= 2) {
        printk(KERN_ERR "Invalid device index %d\n", device_index);
        return -ENODEV;
    }

    // Reset endRead flag if it was set
    if (endRead[device_index] || addr >= INSTR_BRAM_SIZE) {
        return 0; // End of file
    }

    // Initialize buffer
    memset(buff, 0, BUFF_SIZE);

    // Read data from memory and format it
    while (addr <= 128 && len < length) {
        value = ioread32(lp[device_index]->base_addr + addr); // Read data from memory
        // Format value as hexadecimal and append to buffer
        len += snprintf(buff + len, BUFF_SIZE - len, "%08x\n", value);

        // Stop if buffer length exceeds user-requested length
        if (len >= length) {
            break;
        }

        addr += 4; // Move to next address (4 bytes)
    }

    // Update the offset for the next read
    *offset = addr;

    // Check if we copied any data to user space
    if (len == 0) {
        return 0; // No more data to read
    }

    // Copy data to user space
    ret = copy_to_user(buffer, buff, len);
    if (ret) {
        printk(KERN_ERR "Failed to copy %zu bytes to user space from device %d\n", len, device_index);
        return -EFAULT;
    }

    printk(KERN_INFO "Successfully read %zu bytes from device %d\n", len, device_index);

    return len; // Return the number of bytes read
}



ssize_t instr_write(struct file *pfile, const char __user *buffer, size_t length, loff_t *offset)
{
	char buff[BUFF_SIZE];
	int ret, device_index;
	unsigned int value;
	unsigned int addr;
	
	//printk(KERN_INFO "Step 1 of write \n");

	if (length >= BUFF_SIZE) {
		printk(KERN_ERR "Input too large\n");
		return -EINVAL;
	}

	//printk(KERN_INFO "Step 2 of write \n");

	ret = copy_from_user(buff, buffer, length);
	if (ret) {
		return -EFAULT;
	}
	
	//printk(KERN_INFO "HALF WAY THROUH WRITE\n");
	
	ret = sscanf(buff, "%d %u %u", &device_index,&addr, &value);
	if (ret != 3) {
		printk(KERN_ERR "Invalid input format\n");
		return -EINVAL;
	}

	if (addr >= INSTR_BRAM_SIZE) {
		printk(KERN_ERR "Address out of range\n");
		return -EINVAL;
	}
	printk("Trying write %d to %d\n",value,addr);
	iowrite32((u32)value, lp[device_index]->base_addr + addr);
	printk(KERN_INFO "Successfully wrote value %#x to address %#x on device %d\n", value, addr, device_index);
	


	return length;
}

static int __init instr_init(void)
{
    int ret, i;

    // Initialize char device region for MAX_DEVICES
    ret = alloc_chrdev_region(&my_dev_id, 0, MAX_DEVICES, DRIVER_NAME);
    if (ret) {
        printk(KERN_ERR "Failed to register char device\n");
        return ret;
    }
    printk(KERN_INFO "Char device region allocated: %d:%d\n", MAJOR(my_dev_id), MINOR(my_dev_id));

    // Create class
    my_class = class_create(THIS_MODULE, "instr_class");
    if (IS_ERR(my_class)) {
        printk(KERN_ERR "Failed to create class\n");
        unregister_chrdev_region(my_dev_id, MAX_DEVICES);
        return PTR_ERR(my_class);
    }

    // Create device nodes for each device
    for (i = 0; i < MAX_DEVICES; i++) {
        my_device = device_create(my_class, NULL, MKDEV(MAJOR(my_dev_id), i), NULL, DRIVER_NAME "_%d", i);
        if (IS_ERR(my_device)) {
            printk(KERN_ERR "Failed to create device %d\n", i);
            ret = PTR_ERR(my_device);
            goto fail_device_create;
        }
    }

    // Initialize cdev and add it to the system
    my_cdev = cdev_alloc();
    if (!my_cdev) {
        printk(KERN_ERR "Failed to allocate cdev\n");
        ret = -ENOMEM;
        goto fail_cdev_alloc;
    }
    my_cdev->ops = &my_fops;
    my_cdev->owner = THIS_MODULE;
    ret = cdev_add(my_cdev, my_dev_id, MAX_DEVICES);
    if (ret) {
        printk(KERN_ERR "Failed to add cdev\n");
        goto fail_cdev_add;
    }

    // Register platform driver
    ret = platform_driver_register(&instr_driver);
    if (ret) {
        printk(KERN_ERR "Failed to register platform driver\n");
        goto fail_driver_register;
    }

    printk(KERN_INFO "[INIT] Driver loaded\n");
    return 0;

fail_driver_register:
    cdev_del(my_cdev);
fail_cdev_add:
    for (i = 0; i < MAX_DEVICES; i++)
        device_destroy(my_class, MKDEV(MAJOR(my_dev_id), i));
fail_cdev_alloc:
fail_device_create:
    class_destroy(my_class);
    unregister_chrdev_region(my_dev_id, MAX_DEVICES);
    return ret;
}


 
static void __exit instr_exit(void)
{
	int i;

	// Unregister platform driver
	platform_driver_unregister(&instr_driver);

	// Remove cdev
	cdev_del(my_cdev);

	// Destroy devices and class
	for (i = 0; i < MAX_DEVICES; i++)
		device_destroy(my_class, MKDEV(MAJOR(my_dev_id), i));

	class_destroy(my_class);

	// Unregister char device region
	unregister_chrdev_region(my_dev_id, MAX_DEVICES);

	printk(KERN_INFO "Driver unloaded\n");
}

module_init(instr_init);
module_exit(instr_exit);

