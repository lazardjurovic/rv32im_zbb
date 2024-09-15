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

// REGISTER CONSTANTS
#define XIL_AXI_TIMER_TCSR_OFFSET 0x0
#define XIL_AXI_TIMER_TLR_OFFSET 0x4
#define XIL_AXI_TIMER_TLR1_OFFSET 0x14
#define XIL_AXI_TIMER_TCR_OFFSET 0x8
#define XIL_AXI_TIMER_TCSR1_OFFSET 0x18

#define XIL_AXI_TIMER_CSR_CASC_MASK 0x00000800
#define XIL_AXI_TIMER_CSR_ENABLE_ALL_MASK 0x00000400
#define XIL_AXI_TIMER_CSR_ENABLE_PWM_MASK 0x00000200
#define XIL_AXI_TIMER_CSR_INT_OCCURED_MASK 0x00000100
#define XIL_AXI_TIMER_CSR_ENABLE_TMR_MASK 0x00000080
#define XIL_AXI_TIMER_CSR_ENABLE_INT_MASK 0x00000040
#define XIL_AXI_TIMER_CSR_LOAD_MASK 0x00000020
#define XIL_AXI_TIMER_CSR_AUTO_RELOAD_MASK 0x00000010
#define XIL_AXI_TIMER_CSR_EXT_CAPTURE_MASK 0x00000008
#define XIL_AXI_TIMER_CSR_EXT_GENERATE_MASK 0x00000004
#define XIL_AXI_TIMER_CSR_DOWN_COUNT_MASK 0x00000002
#define XIL_AXI_TIMER_CSR_CAPTURE_MODE_MASK 0x00000001

#define BUFF_SIZE 20
#define DRIVER_NAME "timer"
#define DEVICE_NAME "xilaxitimer"

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Xilinx");
MODULE_DESCRIPTION("Driver for RISC-V 32 bit CPU.");
MODULE_ALIAS("custom:xilaxitimer"); // TODO: PROMENI

struct timer_info
{
	unsigned long mem_start;
	unsigned long mem_end;
	void __iomem *base_addr;
	int irq_num;
};

dev_t my_dev_id;
static struct class *my_class;
static struct device *my_device;
static struct cdev *my_cdev;
static struct timer_info *tp = NULL;
struct fasync_struct *async_queue;

// Variables to store timer and alarm values
u64 timer_value = 0;
u64 timer_stopped = 0;
u64 alarm_value = 0;

static int timer_fasync(int fd, struct file *file, int mode);
static irqreturn_t xilaxitimer_isr(int irq, void *dev_id);
static void setup_and_start_timer(u64 milliseconds);
static int timer_probe(struct platform_device *pdev);
static int timer_remove(struct platform_device *pdev);
int timer_open(struct inode *pinode, struct file *pfile);
int timer_close(struct inode *pinode, struct file *pfile);
ssize_t timer_read(struct file *pfile, char __user *buffer, size_t length, loff_t *offset);
ssize_t timer_write(struct file *pfile, const char __user *buffer, size_t length, loff_t *offset);
static int __init timer_init(void);
static void __exit timer_exit(void);

struct file_operations my_fops =
	{
		.owner = THIS_MODULE,
		.open = timer_open,
		.read = timer_read,
		.write = timer_write,
		.release = timer_close,
		.fasync = timer_fasync,
};

static struct of_device_id timer_of_match[] = {
	{.compatible = "axi_timer",},
	{/* end of list */},
};

static struct platform_driver timer_driver = {
	.driver = {
		.name = DRIVER_NAME,
		.owner = THIS_MODULE,
		.of_match_table = timer_of_match,
	},
	.probe = timer_probe,
	.remove = timer_remove,
};

MODULE_DEVICE_TABLE(of, timer_of_match);

//***************************************************
// INTERRUPT SERVICE ROUTINE (HANDLER)

static irqreturn_t xilaxitimer_isr(int irq, void *dev_id)
{
	unsigned int data = 0;

	// Check Timer Counter Value
	data = ioread32(tp->base_addr + XIL_AXI_TIMER_TCR_OFFSET);
	printk(KERN_INFO "xilaxitimer_isr: Interrupt occurred !");

	// Clear Interrupt
	data = ioread32(tp->base_addr + XIL_AXI_TIMER_TCSR_OFFSET);
	iowrite32(data | XIL_AXI_TIMER_CSR_INT_OCCURED_MASK,
			  tp->base_addr + XIL_AXI_TIMER_TCSR_OFFSET);

	kill_fasync(&async_queue, SIGIO, POLL_IN);

	// Disable Timer
	printk(KERN_NOTICE " Disabling timer\n");
	data = ioread32(tp->base_addr + XIL_AXI_TIMER_TCSR_OFFSET);
	iowrite32(data & ~(XIL_AXI_TIMER_CSR_ENABLE_TMR_MASK), tp->base_addr + XIL_AXI_TIMER_TCSR_OFFSET);

	return IRQ_HANDLED;
}

//***************************************************
// HELPER FUNCTION THAT RESETS AND STARTS TIMER WITH PERIOD IN MILISECONDS

static void setup_and_start_timer(u64 milliseconds)
{
	// Disable Timer Counter
	u32 load0, load1;
	u32 tcsr0, tcsr1;
	u64 zero = 0, tval;
	load1 = ((zero - milliseconds * 100000) >> 32);
	load0 = ((zero - milliseconds * 100000) & 0xFFFFFFFF);

	// Disable timer/counter while configuration is in progress
	tcsr0 = ioread32(tp->base_addr + XIL_AXI_TIMER_TCSR_OFFSET);
	tcsr1 = ioread32(tp->base_addr + XIL_AXI_TIMER_TCSR1_OFFSET);
	iowrite32(tcsr0 & ~(XIL_AXI_TIMER_CSR_ENABLE_TMR_MASK),
			  tp->base_addr + XIL_AXI_TIMER_TCSR_OFFSET);
	iowrite32(tcsr1 & ~(XIL_AXI_TIMER_CSR_ENABLE_TMR_MASK),
			  tp->base_addr + XIL_AXI_TIMER_TCSR1_OFFSET);

	// Set load values
	iowrite32(load0, tp->base_addr + XIL_AXI_TIMER_TLR_OFFSET);
	iowrite32(load1, tp->base_addr + XIL_AXI_TIMER_TLR1_OFFSET);

	// Load the values into the counters
	tcsr0 = ioread32(tp->base_addr + XIL_AXI_TIMER_TCSR_OFFSET);
	iowrite32(tcsr0 | XIL_AXI_TIMER_CSR_LOAD_MASK, tp->base_addr + XIL_AXI_TIMER_TCSR_OFFSET);

	tcsr0 = ioread32(tp->base_addr + XIL_AXI_TIMER_TCSR_OFFSET);
	iowrite32(tcsr0 & ~(XIL_AXI_TIMER_CSR_LOAD_MASK), tp->base_addr + XIL_AXI_TIMER_TCSR_OFFSET);
	
	tcsr1 = ioread32(tp->base_addr + XIL_AXI_TIMER_TCSR1_OFFSET);
	iowrite32(tcsr1 | XIL_AXI_TIMER_CSR_LOAD_MASK, tp->base_addr + XIL_AXI_TIMER_TCSR1_OFFSET);

	tcsr1 = ioread32(tp->base_addr + XIL_AXI_TIMER_TCSR1_OFFSET);
	iowrite32(tcsr1 | XIL_AXI_TIMER_CSR_LOAD_MASK, tp->base_addr + XIL_AXI_TIMER_TCSR1_OFFSET);

	tval = load1;
	tval = tval << 32;
	tval = tval | load0;
	printk(KERN_INFO "load0 = %llu, load1 = %llu",div64_u64(~load0,100000),div64_u64(~load1,100000));
	
	// Setting timer mode to cascade
	iowrite32(tcsr0 | XIL_AXI_TIMER_CSR_CASC_MASK, tp->base_addr + XIL_AXI_TIMER_TCSR_OFFSET);

	// Enable interrupts and autoreload, rest should be zero
	iowrite32(XIL_AXI_TIMER_CSR_ENABLE_INT_MASK | XIL_AXI_TIMER_CSR_AUTO_RELOAD_MASK,
			  tp->base_addr + XIL_AXI_TIMER_TCSR_OFFSET);

	// Start Timer by setting enable signal
	tcsr0 = ioread32(tp->base_addr + XIL_AXI_TIMER_TCSR_OFFSET);
	iowrite32(tcsr0 | XIL_AXI_TIMER_CSR_ENABLE_TMR_MASK,
			  tp->base_addr + XIL_AXI_TIMER_TCSR_OFFSET);
}

//***************************************************
// ASYNC SIGNAL
static int timer_fasync(int fd, struct file *file, int mode)
{
	return fasync_helper(fd, file, mode, &async_queue);
}

//***************************************************
// PROBE AND REMOVE
static int timer_probe(struct platform_device *pdev)
{
	struct resource *r_mem;
	int rc = 0;

	// Get phisical register adress space from device tree
	r_mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!r_mem)
	{
		printk(KERN_ALERT "xilaxitimer_probe: Failed to get reg resource\n");
		return -ENODEV;
	}

	// Get memory for structure timer_info
	tp = (struct timer_info *)kmalloc(sizeof(struct timer_info), GFP_KERNEL);
	if (!tp)
	{
		printk(KERN_ALERT "xilaxitimer_probe: Could not allocate timer device\n");
		return -ENOMEM;
	}

	// Put phisical adresses in timer_info structure
	tp->mem_start = r_mem->start;
	tp->mem_end = r_mem->end;

	// Reserve that memory space for this driver
	if (!request_mem_region(tp->mem_start, tp->mem_end - tp->mem_start + 1, DEVICE_NAME))
	{
		printk(KERN_ALERT "xilaxitimer_probe: Could not lock memory region at %p\n", (void *)tp->mem_start);
		rc = -EBUSY;
		goto error1;
	}

	// Remap phisical to virtual adresses
	tp->base_addr = ioremap(tp->mem_start, tp->mem_end - tp->mem_start + 1);
	if (!tp->base_addr)
	{
		printk(KERN_ALERT "xilaxitimer_probe: Could not allocate memory\n");
		rc = -EIO;
		goto error2;
	}

	// Get interrupt number from device tree
	tp->irq_num = platform_get_irq(pdev, 0);
	if (!tp->irq_num)
	{
		printk(KERN_ALERT "xilaxitimer_probe: Failed to get irq resource\n");
		rc = -ENODEV;
		goto error2;
	}

	// Reserve interrupt number for this driver
	if (request_irq(tp->irq_num, xilaxitimer_isr, 0, DEVICE_NAME, NULL))
	{
		printk(KERN_ERR "xilaxitimer_probe: Cannot register IRQ %d\n", tp->irq_num);
		rc = -EIO;
		goto error3;
	}
	else
	{
		printk(KERN_INFO "xilaxitimer_probe: Registered IRQ %d\n", tp->irq_num);
	}

	printk(KERN_NOTICE "xilaxitimer_probe: Timer platform driver registered\n");
	return 0; // ALL OK

error3:
	iounmap(tp->base_addr);
error2:
	release_mem_region(tp->mem_start, tp->mem_end - tp->mem_start + 1);
	kfree(tp);
error1:
	return rc;
}

static int timer_remove(struct platform_device *pdev)
{
	// Disable timer
	unsigned int data = 0;
	data = ioread32(tp->base_addr + XIL_AXI_TIMER_TCSR_OFFSET);
	iowrite32(data & ~(XIL_AXI_TIMER_CSR_ENABLE_TMR_MASK),
			  tp->base_addr + XIL_AXI_TIMER_TCSR_OFFSET);
	// Free resources taken in probe
	free_irq(tp->irq_num, NULL);
	iowrite32(0, tp->base_addr);
	iounmap(tp->base_addr);
	release_mem_region(tp->mem_start, tp->mem_end - tp->mem_start + 1);
	kfree(tp);
	printk(KERN_WARNING "xilaxitimer_remove: Timer driver removed\n");
	return 0;
}

//***************************************************
// FILE OPERATION functions

static void start_stop(void)
{
	u32 data;
	u32 tcr1;
	u32 tcr0;
	u64 tval;

	data = ioread32(tp->base_addr + XIL_AXI_TIMER_TCSR_OFFSET);

	if (data & XIL_AXI_TIMER_CSR_ENABLE_TMR_MASK)
	{
		// Read value of timer
		tcr0 = ioread32(tp->base_addr + XIL_AXI_TIMER_TCSR_OFFSET);
		tcr1 = ioread32(tp->base_addr + XIL_AXI_TIMER_TCSR1_OFFSET);
		tval = tcr1;
		tval = tval << 32;
		tval = tval | tcr0;
		timer_stopped = div64_u64(~tval,100000);

		// Disable timer
		iowrite32(data & (~XIL_AXI_TIMER_CSR_ENABLE_TMR_MASK), tp->base_addr + XIL_AXI_TIMER_TCSR_OFFSET);
	}
	else
	{
		if (timer_stopped == 0)
			setup_and_start_timer(alarm_value - timer_value);
		else
			setup_and_start_timer(timer_stopped);
	}
}

int timer_open(struct inode *pinode, struct file *pfile) 
{
	// printk(KERN_INFO "Succesfully opened timer\n");
	return 0;
}	

int timer_close(struct inode *pinode, struct file *pfile)
{
	// printk(KERN_INFO "Succesfully closed timer\n");
	return 0;
}

ssize_t timer_read(struct file *pfile, char __user *buffer, size_t length, loff_t *offset)
{
	u32 tcr0, tcr1;
	unsigned int hh = 0, mm = 0, ss = 0, ms = 0;
	u64 tval,rem;
	char buf[BUFF_SIZE];
	int len;

	// Reading timer registers
	tcr0 = ioread32(tp->base_addr + XIL_AXI_TIMER_TCSR_OFFSET);
	tcr1 = ioread32(tp->base_addr + XIL_AXI_TIMER_TCSR1_OFFSET);
	tval = tcr1;
	tval = (u64)(tcr1 << 32) | tcr0;
	tval = ~tval;
	tval = div64_u64(timer_value + tval, 100000);

	hh = div64_u64_rem(tval,3600000,&rem);
	mm = div64_u64_rem(rem,60000,&rem);
	ss = div64_u64_rem(rem,1000,&rem);
	ms = rem;

	len = snprintf(buf, BUFF_SIZE, "%02u:%02u:%02u:%03u",hh,mm,ss,ms);

	if (copy_to_user(buffer, buf, len))
		return -EFAULT;

	//printk(KERN_INFO "Succesfully read timer: %u:%u:%u:%u\n",hh,mm,ss,ms);
	return len;
}

ssize_t timer_write(struct file *pfile, const char __user *buffer, size_t length, loff_t *offset)
{
	char buff[BUFF_SIZE];
	unsigned int millis = 0;
	unsigned int hour = 0;
	unsigned int sec = 0;
	unsigned int min = 0;
	int ret = 0;
	char command;
	ret = copy_from_user(buff, buffer, length);
	if (ret)
		return -EFAULT;
	buff[length] = '\0';

	ret = sscanf(buff, "%c %u:%u:%u:%u", &command, &hour, &min, &sec, &millis);

	switch (command)
	{
	case 't':
		// Loading timer value
		timer_value = millis + sec * 1000 + min * 60000 + hour * 3600000;
		printk(KERN_NOTICE "Timer value set at: %u:%u:%u:%u or %llu ms", hour,min,sec,millis,timer_value);
		break;
	case 'a':
		// Loading alarm value
		alarm_value = millis + sec * 1000 + min * 60000 + hour * 3600000;
		printk(KERN_NOTICE "Alarm value set at: %u:%u:%u:%u or %llu ms", hour,min,sec,millis,alarm_value);
		break;
	case 's':
		// Starting/Stoping timer
		start_stop();
		break;
	case 'r':
		// Restarting timer
		setup_and_start_timer(alarm_value - timer_value);
		break;
	default:
		printk(KERN_WARNING "Unknown command!");
		break;
	}

	/*
	if(ret == 2)//two parameters parsed in sscanf
	{

		if (millis > 40000)
		{
			printk(KERN_WARNING "xilaxitimer_write: Maximum period exceeded, enter something less than 40000 \n");
		}
		else
		{
			printk(KERN_INFO "xilaxitimer_write: Starting timer for %d interrupts. One every %d miliseconds \n",number,millis);
			i_num = number;
			setup_and_start_timer(millis);
		}

	}
	else
	{
		printk(KERN_WARNING "xilaxitimer_write: Wrong format, expected n,t \n\t n-number of interrupts\n\t t-time in ms between interrupts\n");
	}
	*/

	return length;
}

//***************************************************
// MODULE_INIT & MODULE_EXIT functions

static int __init timer_init(void)
{
	int ret = 0;

	ret = alloc_chrdev_region(&my_dev_id, 0, 1, DRIVER_NAME);
	if (ret)
	{
		printk(KERN_ERR "xilaxitimer_init: Failed to register char device\n");
		return ret;
	}
	printk(KERN_INFO "xilaxitimer_init: Char device region allocated\n");

	my_class = class_create(THIS_MODULE, "timer_class");
	if (my_class == NULL)
	{
		printk(KERN_ERR "xilaxitimer_init: Failed to create class\n");
		goto fail_0;
	}
	printk(KERN_INFO "xilaxitimer_init: Class created\n");

	my_device = device_create(my_class, NULL, my_dev_id, NULL, DRIVER_NAME);
	if (my_device == NULL)
	{
		printk(KERN_ERR "xilaxitimer_init: Failed to create device\n");
		goto fail_1;
	}
	printk(KERN_INFO "xilaxitimer_init: Device created\n");

	my_cdev = cdev_alloc();
	my_cdev->ops = &my_fops;
	my_cdev->owner = THIS_MODULE;
	ret = cdev_add(my_cdev, my_dev_id, 1);
	if (ret)
	{
		printk(KERN_ERR "xilaxitimer_init: Failed to add cdev\n");
		goto fail_2;
	}
	printk(KERN_INFO "xilaxitimer_init: Cdev added\n");
	printk(KERN_NOTICE "xilaxitimer_init: Hello world\n");

	return platform_driver_register(&timer_driver);

fail_2:
	device_destroy(my_class, my_dev_id);
fail_1:
	class_destroy(my_class);
fail_0:
	unregister_chrdev_region(my_dev_id, 1);
	return -1;
}

static void __exit timer_exit(void)
{
	platform_driver_unregister(&timer_driver);
	cdev_del(my_cdev);
	device_destroy(my_class, my_dev_id);
	class_destroy(my_class);
	unregister_chrdev_region(my_dev_id, 1);
	printk(KERN_INFO "xilaxitimer_exit: Goodbye, cruel world\n");
}

module_init(timer_init);
module_exit(timer_exit);