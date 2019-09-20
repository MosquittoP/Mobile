#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/slab.h>
#include <linux/gpio.h>
#include <mach/platform.h>
#include <linux/io.h>
#include <linux/delay.h>

#define LED_MAJOR       242
#define LED_NAME        "DOT_DRIVER"

#define GPIO_SIZE       256

char led_usage = 0;
static void *led_map;
volatile unsigned *led;

static int led_open(struct inode *minode, struct file *mfile)
{
        if (led_usage != 0)
                return -EBUSY;
        led_usage = 1;

        led_map = ioremap(GPIO_BASE, GPIO_SIZE);
        if (!led_map)
        {
                printk("error: mapping gpio memory");
                iounmap(led_map);
                return -EBUSY;
        }

        led = (volatile unsigned int *)led_map;
        int index;
        for (index = 2 ; index <= 9 ; index++)
        {
                *(led + 0) &= ~(0x7 << (3 * index));
                *(led + 0) |= (0x1 << (3 * index));
        }
        for (index = 0 ; index < 8 ; index++)
        {
                *(led + 1) &= ~(0x7 << (3 * index));
                *(led + 1) |= (0x1 << (3 * index));
        }

        return 0;
}

static int led_write(struct file *mfile, const char *gdata, size_t length, loff_t *off_what)
{
        char    tmp_buf;
        int     result;
        int i;

        result = copy_from_user(&tmp_buf, gdata, length);
        if (result < 0)
        {
                printk("Error: copy from user");
                return result;
        }

        printk("data from app : %d\n", tmp_buf);

        // Control LED
        for (i = 0 ; i < 100; i++)
        {
                *(led + 7) |= (0x00ff << 2);
                mdelay(2);
                *(led + 10) |= (0x00ff << 2);

        }

        return length;
}

static struct file_operations led_fops =
{
        .owner          = THIS_MODULE,
        .open           = led_open,
        .release        = led_release,
        .write          = led_write,
};

static int led_init(void)
{
        int result;

        result = register_chrdev(LED_MAJOR, LED_NAME, &led_fops);
        if (result < 0)
        {
                printk(KERN_WARNING "Can't get any major!\n");
                return result;
        }
        return 0;
}

static void led_exit(void)
{
        unregister_chrdev(LED_MAJOR, LED_NAME);
        printk("LED module removed.\n");
}

module_init(led_init);
module_exit(led_exit);

MODULE_LICENSE("GPL");
