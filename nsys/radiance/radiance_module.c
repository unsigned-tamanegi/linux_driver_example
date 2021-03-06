#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <asm/uaccess.h>
#include "../../bus/omega.h"
#include "../nsystem.h"

extern struct nsystem_device nsys_dev;

#define radiance_mod_dev_emerg(fmt, arg...)	dev_emerg(&(nsys_dev.dev), fmt, ##arg)
#define radiance_mod_dev_crit(fmt, arg...)	dev_crit(&(nsys_dev.dev), fmt, ##arg)
#define radiance_mod_dev_alert(fmt, arg...)	dev_alert(&(nsys_dev.dev), fmt, ##arg)
#define radiance_mod_dev_err(fmt, arg...)	dev_err(&(nsys_dev.dev), fmt, ##arg)
#define radiance_mod_dev_warn(fmt, arg...)	dev_warn(&(nsys_dev.dev), fmt, ##arg)
#define radiance_mod_dev_info(fmt, arg...)	dev_info(&(nsys_dev.dev), fmt, ##arg)
#define radiance_mod_dev_dbg(fmt, arg...)	dev_dbg(&(nsys_dev.dev), fmt, ##arg)

static int
radiance_module_arrive(void)
{
	radiance_mod_dev_dbg("arrive data.\n");
	return 0;
}

static int
radiance_open(struct nsystem_device* nsys_dev)
{
	printk("\t[from %s] open!!\n", nsys_dev->name);
	return 0;
}

static ssize_t
radiance_read(struct nsystem_device* nsys_dev, char* __user buf, size_t len, loff_t* off)
{
	char*						msg      = NULL;
	ssize_t						ret      = 0;
	static int					isrun    = 0;

	printk("\t[from %s] read!!\n", nsys_dev->name);
	isrun = (isrun + 1) & 0x01;
	if(isrun) {
		msg = kasprintf(GFP_KERNEL, "\t[from %s] read!!\n", nsys_dev->name);
		if(msg == NULL) {
			ret = -EIO;
			goto out; 
		}
		ret = strlen(msg) + 1;
		copy_to_user(buf, msg, min(strlen(msg) + 1, len));
		kfree(msg);
	}
out :
	return ret;
}

static ssize_t
radiance_write(struct nsystem_device* nsys_dev, const char* __user buf, size_t len, loff_t* off)
{
	printk("\t[from %s] write!!\n", nsys_dev->name);
	return len;
}

static int
radiance_close(struct nsystem_device* nsys_dev)
{
	printk("\t[from %s] close!!\n", nsys_dev->name);
	return 0;
}

struct nsystem_device nsys_dev = {
	.name     = "radiance"
  , .nsys_ops = {
		.arrive_data = radiance_module_arrive
	}
  , .chr_ops = {
		.open  = radiance_open
	  , .read  = radiance_read
	  , .write = radiance_write
	  , .close = radiance_close
	}
};

static int
radiance_module_probe(struct omega_device *omega_dev)
{
	return nsystem_device_register(&(omega_dev->dev), &nsys_dev, NULL);
}

static int
radiance_module_remove(struct omega_device *omega_dev)
{
	nsystem_device_unregister(&nsys_dev);
	return 0;
}

static void
radiance_module_shutdown(struct omega_device *omega_dev)
{
	printk("=C shutdown radiance module!!\n");
	return;
}

struct omega_driver radiance_mod_drv = {
	.name     = "radiance"
  , .vid      = 0x77dc
  , .pid      = 0x0002
  , .probe    = radiance_module_probe
  , .remove   = radiance_module_remove
  , .shutdown = radiance_module_shutdown
};

__init int
radiance_module_init(void)
{
	int			ret = 0;

	nsystem_pr_debug("[radiance module] init called.\n");
	ret = omega_driver_register(&radiance_mod_drv);
	if(ret < 0) {
		printk("error %s:%d\n", __FILE__, __LINE__);
		goto err1;
	}
	return ret;
err1 :
	return ret;
}

__exit void
radiance_module_exit(void)
{
	omega_driver_unregister(&radiance_mod_drv);
	nsystem_pr_debug("[radiance module] exit called.\n");
}

module_init(radiance_module_init);
module_exit(radiance_module_exit);
MODULE_LICENSE("GPL");
