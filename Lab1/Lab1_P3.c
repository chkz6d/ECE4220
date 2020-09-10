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

// Necessary static defines
#define GPIO_BASE 0x3F200000 // GPSEL0 is at base address
#define BLOCK_SIZE 4096
#define GPSEL1_OFFSET 0x04
#define GPSEL2_OFFSET 0x08
#define GPSET0_OFFSET 0x1c
#define GPCLR0_OFFSET 0x28
#define GPAREN0_OFFSET 0x7C
#define GPEDS0_OFFSET 0x40
#define GPPUD_OFFSET 0x94
#define GPPUDCLK0_OFFSET 0x98
#define LED 3
#define CLR_LED 6

// Helper functions
int init_led(void);
void exit_led(void);

// Want to toggle led based on button, need to configure this button with interupt
int init_led()
{
	// Instructions/Configuration for Red LED
	unsigned long *gpsel0 = (unsigned long*)ioremap(GPIO_BASE, BLOCK_SIZE);
	unsigned long *gpset0 = (unsigned long*)ioremap(GPIO_BASE + GPSET0_OFFSET, BLOCK_SIZE);
	unsigned long *gpclr0 = (unsigned long*)ioremap(GPIO_BASE + GPCLR0_OFFSET, BLOCK_SIZE);

    // Instructions/Configuration for Buttons
    // Can select pins 10-19 with this
    unsigned long *gpsel1 = (unsigned long*)ioremap(GPIO_BASE + GPSEL1_OFFSET, BLOCK_SIZE);
    unsigned long *gpsel2 = (unsigned long*)ioremap(GPIO_BASE + GPSEL2_OFFSET, BLOCK_SIZE);
    unsigned long *gppud0 = (unsigned long*)ioremap(GPIO_BASE + GPPUD_OFFSET, BLOCK_SIZE);
    unsigned long *gppudclk0 = (unsigned long*)ioremap(GPIO_BASE + GPPUDCLK0_OFFSET, BLOCK_SIZE);

	printk("Begin INIT Instructions.\n");
	

    // 4 for gpsel1 (buttons 1-4 BCM 16-19)
    iowrite32(0 << 18, gpsel1); // GPSEL pin 16 to output mode
    iowrite32(0 << 21, gpsel1); // GPSEL pin 17 to output mode
    iowrite32(0 << 24, gpsel1); // GPSEL pin 18 to output mode
    iowrite32(0 << 27, gpsel1); // GPSEL pin 19 to output mode

    // 1 for gpsel 2 (button 5 BCM 20)
    iowrite32(0, gpsel2); // GPSEL pin 20 to output mode

    // GPPUD Configuration to make all reads pulldown
    iowrite32(0b01, gppud0);
    udelay(100);
    iowrite32(0b11111000000000000000, gppudclk0);
    udelay(100);
    iowrite32(0b00, gppud0);
    udelay(100);
    iowrite32(0b00000000000000000000, gppudclk0);

    request_irq(79);

    // for LED
    iowrite32(1 << 3, gpsel0); // GPSEL pin 3 to output mode
	iowrite32(1 << 3, gpset0); // GPSET pin 3 to HIGH
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
