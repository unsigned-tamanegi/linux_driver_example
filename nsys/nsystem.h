#ifndef __NSYSTEM_H_INCLUDED__ /* { */
#define __NSYSTEM_H_INCLUDED__ /* }{ */

#include <linux/device.h>
#include <linux/cdev.h>

struct nsystem_device {
	char*					name;
	int						(*arrive_data)(void);
	struct device			dev;
};

int nsystem_device_register(struct device* dev, struct nsystem_device* nsys_dev);
void nsystem_device_unregister(struct nsystem_device* dev);

#define NSYSTEM_MAX_CHAR_DEVICES	(64)
#define to_nsystem_chrdev(ptr)		container_of(ptr, struct nsystem_chrdevice, dev)
#define to_nsystem_dev(ptr)			container_of(ptr, struct nsystem_device, dev)

#define nsystem_pr_emerg(fmt, args...)	pr_emerg("nsystem: " fmt, ##args) 
#define nsystem_pr_alert(fmt, args...)	pr_alert("nsystem: " fmt, ##args)
#define nsystem_pr_crit(fmt, args...)	pr_crit("nsystem: " fmt, ##args)
#define nsystem_pr_err(fmt, args...)		pr_err("nsystem: " fmt, ##args)
#define nsystem_pr_warn(fmt, args...)	pr_warn("nsystem: " fmt, ##args)
#define nsystem_pr_notice(fmt, args...)	pr_notice("nsystem: " fmt, ##args)
#define nsystem_pr_info(fmt, args...)	pr_info("nsystem: " fmt, ##args)
#define nsystem_pr_cont(fmt, args...)	pr_cont("nsystem: " fmt, ##args)
#define nsystem_pr_devel(fmt, args...)	pr_devel("nsystem: " fmt, ##args)
#define nsystem_pr_debug(fmt, args...)	pr_debug("nsystem: " fmt, ##args)

#endif /* } */ 
