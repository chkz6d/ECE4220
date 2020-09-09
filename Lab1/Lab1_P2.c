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
#define CLR_LED 6

// Helper functions
int init_led(void);
void exit_led(void);

int init_led()
{
	// Local function variables
	unsigned long *gpsel0 = (unsigned long*)ioremap(GPIO_BASE, BLOCK_SIZE);
	unsigned long *gpset0 = gpsel0 + GPSET0_OFFSET;
	unsigned long *gpclr0 = gpsel0 + GPCLR0_OFFSET;

	printk("Begin INIT Instructions.\n");
	iowrite32(1 << LED, gpsel0); // GPSEL pin 3 to output mode
	iowrite32(1 << LED, gpset0); // GPSET pin 3 to HIGH
	printk("Finish INIT Instructions.\n");
	return 0;
}

void exit_led()
{
	// Local function variables
	unsigned long *gpsel0 = (unsigned long*)ioremap(GPIO_BASE, BLOCK_SIZE);
	unsigned long *gpset0 = gpsel0 + GPSET0_OFFSET;
	unsigned long *gpclr0 = gpsel0 + GPCLR0_OFFSET;

	printk("Begin CLEANUP Instructions.\n");
	iowrite32(1 << CLR_LED, gpsel0); // GPSEL pin 3 to output mode
	iowrite32(1 << LED, gpclr0); // GPCLR pin 3 to LOW
	iounmap(gpsel0);
	printk("Finish CLEANUP Instructions.\n");
}

module_init(init_led);
module_exit(exit_led);
