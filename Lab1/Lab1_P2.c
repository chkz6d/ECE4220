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
static void set_pin(void);
static void unset_pin(void);

// Global Variables
static void __iomem *iomap; // iomap is the base memory address at which we will do our manipulations
static int gpio_num = LED;


int init_led()
{
	printk("Begin INIT Instructions.\n");
	iomap = ioremap(GPIO_BASE, BLOCK_SIZE);
	set_pin();
	printk("Finish INIT Instructions.\n");
	return 0;
}

void exit_led()
{
	printk("Begin CLEANUP Instructions.\n");
	unset_pin();
	iounmap(iomap);
	printk("Finish CLEANUP Instructions.\n");
}

static void set_pin(void){
	iowrite32(1 << gpio_num, iomap + GPSET0_OFFSET);
}

static void unset_pin(void)
{
	iowrite32(1 << gpio_num, iomap + GPCLR0_OFFSET);
}

module_init(init_led);
module_exit(exit_led);