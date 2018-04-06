/*
Programming Assignment 3
COP 4600 Spring 18
Group 50
Christopher Hodge
Michael Jones
Marlee Knotts
*/

#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/mutex.h>
#include "extern_declare.h"

#define  DEVICE_NAME "groupFiftyLDriver"
#define  CLASS_NAME  "groupFifty"
#define  MAX_SIZE  1024

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Group 50");
MODULE_DESCRIPTION("Character-mode Linux device driver as kernel module for PA 2");
MODULE_VERSION("1.0");

static int    majorNumber;
char   message[1024] = {0};
static short  size_of_message;
static int    numberOpens = 0;
static char temp[MAX_SIZE];
char data[MAX_SIZE];
int data_size;
static struct class*  groupFiftycharClass  = NULL;
static struct device* groupFiftycharDevice = NULL;

DEFINE_MUTEX(groupfifty_mutex); // Declare the mutex lock.

// The prototype functions for the character driver -- must come before the struct definition
static int     dev_open(struct inode *, struct file *);
static int     dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);

static struct file_operations fops =
{
   .open = dev_open,
   .read = dev_read,
   .write = dev_write,
   .release = dev_release,
};

static int __init lkmchar_init(void){
   printk(KERN_INFO "groupFiftyLDriver: Initializing the groupFiftyLDriver LKM\n");

   // Try to dynamically allocate a major number for the device -- more difficult but worth it
   majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
   if (majorNumber<0){
      printk(KERN_ALERT "groupFiftyLDriver failed to register a major number\n");
      return majorNumber;
   }
   printk(KERN_INFO "groupFiftyLDriver: registered correctly with major number %d\n", majorNumber);

   // Register the device class
   groupFiftycharClass = class_create(THIS_MODULE, CLASS_NAME);
   if (IS_ERR(groupFiftycharClass)){                // Check for error and clean up if there is
      unregister_chrdev(majorNumber, DEVICE_NAME);
      printk(KERN_ALERT "Failed to register device class\n");
      return PTR_ERR(groupFiftycharClass);          // Correct way to return an error on a pointer
   }
   printk(KERN_INFO "groupFiftyLDriver: device class registered correctly\n");

   // Register the device driver
   groupFiftycharDevice = device_create(groupFiftycharClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
   if (IS_ERR(groupFiftycharDevice)){               // Clean up if there is an error
      class_destroy(groupFiftycharClass);           // Repeated code but the alternative is goto statements
      unregister_chrdev(majorNumber, DEVICE_NAME);
      printk(KERN_ALERT "Failed to create the device\n");
      return PTR_ERR(groupFiftycharDevice);
   }

   data_size = 0;

	for (i = 0; i < MAX_SIZE; i++) {
		data[i] = '\0';
	}

   printk(KERN_INFO "groupFiftyLDriver: device class created correctly\n"); // Made it! device was initialized
   mutex_init(&groupfifty_mutex);  //Initialize the mutex dynamically.
   return 0;
}

static void __exit lkmchar_exit(void){
   mutex_destroy(&groupfifty_mutex);                       // destroy the dynamically-allocated mutex
   device_destroy(groupFiftycharClass, MKDEV(majorNumber, 0));     // remove the device
   class_unregister(groupFiftycharClass);                          // unregister the device class
   class_destroy(groupFiftycharClass);                             // remove the device class
   unregister_chrdev(majorNumber, DEVICE_NAME);             // unregister the major number
   printk(KERN_INFO "groupFiftyLDriver: Goodbye from the LKM!\n");
}

static int dev_open(struct inode *inodep, struct file *filep){
   if(!mutex_trylock(&groupfifty_mutex)){                  // Try to acquire the mutex (returns 0 on fail)
	 printk(KERN_ALERT "EBBChar: Device in use by another process");
	 return -EBUSY;
   }
   numberOpens++;
   printk(KERN_INFO "groupFiftyLDriver: Device has been opened %d time(s)\n", numberOpens);
   return 0;
}

static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset){

    mutex_lock(&groupfifty_mutex); // Lock the mutex to perform the work.

    int i, j;
    int limit = MAX_SIZE, error_count = 0;

	printk(KERN_INFO "groupFiftyLDriver: Reading from device.\n");

	if (data_size == 0) {
		printk(KERN_INFO "groupFiftyLDriver: Tried to read empty buffer!\n");
	}

	if (len < data_size) {
		limit = len;
		data_size = data_size - len;
	} else {
		limit = data_size;
		data_size = 0;
	}

	for (i = limit, j = 0; i < MAX_SIZE; i++, j++) {
		temp[j] = data[i];
	}

	for ( ; j < MAX_SIZE; j++) {
		temp[j] = '\0';
	}

	for (i = 0; i < MAX_SIZE; i++) {
		data[i] = temp[i];
	}

   error_count = copy_to_user(buffer, message, data_size);

   if (error_count==0){
      printk(KERN_INFO "groupFiftyLDriver: Sent %d characters to the user\n", data_size);
      mutex_unlock(&groupfifty_mutex); // Unlock the mutex.
      return (data_size=0);
   }
   else {
      printk(KERN_INFO "groupFiftyLDriver: Failed to send %d characters to the user\n", error_count);
      mutex_unlock(&groupfifty_mutex); // Unlock the mutex.
      return -EFAULT;              // Failed -- return a bad address message (i.e. -14)
   }
}

static int dev_release(struct inode *inodep, struct file *filep){
   mutex_unlock(&groupfifty_mutex);                      // release the mutex (i.e., lock goes up)
   printk(KERN_INFO "groupFiftyLDriver: Device successfully closed\n");
   return 0;
}

module_init(lkm_init);
module_exit(lkm_exit);
