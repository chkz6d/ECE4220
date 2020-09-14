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
#define GPLEV0_OFFSET 0x0D
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
    unsigned long *gplev0       = gpsel0 + GPLEV0_OFFSET;
    
    unsigned long pin_used_raw = ioread32(gpeds0);
    unsigned long pin_mask = 0b00000000000011111000000000000000;
    unsigned long pin_used = pin_used_raw & pin_mask;

    printk("%lx", pin_used);

    // GPSEL pins 5, 4, 3, and 2 to output mode (LEDS)
    iowrite32(0b001001001001000000, gpsel0); 

    switch (pin_used)
    {
        case 0X10000:
            if( (ioread32(gplev0) & 0b100) == 0b0){
                iowrite32(0b100, gpset0);
            }
            else{
                iowrite32(0b100, gpclr0);
            }
            printk("Button 1 (BCM 16) Used");
            break;
        case 0X20000:
            if( (ioread32(gplev0) & 0b1000) == 0b0){
                iowrite32(0b1000, gpset0);
            }
            else{
                iowrite32(0b1000, gpclr0);
            }
            printk("Button 2 (BCM 17) Used");
            break;
        case 0X40000:
            if( (ioread32(gplev0) & 0b10000) == 0b0){
                iowrite32(0b10000, gpset0);
            }
            else{
                iowrite32(0b10000, gpclr0);
            }
            printk("Button 3 (BCM 18) Used");
            break;
        case 0X80000:
            if( (ioread32(gplev0) & 0b100000) == 0b0){
                iowrite32(0b100000, gpset0);
            }
            else{
                iowrite32(0b100000, gpclr0);
            }
            printk("Button 4 (BCM 19) Used");
            break;
        case 0:
            if( (ioread32(gplev0) & 0b100000) == 0b0){
                iowrite32(0b111100, gpset0);
            }
            else{
                iowrite32(0b111100, gpclr0);
            }
            printk("Button 5 (BCM 20) Used");
            break;
        default:
            break;
    }

    iowrite32(0b00000000000111110000000000000000, gpeds0);
    enable_irq(79); // Re-enable 79iowrite32(1 << 3, gpclr0); // GPCLR pin 3 to LOW

    printk("Interrupt handled\n");
    
    return IRQ_HANDLED;
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
    iowrite32(0x0, gpsel1); // GPSEL pin 16 to input mode

    // 1 for gpsel 2 (button 5 BCM 20)
    iowrite32(0x0, gpsel2); // GPSEL pin 20 to input mode

    // GPPUD Configuration to make all reads pulldown
    iowrite32(0b01, gppud);
    udelay(100);
    iowrite32(0b00000000000111110000000000000000, gppudclk0);
    udelay(100);
    iowrite32(0x0, gppud);
    iowrite32(0x0, gppudclk0);
    iowrite32(0b00000000000111110000000000000000, gparen0);

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
