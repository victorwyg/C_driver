#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/ide.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/gpio.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/of_gpio.h>
#include <linux/semaphore.h>
#include <linux/timer.h>
#include <linux/irq.h>
#include <linux/wait.h>
#include <linux/poll.h>
#include <linux/fs.h>
#include <linux/fcntl.h>
#include <linux/platform_device.h>
#include <asm/mach/map.h>
#include <asm/uaccess.h>
#include <asm/io.h>

#define BEEPDEV_CNT 1
#define BEEPDEV_NAME "beep"
#define BEEPOFF 0
#define BEEPON 1

struct beep_dev
{
    dev_t devid;                /*设备ID*/
    struct cdev cdev;            /*字符设备cdev*/
    struct class *class;         /*类*/
    struct device *device;       /*设备*/
    int major;                   /*主设备号*/
    struct device_node *nd;      /*设备节点*/
    int beep0;                   /*设备别名*/
};

struct beep_dev Beep;

void beep_switch(u8 sta)        /*开关控制传参*/
{
    if(sta == BEEPOFF)
    {
        gpio_set_value(Beep.beep0,0);
    }
    else if(sta == BEEPON)
    {
        gpio_set_value(Beep.beep0,1);
    }
}

static int beep_open(struct inode *inode,struct file *filp)     /*open操作*/
{
    filp->private_data = &Beep;                                 /*设置私有数据*/
    return 0;
}

static ssize_t beep_write(struct file *filp,const char __user *buf,size_t cnt,loff_t *offt) /*写函数*/
{
    int retvalue;
    unsigned char databuf[2];
    unsigned char beepstat;

    retvalue = copy_from_user(databuf,buf,cnt);
    if(retvalue < 0){
        printk("kernel write failed!\r\n");
        return -EFAULT;
    }
    beepstat = databuf[0];
    if(beepstat == BEEPOFF){
       beep_switch(BEEPOFF);        
    }else if(beepstat == BEEPON){
       beep_switch(BEEPON);
    }
    return 0;
}
static struct file_operations beep_fops = {                 /*操作函数*/
    .owner = THIS_MODULE,
    .open = beep_open,
    .write = beep_write,

};


static int beep_probe(struct platform_device *dev){
    printk("beep probe\r\n");
    if(Beep.major){
        Beep.devid = MKDEV(Beep.major,0);                               /*固定设置主次设备号*/
        register_chrdev_region(Beep.devid,BEEPDEV_CNT,BEEPDEV_NAME);
    }else{                                                              /*申请主次设备号*/                    
        alloc_chrdev_region(&Beep.devid,0,BEEPDEV_CNT,BEEPDEV_NAME);
        Beep.major = MAJOR(Beep.devid);
    }
        printk("beep alloc succes\r\n");
        cdev_init(&Beep.cdev,&beep_fops);
        cdev_add(&Beep.cdev,Beep.devid,BEEPDEV_CNT);
        Beep.class = class_create(THIS_MODULE,BEEPDEV_NAME);
        if(IS_ERR(Beep.class)){
            return PTR_ERR(Beep.class);
        }
        Beep.device = device_create(Beep.class,NULL,Beep.devid,NULL,BEEPDEV_NAME);
        if(IS_ERR(Beep.device)){
            return PTR_ERR(Beep.device);
        }
        
        /*io初始化*/
        Beep.nd = of_find_node_by_path("/beep");
        if(Beep.nd == NULL){
            printk("node is not find\r\n");
            return -EINVAL;
        }
        Beep.beep0 = of_get_named_gpio(Beep.nd,"beep_gpios",0);
        if (Beep.beep0 < 0){
            printk("can't get beep-gpio\r\n");
            return -EINVAL;
        }
        gpio_request(Beep.beep0, "beep");
        gpio_direction_output(Beep.beep0, 0); /*设置为输出，默认高电平 */
        return 0;
}

static int beep_remove(struct platform_device *dev){
    printk("beep remove\r\n");
    gpio_set_value(Beep.beep0, 0); /* 卸载驱动的时候关闭 LED */
    cdev_del(&Beep.cdev);           /*删除cdev*/
    unregister_chrdev_region(Beep.devid,BEEPDEV_CNT);
    device_destroy(Beep.class,Beep.devid);
    class_destroy(Beep.class);
   return 0;
}
static struct of_device_id beep_of_match[] = {
    {.compatible = "wyg"},
    {/*sentinel*/},
};
static struct platform_driver beep_driver = {
    .driver = {
        .name = "imx6ull-beep",
        .of_match_table = beep_of_match,         /*匹配表*/
    },
    .probe = beep_probe,
    .remove = beep_remove,
};


static int __init beep_init(void)
{
    return platform_driver_register(&beep_driver);
} 
static void __exit beep_exit(void)
{
    
    platform_driver_unregister(&beep_driver);
    printk("exit");
}



module_init(beep_init);
module_exit(beep_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("wyg");