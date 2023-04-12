#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>

static int __init xxx_init(void)
{
    
    printk(KERN_EMERG "[ KERN_EMERG ] Hello Module Init\n");
    printk( "[ default ] Hello Module Init\n");
    return 0;
}
static void __exit xxx_exit(void)
{
    printk("[ default ] Hello Module Exit\n");
}
module_init(xxx_init);
module_exit(xxx_exit);

MODULE_LICENSE("GPL2");
MODULE_AUTHOR("WYG");