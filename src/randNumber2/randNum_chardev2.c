
// #include <linux/version.h>
// #include <linux/kernel.h>
// #include <linux/types.h>
// #include <linux/kdev_t.h>
#include <linux/random.h>

#include <linux/module.h> /*thu vien dinh nghia cac macro nhu module_init, module_exit */
#include <linux/fs.h> /* thu vien de xai dev_t */
#include <linux/device.h> // phuc vu tao device file
#include <linux/slab.h> // cho ham kmalloc va kfree 
#include <linux/cdev.h> 
//#include "character_driver.h" // mo ta thanh ghi cua character device 
#include <linux/uaccess.h> // ham tro doi du lieu giua user va kernel
#define DRIVER_AUTHOR "Minh Nguyen <npminh@apcs.vn>"
#define DRIVER_DESC "A sample loadable kernel module that generate random number"
#define DRIVER_VERSION "1.0"


// static dev_t first; // Global variable for the first device number
// static struct cdev c_dev; // Global variable for the character device structure
// static struct class *cl; // Global variable for the device class
typedef struct vchar_dev{
	unsigned char * control_regs;
	unsigned char * status_regs;
	unsigned char * data_regs;
}vchar_dev_t;
struct _vchar_drv
{
	dev_t dev_num;
	struct class *dev_class;
	struct device *dev;
	struct cdev * vcdev;
	vchar_dev_t * vchar_hw;
	unsigned int open_cnt;
}vchar_drv;


static int rVal;

/******************* device specific - START ***********************/
// Ham khoi tao thiet bi
// Ham giai phong thiet bi
void vchar_hw_exit(vchar_dev_t *hw){
	kfree(hw->control_regs);
}
// Ham doc tu cac thanh ghi du lieu cua thiet bi
// Ham ghi vao cac thanh ghi dieu khien cua thiet bi    
// Ham xu ly tin hieu gui tu thiet bi


/******************* device specific - END ***********************/

/******************* OS specific - START ***********************/
// Cac ham entry points

static int dev_open(struct inode* i, struct file *f)
{
    vchar_drv.open_cnt++;
	printk("Handle opened event (%d)\n",vchar_drv.open_cnt);
	return 0;
    // printk(KERN_INFO "Driver: open()\n");
    // return 0;
}

static int dev_close(struct inode *i, struct file *f)
{
    printk("Handle closed event\n");
	return 0;
    // printk(KERN_INFO "Driver: close()\n");
    // return 0;
}
static ssize_t dev_read(struct file *f, char __user *buf, size_t len, loff_t *off)
{
    // generate a random non-negative integer (4 bytes)
       
    printk("Reading event...\n");
    get_random_bytes(&rVal, 4);
    //rVal = initRandomInteger();
    
    /* You have just a single char in your buffer, so only 0 offset is valid */
    if (*off > 0) return 0; /* End of file */
    if (copy_to_user(buf, &rVal, 4))
        return -EFAULT;    
    *off = *off + 4;
    return 4;
}

static ssize_t dev_write(struct file *f, const char __user *buf, size_t len, loff_t *off)
{
    printk("Writing event...\n");
    // if (copy_from_user(&c, buf + len - 1, 1) != 0)
    // {
    //     return -EFAULT;
    // }
    //return len;
    //int num_bytes = 4;
    // *off += 4;
	// return 4;
    return len;
}
static struct file_operations fops =
{
        .owner = THIS_MODULE,
        .open = dev_open,
        .release = dev_close,
        //.read = dev_read,
        //.write = dev_write,
};

static int __init init_chardev(void) /* Driver Constructor */
{
    int ret = 0;
	// Cap phat device number
	vchar_drv.dev_num = 0;
	ret = alloc_chrdev_region(& vchar_drv.dev_num,0,1,"random_device2");
	if(ret < 0)
	{
		printk("failed to register device number\n");
		//goto failed_register_devnum;
        return -1;
	}
	printk("allocated device number (%d,%d)\n", MAJOR(vchar_drv.dev_num), MINOR(vchar_drv.dev_num));
	
	// Tao device file
	vchar_drv.dev_class = class_create(THIS_MODULE,"class_random_dev2");
	if( vchar_drv.dev_class == NULL)
	{
		printk("failed to create a device class\n");
        unregister_chrdev_region(vchar_drv.dev_num,1);
       // return -1;
	}
	vchar_drv.dev = device_create(vchar_drv.dev_class, NULL,vchar_drv.dev_num, NULL,"random_dev2");
	if( IS_ERR(vchar_drv.dev))
	{
		printk("failed to create a device\n");
        class_destroy(vchar_drv.dev_class);
        //return -1;
	}

	//Cap phat bo nho cho cac ctdl cua driver va khoi tao
	// vchar_drv.vchar_hw = kzalloc(sizeof(vchar_dev_t), GFP_KERNEL);
	// if(!vchar_drv.vchar_hw){
	// 	printk("failed to allocated data structure of the driver\n");
	// 	ret = -ENOMEM;
	// 	goto failed_allocate_structure;
	// }
	// vchar_drv.vcdev = cdev_alloc();
	// if(vchar_drv.vcdev == NULL){
	// 	printk("failed to allocate cdev structure\n");
	// 	goto failed_allocate_cdev;
	// }

	//Dang ky cac entry point voi kernel - Entry points register
	vchar_drv.vcdev = cdev_alloc();
	if( vchar_drv.vcdev == NULL){
		printk("failed to allocate cdev strucure");
		vchar_hw_exit(vchar_drv.vchar_hw);
	}

	cdev_init(vchar_drv.vcdev, &fops);
	ret = cdev_add(vchar_drv.vcdev, vchar_drv.dev_num,1);
	if(ret < 0){
		printk("failed to add character device to system\n");
		vchar_hw_exit(vchar_drv.vchar_hw);
		// device_destroy(vchar_drv.dev_class,vchar_drv.dev_num);
		// class_destroy(vchar_drv.dev_class);
		// unregister_chrdev_region(vchar_drv.dev_num,1);
		//return -1;
	}
	// Dang ky ham xu ly ngat

	printk("Initialize character driver successfully\n");
	return 0;



// failed_init_hw:
// 	kfree(vchar_drv.vchar_hw);

}

static void __exit exit_chardev(void) /* Destructor */
{
	// Huy dang ky entry point voi kernel
	//cdev_del(vchar_drv.vcdev);

	// Giai phong thiet bi vat ly
	// vchar_hw_exit(vchar_drv.vchar_hw);
	// // Giai phong bo nho da cap phat ctdl cua driver
	// kfree(vchar_drv.vchar_hw);
	// Giai phong device file
	device_destroy(vchar_drv.dev_class, vchar_drv.dev_num);
	class_destroy(vchar_drv.dev_class);
	//Giai phong device number
	unregister_chrdev_region(vchar_drv.dev_num,1);
    printk("Exit character driver\n");
}

module_init(init_chardev);
module_exit(exit_chardev);
MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_SUPPORTED_DEVICE("testdevice"); /* kieu device ma module support */