#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/slab.h>
#include <linux/gpio.h>
#include <mach/platform.h>
#include <linux/io.h>
#include <linux/poll.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/sched.h>
#include <linux/wait.h>

#define MOTER_MAJOR     243
#define MOTER_NAME      "MOTER_DRIVER"

#define GPIO_SIZE       256

char moter_usage = 0;
static void *moter_map;
volatile unsigned *moter;
static char tmp_buf;
static int event_flag = 0;
DECLARE_WAIT_QUEUE_HEAD(waitqueue);

static irqreturn_t ind_interrupt_handler(int irq, void *pdata)
{
        int tmp_moter;

        tmp_moter = (*(moter + 13) & (1 << 25)) == 0 ? 0 : 1;

        if (tmp_moter == 0)
                ++tmp_buf;

        wake_up_interruptible(&waitqueue);
        ++event_flag;

        return IRQ_HANDLED;
}

static unsigned moter_poll(struct file *mfile, struct poll_table_struct *pt)
{
        int mask = 0;

        poll_wait(mfile, &waitqueue, pt); // interruptible_sleep_on..
        if (event_flag > 0)
                mask |= (POLLIN | POLLRDNORM);

        event_flag = 0;

        return mask;
}

static int moter_open(struct inode *minode, struct file *mfile)
{
        int result;

        if (moter_usage != 0)
                return -EBUSY;
        moter_usage = 1;

        moter_map = ioremap(GPIO_BASE, GPIO_SIZE);
        if (!moter_map)
        {
                printk("error: mapping gpio memory");
                iounmap(moter_map);
                return -EBUSY;
        }

        moter = (volatile unsigned int *)moter_map;

        // output(GPIO 24)
        *(moter + 2) &= ~(0x7 << (3 * 4));
        *(moter + 2) |= (0x1 << (3 * 4));


        // input(GPIO 25)
        *(moter + 2) &= ~(0x7 << (3 * 5));
        //*(moter + 2) |= (0x0 << (3 * 5));

        // Edge //22
        *(moter + 22) |= (0x1 << 25);
        result = request_irq(gpio_to_irq(25), ind_interrupt_handler, IRQF_TRIGGER_FALLING, "gpio_irq_moter", NULL);
        if (result < 0)
        {
                printk("error: request_irq()");
                return result;
        }

        return 0;
}

static int moter_release(struct inode *minode, struct file *mfile)
{
        moter_usage = 0;
        if (moter)
                iounmap(moter);
        free_irq(gpio_to_irq(25), NULL);
        return 0;
}

static int moter_read(struct file *mfile, char *gdata, size_t length, loff_t *off_what)
{
        int result;

        printk("moter_read = %d\n", tmp_buf);
        result = copy_to_user((void *)gdata, &tmp_buf, length);
        if (result < 0)
        {
                printk("error: copy_to_user()");
                return result;
        }

        return length;
}

static int moter_write(struct file *mfile, const char *gdata, size_t length, loff_t *off_what)
{
        char    tmp_buf;
        int     result;

        result = copy_from_user(&tmp_buf, gdata, length);
        if (result < 0)
        {
                printk("Error: copy from user");
                return result;
        }

        printk("data from app : %d\n", tmp_buf);

        // Control LED
        if (tmp_buf == 0)
                *(moter + 7) |= (0x1 << 24);
        else
                *(moter + 10) |= (0x1 << 24);

        return length;
}

static struct file_operations moter_fops =
{
        .owner          = THIS_MODULE,
        .open           = moter_open,
        .release        = moter_release,
        .read           = moter_read,
        .write          = moter_write,
        .poll           = moter_poll,
};

static int _moter_init(void)
{
        int result;

        result = register_chrdev(MOTER_MAJOR, MOTER_NAME, &moter_fops);
        if (result < 0)
        {
                printk(KERN_WARNING "Can't get any major!\n");
                return result;
        }
        return 0;
}

static void moter_exit(void)
{
        unregister_chrdev(MOTER_MAJOR, MOTER_NAME);
        printk("moter module removed.\n");
}

module_init(_moter_init);
module_exit(moter_exit);

MODULE_LICENSE("GPL");
