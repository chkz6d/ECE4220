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
#define GPSET0_OFFSET 0x07
#define GPCLR0_OFFSET 0x0A

// Helper functions
int start_led(void);
void end_led(void);

int start_led()
{
	// Local function variables
	unsigned long *gpsel0 = (unsigned long*)ioremap(GPIO_BASE, BLOCK_SIZE);
	unsigned long *gpset0 = gpsel0 + GPSET0_OFFSET;
	unsigned long *gpclr0 = gpsel0 + GPCLR0_OFFSET;

	printk("%X", gpsel0);
	printk("%X", gpsel0 + 1);
	printk("%X", gpset0);
	printk("%X", gpclr0);
	iowrite32(1 << 3, gpsel0); // GPSEL pin 3 to output mode
	iowrite32(1 << 3, gpset0); // GPSET pin 3 to HIGH
	return 0;
}

void end_led()
{
	// Local function variables
	unsigned long *gpsel0 = (unsigned long*)ioremap(GPIO_BASE, BLOCK_SIZE);
	unsigned long *gpset0 = gpsel0 + GPSET0_OFFSET;
	unsigned long *gpclr0 = gpsel0 + GPCLR0_OFFSET;

	iowrite32(1 << 6, gpsel0); // GPSEL pin 3 to output mode
	iowrite32(1 << 3, gpclr0); // GPCLR pin 3 to LOW
	iounmap(gpsel0);
}

module_init(start_led);
module_exit(end_led);
