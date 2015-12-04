#ifndef __NSYSTEM_H_INCLUDED__ /* { */
#define __NSYSTEM_H_INCLUDED__ /* }{ */

#include <linux/device.h>
#include <linux/cdev.h>

struct nsystem_device;

struct nsyschr_file_operations
{
	int (*open)(struct nsystem_device* nsys_dev);
	ssize_t (*read)(struct nsystem_device* nsys_dev, char* __user buf, size_t len, loff_t* off);
	ssize_t (*write)(struct nsystem_device* nsys_dev, const char* __user buf, size_t len, loff_t* off);
	int (*close)(struct nsystem_device* nsys_dev);
};

struct nsys_operations
{
	int	(*arrive_data)(void);
};

struct nsystem_device
{
	char*							name;
	struct device					dev;
	struct cdev						c_dev;
	struct nsys_operations			nsys_ops;
	struct nsyschr_file_operations	chr_ops;
};


int nsystem_device_register(struct device* dev, struct nsystem_device* nsys_dev, struct file_operations* filp);
void nsystem_device_unregister(struct nsystem_device* dev);

#define NSYSTEM_MAX_CHAR_DEVICES	(64)
#define to_nsystem_chrdev(ptr)		container_of(ptr, struct nsystem_device, c_dev)
#define to_nsystem_dev(ptr)			container_of(ptr, struct nsystem_device, dev)

#define nsystem_pr_emerg(fmt, args...)	pr_emerg("nsystem: " fmt, ##args) 
#define nsystem_pr_alert(fmt, args...)	pr_alert("nsystem: " fmt, ##args)
#define nsystem_pr_crit(fmt, args...)	pr_crit("nsystem: " fmt, ##args)
#define nsystem_pr_err(fmt, args...)	pr_err("nsystem: " fmt, ##args)
#define nsystem_pr_warn(fmt, args...)	pr_warn("nsystem: " fmt, ##args)
#define nsystem_pr_notice(fmt, args...)	pr_notice("nsystem: " fmt, ##args)
#define nsystem_pr_info(fmt, args...)	pr_info("nsystem: " fmt, ##args)
#define nsystem_pr_cont(fmt, args...)	pr_cont("nsystem: " fmt, ##args)
#define nsystem_pr_devel(fmt, args...)	pr_devel("nsystem: " fmt, ##args)
#define nsystem_pr_debug(fmt, args...)	pr_debug("nsystem: " fmt, ##args)

#endif /* } */ 
