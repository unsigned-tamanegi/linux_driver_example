#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include "nsystem.h"
#include "nsystem_util.h"

static void omega_bus_device_release(struct device *dev) {}

typedef uint64_t	minor_list_t;
static int			major      = 0;

static int
each_dev_ops(struct device* dev, void* arg)
{
	struct nsystem_device*	nsys_dev = to_nsystem_dev(dev);

	if(nsys_dev->arrive_data) {
		(nsys_dev->arrive_data)();
	}
	return 0;
}

static ssize_t
intr_debug_store(struct class *class, struct class_attribute *attr, const char *buf, size_t len)
{
	class_for_each_device(class, NULL, NULL, each_dev_ops);
	return len;
}

static ssize_t
intr_debug_show(struct class *class, struct class_attribute *attr, char *buf)
{
	const char		description[] = "[0]:data arrival interrupt\n";
	strcpy(buf, description);
	return sizeof(description);
}

static struct class_attribute nsystem_class_attrs[] = {
	__ATTR(intr_debug, S_IWUSR | S_IRUSR, intr_debug_show, intr_debug_store),
	__ATTR_NULL,
};

static char*
nsystem_devnode(struct device* dev, umode_t* mode)
{
	// remove時はmodeがNULLで渡ってくる
	if(mode != NULL) {
		*mode = S_IWUSR | S_IRUGO;
	}
	// kernelがudevに環境変数を送信した後、devtmpfs_create_nodeでkreeするためヒープを返す
	return kasprintf(GFP_KERNEL, "nsystem/%s", dev_name(dev));
}

static struct class nsystem_class = {
	.name        = "nsystem"
  , .owner       = THIS_MODULE
  , .class_attrs = nsystem_class_attrs
  , .devnode     = nsystem_devnode
};

int
nsystem_device_register(struct device* parent_dev, struct nsystem_device* nsys_dev)
{
	int						ret     = 0;

	// classデバイス
	nsys_dev->dev.parent  = parent_dev;
	nsys_dev->dev.release = omega_bus_device_release;
	nsys_dev->dev.class   = &nsystem_class; 
	nsys_dev->dev.devt    = MKDEV(major, get_minor_no());

	// /sys/classデバイス名の設定
	dev_set_name(&(nsys_dev->dev), "%s", nsys_dev->name);
	// classデバイスの生成
	ret = device_register(&(nsys_dev->dev));
	if(ret < 0) {
		nsystem_pr_err("cannot register class device.\n");
		goto err1;
	}
	return ret;
err1 :
	return ret;
}
EXPORT_SYMBOL(nsystem_device_register);

void
nsystem_device_unregister(struct nsystem_device* nsys_dev)
{
	device_unregister(&(nsys_dev->dev));
}
EXPORT_SYMBOL(nsystem_device_unregister);

__init int
nsystem_init(void)
{
	int				ret   = 0;
	dev_t			devno = (dev_t)0;

	ret = class_register(&nsystem_class);
	if(ret < 0) {
		nsystem_pr_err("cannot register class.\n");
		goto err1;
	}
	// 最終引数:/proc/devicesに表示される名称
	ret = alloc_chrdev_region(&devno, 0, NSYSTEM_MAX_CHAR_DEVICES, "nsystem");
	if(ret < 0) {
		nsystem_pr_err("cannot allocate chardev region.\n");
		goto err2;
	}
	major = MAJOR(devno);
	return ret;
err2 :
	class_unregister(&nsystem_class);
err1 :
	return ret;
}

__exit void
nsystem_exit(void)
{
	class_unregister(&nsystem_class);
	unregister_chrdev_region(MKDEV(major, 0), NSYSTEM_MAX_CHAR_DEVICES);
}

module_init(nsystem_init);
module_exit(nsystem_exit);
MODULE_LICENSE("GPL");
