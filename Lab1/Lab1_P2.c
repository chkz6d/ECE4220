/* ece4220lab1_isr.c
 * ECE4220/7220
 * Author: Luis Alberto Rivera
 
 Basic steps needed to configure GPIO interrupt and attach a handler.
 Check chapter 6 of the BCM2837 ARM Peripherals manual for details about
 the GPIO registers, how to configure, set, clear, etc.
 
 Note: this code is not functional. It shows some of the actual code that you need,
 but some parts are only descriptions, hints or recommendations to help you
 implement your module.
 
 You can compile your module using the same Makefile provided. Just add
 obj-m += YourModuleName.o
 */

#ifndef MODULE
#define MODULE
#endif
#ifndef __KERNEL__
#define __KERNEL__
#endif

#include <linux/module.h>
#include <linux/kernel.h>
#include <asm/io.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/interrupt.h>
#include <linux/delay.h>

MODULE_LICENSE("GPL");

// Function address defintions
#define GPIO_BASE 0x3F200000 // GPSEL0 is at base address
#define BLOCK_SIZE 4096
#define GPSET0_OFFSET 0x1c
#define GPCLR0_OFFSET 0x28
#define LED 3
#define HIGH 1
#define LOW 0

// Helper functions
int init_led(void);
void exit_led(void);
static void set_pin(void);
static void unset_pin(void);

// Global Variables
static int gpio_num = LED;




int init_led()
{
	// Local function variables
	unsigned long *gpsel0 = (unsigned long*)ioremap(GPIO_BASE, BLOCK_SIZE);
	unsigned long *gpset0 = gpset0 + GPSET0_OFFSET;
	unsigned long *gpclr0 = gpset0 + GPCLR0_OFFSET;

	printk("Begin INIT Instructions.\n");
	iowrite32(1 << gpio_num, gpsel0); // GPSEL pin 3 to output mode
	iowrite32(1 << gpio_num, gpset0); // GPSET pin 3 to HIGH
	set_pin();
	printk("Finish INIT Instructions.\n");
	return 0;
}

void exit_led()
{
	// Local function variables
	unsigned long *gpsel0 = (unsigned long*)ioremap(GPIO_BASE, BLOCK_SIZE);
	unsigned long *gpset0 = gpset0 + GPSET0_OFFSET;
	unsigned long *gpclr0 = gpset0 + GPCLR0_OFFSET;

	printk("Begin CLEANUP Instructions.\n");
	iowrite32(1 << gpio_num, gpclr0); // GPCLR pin 3 to LOW
	iounmap(gpsel0);
	printk("Finish CLEANUP Instructions.\n");
}

module_init(init_led);
module_exit(exit_led);
