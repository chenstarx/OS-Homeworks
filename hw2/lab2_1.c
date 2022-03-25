#include <linux/init.h>
#include <linux/module.h>
#include <linux/jiffies.h>
MODULE_LICENSE("Dual BSD/GPL");

static int hello_init(void)
{
    unsigned int tick = 1000 / HZ;
    printk(KERN_ALERT "Hello, world\n");
    printk(KERN_ALERT "Tick time: %d", tick);
    return 0;
}

static void hello_exit(void)
{
    printk(KERN_ALERT "Goodbye, cruel world\n");
}

module_init(hello_init);
module_exit(hello_exit);
