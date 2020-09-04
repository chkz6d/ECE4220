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

// Local Variables
iomap = ioremap(GPIO_BASE, BLOCK_SIZE);


/* Declare your pointers for mapping the necessary GPIO registers.
   You need to map:
   
   - Pin Event detect status register(s)
   - Rising Edge detect register(s) (either synchronous or asynchronous should work)
   - Function selection register(s)
   - Pin Pull-up/pull-down configuration registers
   
   Important: remember that the GPIO base register address is 0x3F200000, not the
   one shown in the BCM2835 ARM Peripherals manual.
*/

static void set_pin(void){
	iowrite32(1 << LED, iomap + GPSET0_OFFSET);
}

static void unset_pin(void)
{
	iowrite32(1 << LED, iomap + GPCLR0_OFFSET);
}

int init_module()
{
	printk("Begin INIT Instructions.\n");
	set_pin();
	printk("Finish INIT Instructions.\n");
	return 0;
}

void cleanup_module()
{
	printk("Begin CLEANUP Instructions.\n");
	unset_pin();
	printk("Finish CLEANUP Instructions.\n");
}