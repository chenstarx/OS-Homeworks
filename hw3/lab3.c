#include <linux/init.h>
#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/rtc.h>

MODULE_LICENSE("Dual BSD/GPL");

unsigned int read_index;

static int device_open(struct inode *inode, struct file *file)
{
    pr_info("Lab3 device open\n");
    read_index = 0;
    return 0;
}

static int device_close(struct inode *inodep, struct file *filp)
{
    pr_info("Lab3 device close\n");
    return 0;
}

static ssize_t device_write(struct file *file, const char __user *buf,
               size_t len, loff_t *ppos)
{
    pr_info("Lab3 device write\n");
    return len; 
}
 
static ssize_t device_read(struct file *filp, char __user *buf,
                    size_t count, loff_t *f_pos)
{
    char data[100];
    size_t remain_len;

    struct rtc_time tm;
    time64_t time = ktime_get_real() / 1000000000;
    rtc_time64_to_tm(time, &tm);

    sprintf(
        data,
        "Hello world, the current date is %02d/%02d/%04d. The current time is %02d:%02d:%02d.\n",
        tm.tm_mon + 1, tm.tm_mday, tm.tm_year + 1900, tm.tm_hour, tm.tm_min, tm.tm_sec
    );

    remain_len = strlen(data) - read_index;

    count = remain_len <= count ? remain_len : count;

    if (copy_to_user(buf, data, count)) {
        return -EFAULT;
    }

    read_index += count;

	pr_info("Lab3 device read\n");

    return count;

}

static const struct file_operations fops = {
    .owner      = THIS_MODULE,
    .write      = device_write,
    .read       = device_read,
    .open       = device_open,
    .release    = device_close,
    .llseek     = no_llseek,
};

static struct miscdevice my_dev = {
    .minor      = MISC_DYNAMIC_MINOR,
    .name       = "lab3",
    .fops       = &fops,
};

static int init_my_module(void)
{
    int retval;
    retval = misc_register(&my_dev);
    printk("Lab3 module init\n");
    if (retval) return retval;
    printk("Device registered with minor %i\n",my_dev.minor);
    return 0;
}

static void exit_my_module(void)
{
    misc_deregister(&my_dev);
    printk("Lab3 module exit\n");
}

module_init(init_my_module);
module_exit(exit_my_module);
