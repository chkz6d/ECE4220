#include <linux/module.h>
#include <linux/kernel.h>
// #include <asm/io.h>
// #include <linux/init.h>
// #include <linux/types.h>
// #include <linux/interrupt.h>
// #include <linux/delay.h>

int init_module()
{
	printk("Hello World.\n")
	return 0;
}

// Cleanup - undo whatever init_module did
void cleanup_module()
{
	printk("Goodbye World.\n");
}

module_init(init_module);
module_exit(cleanup_module);