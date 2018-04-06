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

#define  MAX_SIZE  1024

// The prototype functions for the character driver -- must come before the struct definition
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);

static struct file_operations fops =
{
   .open = dev_open,
   .read = dev_read,
   .write = dev_write,
   .release = dev_release,
};

static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset){

  mutex_lock(&groupfifty_mutex); // Lock the mutex to perform the work.

   sprintf(message, "%s(%zu letters)", buffer, len);   // appending received string with its length
   data_size = strlen(message);                 // store the length of the stored message
   printk(KERN_INFO "groupFiftyLDriver: Received %zu characters from the user\n", len);

   if (len > MAX_SIZE) {
		printk(KERN_INFO "groupFiftyLDriver: Insufficient space\n");
	}

	for (i = 0, data_size = 0; i < MAX_SIZE; i++) {
		if (i >= len)
			data[i] = '\0';
		else {
			data[i] = buffer[i];
			data_size++;
		}
	}

  mutex_unlock(&groupfifty_mutex); // Unlock the mutex.

   return len;
}
