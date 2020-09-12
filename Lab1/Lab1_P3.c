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

// Necessary static defines (Note need to divide all IO addresses by 4 to get into bytes)
#define GPIO_BASE 0x3F200000 // GPSEL0 is at base address
#define BLOCK_SIZE 4096
#define GPSEL1_OFFSET 0x01
#define GPSEL2_OFFSET 0x02
#define GPSET0_OFFSET 0x07
#define GPCLR0_OFFSET 0x0A
#define GPAREN0_OFFSET 0x1F
#define GPEDS0_OFFSET 0x10
#define GPPUD_OFFSET 0x25
#define GPPUDCLK0_OFFSET 0x26
#define LED 3
#define CLR_LED 6

// Helper functions
int init_led(void);
void exit_led(void);

// Misc variables
int mydev_id; // variable needed to idenitify the handler

static irqreturn_t button_isr(int irq, void *dev_id){
    disable_irq_nosync(79); // Disable 79

    // Instructions/Configuration for Red LED and Button reads
	unsigned long *gpsel0       = (unsigned long*)ioremap(GPIO_BASE, BLOCK_SIZE);
	unsigned long *gpset0       = gpsel0 + GPSET0_OFFSET;
	unsigned long *gpclr0       = gpsel0 + GPCLR0_OFFSET;
    unsigned long *gpeds0       = gpsel0 + GPEDS0_OFFSET;
    
    if ( (*gpeds0 + 16) == 1 || (*gpeds0 + 17) == 1 || (*gpeds0 + 18) == 1 || (*gpeds0 + 19) == 1 || (*gpeds0 + 20) == 1 ){
        iowrite32(1 << 3, gpsel0); // GPSEL pin 2 to output mode
        iowrite32(1 << 3, gpset0);
    }
    else{
        iowrite32(1 << 6, gpsel0); // GPSEL pin 2 to output mode
	    iowrite32(1 << 3, gpclr0); // GPCLR pin 2 to LOW
    }

    printk("Interrupt handled\n");
    enable_irq(79); // Re-enable 79iowrite32(1 << 3, gpclr0); // GPCLR pin 3 to LOW
}

// Want to toggle led based on button, need to configure this button with interupt
int init_led()
{
    int dummy = 0;

    // Instructions/Configuration for Buttons
    // Can select pins 10-19 with this
    unsigned long *gpsel0       = (unsigned long*)ioremap(GPIO_BASE, BLOCK_SIZE);
    unsigned long *gpsel1       = gpsel0 + GPSEL1_OFFSET;
    unsigned long *gpsel2       = gpsel0 + GPSEL2_OFFSET;
    unsigned long *gppud        = gpsel0 + GPPUD_OFFSET;
    unsigned long *gppudclk0    = gpsel0 + GPPUDCLK0_OFFSET;
    unsigned long *gparen0      = gpsel0 + GPAREN0_OFFSET;

	printk("Begin INIT Instructions.\n");
	
    // 4 for gpsel1 (buttons 1-4 BCM 16-19)
    iowrite32(0 << 18, gpsel1); // GPSEL pin 16 to input mode
    iowrite32(0 << 21, gpsel1); // GPSEL pin 17 to input mode
    iowrite32(0 << 24, gpsel1); // GPSEL pin 18 to input mode
    iowrite32(0 << 27, gpsel1); // GPSEL pin 19 to input mode

    // 1 for gpsel 2 (button 5 BCM 20)
    iowrite32(0, gpsel2); // GPSEL pin 20 to input mode

    // GPPUD Configuration to make all reads pulldown
    iowrite32(0b01, gppud);
    udelay(100);
    iowrite32(0b11111000000000000000, gppudclk0);
    udelay(100);
    iowrite32(0, gppud);
    iowrite32(0, gppudclk0);

    enable_irq(79);
    dummy = request_irq(79, button_isr, IRQF_SHARED, "Button_handler", &mydev_id);

	printk("Finish INIT Instructions.\n");
	return 0;
}

void exit_led()
{
	printk("Begin CLEANUP Instructions.\n");
	free_irq(79, &mydev_id);
	printk("Finish CLEANUP Instructions.\n");
}

module_init(init_led);
module_exit(exit_led);
