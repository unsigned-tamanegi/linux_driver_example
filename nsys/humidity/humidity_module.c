#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include "../../bus/omega.h"
#include "../nsystem.h"

extern struct nsystem_device nsys_dev;

#define humidity_mod_dev_emerg(fmt, arg...)	dev_emerg(&(nsys_dev.dev), fmt, ##arg)
#define humidity_mod_dev_crit(fmt, arg...)	dev_crit(&(nsys_dev.dev), fmt, ##arg)
#define humidity_mod_dev_alert(fmt, arg...)	dev_alert(&(nsys_dev.dev), fmt, ##arg)
#define humidity_mod_dev_err(fmt, arg...)	dev_err(&(nsys_dev.dev), fmt, ##arg)
#define humidity_mod_dev_warn(fmt, arg...)	dev_warn(&(nsys_dev.dev), fmt, ##arg)
#define humidity_mod_dev_info(fmt, arg...)	dev_info(&(nsys_dev.dev), fmt, ##arg)
#define humidity_mod_dev_dbg(fmt, arg...)	dev_dbg(&(nsys_dev.dev), fmt, ##arg)

static int
humidity_module_arrive(void)
{
	humidity_mod_dev_dbg("data arrive!\n");
	return 0;
}

struct nsystem_device nsys_dev = {
	.name     = "humidity"
  , .nsys_ops = {
	  .arrive_data = humidity_module_arrive
	}
};

static int
humidity_module_probe(struct omega_device *omega_dev)
{
	int			ret = 0;

	nsystem_pr_debug("[humidity module] probe called.\n");
	nsystem_device_register(&(omega_dev->dev), &nsys_dev, NULL);
	return ret;
}

static int
humidity_module_remove(struct omega_device *omega_dev)
{
	nsystem_device_unregister(&nsys_dev);
	nsystem_pr_debug("[humidity module] remove called.\n");
	return 0;
}

static void
humidity_module_shutdown(struct omega_device *omega_dev)
{
	nsystem_pr_debug("[humidity module] shutdown called.\n");
	return;
}

struct omega_driver humidity_mod_drv = {
	.name     = "humidity"
  , .vid      = 0x77dc
  , .pid      = 0x0003
  , .probe    = humidity_module_probe
  , .remove   = humidity_module_remove
  , .shutdown = humidity_module_shutdown
};

__init int
humidity_module_init(void)
{
	int			ret = 0;

	nsystem_pr_debug("[humidity module] init called.\n");
	ret = omega_driver_register(&humidity_mod_drv);
	if(ret < 0) {
		nsystem_pr_err("[humidity module] cannot register omega driver.\n");
		goto err1;
	}
	return ret;
err1 :
	return ret;
}

__exit void
humidity_module_exit(void)
{
	omega_driver_unregister(&humidity_mod_drv);
	nsystem_pr_debug("[humidity module] exit called.\n");
}

module_init(humidity_module_init);
module_exit(humidity_module_exit);
MODULE_LICENSE("GPL");
