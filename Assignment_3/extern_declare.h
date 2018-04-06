/*
Programming Assignment 3
COP 4600 Spring 18
Group 50
Christopher Hodge
Michael Jones
Marlee Knotts
*/

// Header file for extern declaration so that each module can share memory.
// These variables are only defined in the input_device.c file and they connot be static.


// Dont know which ones we need so everybody gets to come to the party.
#include <linux/mutex.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

extern int data;
extern int data_size;
extern char message[1024];
extern pthread_mutex_t groupfifty_mutex;
