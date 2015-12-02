#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/device.h>
#include "omega.h"

static void omega_device_release(struct device *dev) {}
static void omega_bus_device_release(struct device *dev) {}

/*
 * omegaのdevice構造体 /sys/devices/直下に現れるomegaデバイスの親デバイス
 */
struct device omega_bus_device = {
	.release   = omega_bus_device_release
};

/*
 * omegaにデバイスが追加、削除される度に
 * udevに独自環境変数OMEGA_VIDとOMEGA_PIDをエクスポートする
 */
static int
omega_bus_uevent(struct device *dev, struct kobj_uevent_env* env)
{
	int					ret      = 0;
	struct omega_device* omega_dev = to_omega_dev(dev);

	ret = add_uevent_var(env, "OMEGA_VID=0x%04x", omega_dev->vid);
	if(ret < 0) {
		goto err;
	}
	return add_uevent_var(env, "OMEGA_PID=0x%04x", omega_dev->pid);
err :
	return ret;
}

/*
 * omegaバス上にdriverとdeviceが追加される度、deviceに一致したdriverかをチェックする。
 * 一致した場合0以外を返却すると、struct deviceのdriverが設定され、probeメソッドが
 * コールされる。probeメソッドないで必要があれば初期化処理等を入れる
 */
static int
omega_bus_match(struct device *dev, struct device_driver *drv)
{
	int					ret = 0;
	struct omega_device* omega_dev = to_omega_dev(dev);
	struct omega_driver* omega_drv = to_omega_drv(drv);

	if(omega_dev->pid == omega_drv->pid && omega_dev->vid == omega_drv->vid) {
		omega_pr_debug("driver and device are matched[%s]\n", omega_dev->description);
		ret = 1;
	}
	return ret;
}

/*
 * bus構造体。device、driverに同一のbus構造体を設定することにより、
 * match関数が呼ばれる準備ができる。(例えばpcomega deviceとusbbus driverは種類が異なるためmatch関数が呼ばれない)
 */
struct bus_type omega_bus_type = {
	.name   = "omega"
  , .match  = omega_bus_match
  , .uevent = omega_bus_uevent
};

/*
 * deviceとdriverが一致した場合呼ばれる。下記関数ではdevice及びdriverをラップしている特化したprobe関数を呼び出す。
 * 通常特化probe関数では初期化処理を入れる。
 */
static int
omega_driver_probe(struct device* dev)
{
	int					ret = 0;
	struct omega_device*	omega_dev = to_omega_dev(dev);
	struct omega_driver*	omega_drv = to_omega_drv(dev->driver);

	omega_dev->omega_drv = omega_drv;

	if(omega_drv->probe) {
		ret = omega_drv->probe(omega_dev);
	}
	return ret;
}

/*
 * すでにmatch関数で一致しているdriverもしくはdeviceが削除されると呼ばれる。
 */
static int
omega_driver_remove(struct device* dev)
{
	int					ret = 0;
	struct omega_device*	omega_dev = to_omega_dev(dev);
	struct omega_driver*	omega_drv = to_omega_drv(dev->driver);

	if(omega_drv->remove) {
		ret = omega_drv->remove(omega_dev);
	}
	return ret;
}

static void
omega_driver_shutdown(struct device* dev)
{
	struct omega_device*	omega_dev = to_omega_dev(dev);
	struct omega_driver*	omega_drv = to_omega_drv(dev->driver);

	if(omega_drv->shutdown) {
		omega_drv->shutdown(omega_dev);
	}
	return;
}

/*
 * driverの登録の特化関数。match関数が呼ばれる準備をする。
 * omegaに特化しており、busにomega_bus_typeを指定。これらbusやmethodなどは
 * 呼び出し元が意識することなく設定される。
 */
int
omega_driver_register(struct omega_driver* omega_drv)
{
	omega_drv->drv.bus      = &omega_bus_type;
	omega_drv->drv.name     = omega_drv->name;
	omega_drv->drv.probe    = omega_driver_probe;
	omega_drv->drv.remove   = omega_driver_remove;
	omega_drv->drv.shutdown = omega_driver_shutdown;
	return driver_register(&(omega_drv->drv));
}
EXPORT_SYMBOL(omega_driver_register);

void
omega_driver_unregister(struct omega_driver* omega_drv)
{
	driver_unregister(&(omega_drv->drv));
}
EXPORT_SYMBOL(omega_driver_unregister);

/*
 * omega_driver同様。
 */
int
omega_device_register(struct omega_device* omega_dev)
{
	omega_dev->dev.bus    = &omega_bus_type;   // /sys/bus/omegaが生成される
	omega_dev->dev.parent = &omega_bus_device; // /sys/device/omegaの子としてデバイスが生成される
	dev_set_name(&(omega_dev->dev), "omega:%02d", omega_dev->module_index);

	return device_register(&(omega_dev->dev));
}
EXPORT_SYMBOL(omega_device_register);

void
omega_device_unregister(struct omega_device* omega_dev)
{
	device_unregister(&(omega_dev->dev));
}
EXPORT_SYMBOL(omega_device_unregister);

// とりあえずモジュール3つぶん
static struct omega_device	omega_dev[3];

/*
 * モジュールロード時バスをスキャンしてデバイスの接続状態を調べると仮定した関数
 */
static int omega_bus_scan(void)
{
	// omegaのレジスタをスキャンして3モジュールあったと仮定
	// 本来ならレジスタの情報より接続されているデバイス情報を取得 {
	int					ret   = 0;
	int					index = 0;

	omega_dev[index].module_index = index;
	omega_dev[index].vid          = 0x77dc;
	omega_dev[index].pid          = 0x0001;
	omega_dev[index].description  = "Nsystem Main module";
	omega_dev[index].dev.release  = omega_device_release;
	ret = omega_device_register(&omega_dev[0]);
	index++;
	if(ret < 0) {
		omega_pr_err("cannot register omega device.[%d]\n", index - 1);
		goto err1;
	}

	omega_dev[index].module_index = index;
	omega_dev[index].vid          = 0x77dc;
	omega_dev[index].pid          = 0x0002;
	omega_dev[index].description  = "Nsystem radiance module";
	omega_dev[index].dev.release  = omega_device_release;
	ret = omega_device_register(&omega_dev[index]);
	index++;
	if(ret < 0) {
		omega_pr_err("cannot register omega device.[%d]\n", index - 1);
		goto err2;
	}

	omega_dev[index].module_index = index;
	omega_dev[index].vid          = 0x77dc;
	omega_dev[index].pid          = 0x0003;
	omega_dev[index].description  = "Nsystem humidity module";
	omega_dev[index].dev.release  = omega_device_release;
	ret = omega_device_register(&omega_dev[index]);
	index++;
	if(ret < 0) {
		omega_pr_err("cannot register omega device.[%d]\n", index - 1);
		goto err3;
	}
	// }
	return 0;
err3 :
	omega_device_unregister(&omega_dev[1]);
err2 :
	omega_device_unregister(&omega_dev[0]);
err1 :
	return -EIO;
}

static void
omega_release_all_device(void)
{
	omega_device_unregister(&omega_dev[0]);
	omega_device_unregister(&omega_dev[1]);
	omega_device_unregister(&omega_dev[2]);
}

__init int
omega_init(void)
{
	int			ret = 0;
	dev_set_name(&omega_bus_device, "omega");
	// /sys/devices/omegaが作成される
	ret = device_register(&omega_bus_device);
	if(ret < 0) {
		omega_pr_err("cannot register omega bus device.\n");
		goto err1;
	}
	ret = bus_register(&omega_bus_type);
	if(ret < 0) {
		omega_pr_err("cannot register bus.\n");
		goto err2;
	}
	ret = omega_bus_scan();
	if(ret < 0) {
		omega_pr_err("omega scan error.\n");
		goto err3;
	}

	return ret;
err3 :
	bus_unregister(&omega_bus_type);
err2 :
	device_unregister(&omega_bus_device);
err1 :
	return ret;
}

__exit void
omega_exit(void)
{
	omega_release_all_device();
	bus_unregister(&omega_bus_type);
	device_unregister(&omega_bus_device);
}

module_init(omega_init);
module_exit(omega_exit);
MODULE_LICENSE("GPL");
