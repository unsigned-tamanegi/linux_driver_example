#include "nsystem.h"
#include "nsystem_sysfs.h"
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/device.h>

static ssize_t
nsystem_status_show(struct device* dev, struct device_attribute* attr, char* buf)
{
	ssize_t					ret      = 0;
	struct nsystem_device*	nsys_dev = to_nsystem_dev(dev);

	if(nsys_dev->status == STATUS_STOP) {
		ret = sprintf(buf, "%s\n", "stop");
	}
	else if(nsys_dev->status == STATUS_START) {
		ret = sprintf(buf, "%s\n", "start");
	}
	else if(nsys_dev->status == STATUS_UPDATE) {
		ret = sprintf(buf, "%s\n", "update");
	}
	return ret;
}

static ssize_t
nsystem_status_store(struct device* dev, struct device_attribute* attr, const char* buf, size_t size)
{
	ssize_t					ret      = size;
	struct nsystem_device*	nsys_dev = to_nsystem_dev(dev);

	if(strcmp(buf, "stop") == 0 || strcmp(buf, "stop\n") == 0) {
		nsys_dev->status = STATUS_STOP;
	}
	else if(strcmp(buf, "start") == 0 || strcmp(buf, "start\n") == 0) {
		nsys_dev->status = STATUS_START;
	}
	else if(strcmp(buf, "update") == 0 || strcmp(buf, "update\n") == 0) {
		nsys_dev->status = STATUS_UPDATE;
	}
	else {
		ret = -EINVAL;
	}
	return ret;
}

static ssize_t
nsystem_sampling_rate_show(struct device* dev, struct device_attribute* attr, char* buf)
{
	struct nsystem_device*	nsys_dev = to_nsystem_dev(dev);

	return sprintf(buf, "%d\n", nsys_dev->sampling_rate);
}

static ssize_t
nsystem_sampling_rate_store(struct device* dev, struct device_attribute* attr, const char* buf, size_t size)
{
	ssize_t					ret      = size;
	struct nsystem_device*	nsys_dev = to_nsystem_dev(dev);
	int						sr       = 0;
	char					*endp    = NULL;

	sr = simple_strtol(buf, &endp, 0);
	if(endp[0] != '\0' && endp[0] != '\n') {
		ret = -EINVAL;
		goto out;
	}
	nsys_dev->sampling_rate = sr;
out :
	return ret;
}

static DEVICE_ATTR(sampling_rate, S_IWUSR | S_IRUGO, nsystem_sampling_rate_show, nsystem_sampling_rate_store);
static DEVICE_ATTR(status,        S_IWUSR | S_IRUGO, nsystem_status_show,        nsystem_status_store);

struct attribute* nsys_attrs[] = {
	&(dev_attr_sampling_rate.attr)
  , &(dev_attr_status.attr)
  , NULL
};

static const struct attribute_group nsys_attr_group = {
#if 0
	// ディレクトリに入れる場合名前を定義
	.name  = "dirname",
#endif
	.attrs = nsys_attrs
};

static const struct attribute_group *nsys_attr_groups[] = {
	&nsys_attr_group
  , NULL
};

void
nsystem_init_sysfs(struct nsystem_device* nsys_dev)
{
	nsys_dev->dev.groups = nsys_attr_groups;
}

EXPORT_SYMBOL(nsystem_init_sysfs);
