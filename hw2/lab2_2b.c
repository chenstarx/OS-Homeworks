#include <linux/init.h>
#include <linux/module.h>
#include <linux/jiffies.h>
MODULE_LICENSE("Dual BSD/GPL");

unsigned long start;

static int hello_init(void)
{
    start = ktime_get_boottime();
    printk(KERN_ALERT "Hello, world\n");
    return 0;
}

static void hello_exit(void)
{
    unsigned long end = ktime_get_boottime();
    unsigned long diff = (end - start) / 1000000;
    printk(KERN_ALERT "Goodbye, cruel world\n");
    printk(KERN_ALERT "Time difference using ktime: %ldms", diff);
}

module_init(hello_init);
module_exit(hello_exit);
