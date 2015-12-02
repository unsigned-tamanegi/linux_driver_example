#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/device.h>
#include "omega.h"

static void link_module_dev_release(struct device *dev) {}
static struct omega_device	link_module_dev;

__init int
link_module_dev_init(void)
{
	printk("-- link module connected!\n");
	link_module_dev.module_index = 3;
	link_module_dev.vid          = 0x77dc;
	link_module_dev.pid          = 0x0004;
	link_module_dev.description  = "Nsystem Link module";
	link_module_dev.dev.release  = link_module_dev_release;
	return omega_device_register(&link_module_dev);
}

__exit void
link_module_dev_exit(void)
{
	omega_device_unregister(&link_module_dev);
	printk("-- link module disconnected!\n");
}

module_init(link_module_dev_init);
module_exit(link_module_dev_exit);
MODULE_LICENSE("GPL");

