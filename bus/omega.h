#ifndef __OMEGA_H_INCLUDED__ /* { */
#define __OMEGA_H_INCLUDED__ /* }{ */

#include <linux/device.h>

struct omega_device {
	int						module_index;
	uint16_t				vid;
	uint16_t				pid;
	char					*description;
	struct omega_driver*	omega_drv;
	struct device			dev;
};

struct omega_driver {
	char*					name;
	uint16_t				vid;
	uint16_t				pid;

	int (*probe) (struct omega_device *omega_dev);
	int (*remove) (struct omega_device *omega_dev);
	void (*shutdown) (struct omega_device *omega_dev);
	
	struct device_driver	drv;
};

int omega_device_register(struct omega_device* omega_dev);
void omega_device_unregister(struct omega_device* omega_dev);
int omega_driver_register(struct omega_driver* omega_drv);
void omega_driver_unregister(struct omega_driver* omega_drv);

#define to_omega_bus(ptr)	container_of(ptr, struct omega_bus_type, bus)
#define to_omega_dev(ptr)	container_of(ptr, struct omega_device, dev)
#define to_omega_drv(ptr)	container_of(ptr, struct omega_driver, drv)

#define omega_pr_emerg(fmt, args...)	pr_emerg("omega: " fmt, ##args) 
#define omega_pr_alert(fmt, args...)	pr_alert("omega: " fmt, ##args)
#define omega_pr_crit(fmt, args...)		pr_crit("omega: " fmt, ##args)
#define omega_pr_err(fmt, args...)		pr_err("omega: " fmt, ##args)
#define omega_pr_warn(fmt, args...)		pr_warn("omega: " fmt, ##args)
#define omega_pr_notice(fmt, args...)	pr_notice("omega: " fmt, ##args)
#define omega_pr_info(fmt, args...)		pr_info("omega: " fmt, ##args)
#define omega_pr_cont(fmt, args...)		pr_cont("omega: " fmt, ##args)
#define omega_pr_devel(fmt, args...)	pr_devel("omega: " fmt, ##args)
#define omega_pr_debug(fmt, args...)	pr_debug("omega: " fmt, ##args)

#endif /* } */ 
