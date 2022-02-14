#include <linux/init.h>
#include <linux/module.h>
#include <linux/jiffies.h>
MODULE_LICENSE("Dual BSD/GPL");

unsigned long start;

static int hello_init(void)
{
    start = jiffies;
    printk(KERN_ALERT "Hello, world\n");
    return 0;
}

static void hello_exit(void)
{
    unsigned long dur = (jiffies - start) * 1000 / HZ;
    printk(KERN_ALERT "Goodbye, cruel world\n");
    printk(KERN_ALERT "Time difference using jiffies: %ldms", dur);
}

module_init(hello_init);
module_exit(hello_exit);
