#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0x2b22e0c3, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0x74a2800d, __VMLINUX_SYMBOL_STR(usb_deregister) },
	{ 0xdc62d666, __VMLINUX_SYMBOL_STR(usb_register_driver) },
	{ 0x13e4de3, __VMLINUX_SYMBOL_STR(usb_deregister_dev) },
	{ 0xbdfc1512, __VMLINUX_SYMBOL_STR(usb_kill_urb) },
	{ 0x8028e5a5, __VMLINUX_SYMBOL_STR(_dev_info) },
	{ 0xd2b09ce5, __VMLINUX_SYMBOL_STR(__kmalloc) },
	{ 0x7c8d8f78, __VMLINUX_SYMBOL_STR(usb_register_dev) },
	{ 0xace1a41b, __VMLINUX_SYMBOL_STR(dev_set_drvdata) },
	{ 0xe243d198, __VMLINUX_SYMBOL_STR(usb_get_dev) },
	{ 0xf432dd3d, __VMLINUX_SYMBOL_STR(__init_waitqueue_head) },
	{ 0xb153f6c5, __VMLINUX_SYMBOL_STR(kmem_cache_alloc_trace) },
	{ 0x2b056eed, __VMLINUX_SYMBOL_STR(kmalloc_caches) },
	{ 0x69117535, __VMLINUX_SYMBOL_STR(usb_alloc_coherent) },
	{ 0xd2e5c298, __VMLINUX_SYMBOL_STR(usb_alloc_urb) },
	{ 0x4f6b400b, __VMLINUX_SYMBOL_STR(_copy_from_user) },
	{ 0xf22449ae, __VMLINUX_SYMBOL_STR(down_interruptible) },
	{ 0x16305289, __VMLINUX_SYMBOL_STR(warn_slowpath_null) },
	{ 0xdc0accc3, __VMLINUX_SYMBOL_STR(dev_get_drvdata) },
	{ 0x82764ccb, __VMLINUX_SYMBOL_STR(usb_find_interface) },
	{ 0xfa66f77c, __VMLINUX_SYMBOL_STR(finish_wait) },
	{ 0x34f22f94, __VMLINUX_SYMBOL_STR(prepare_to_wait_event) },
	{ 0x1000e51, __VMLINUX_SYMBOL_STR(schedule) },
	{ 0xe39fb986, __VMLINUX_SYMBOL_STR(usb_bulk_msg) },
	{ 0x68aca4ad, __VMLINUX_SYMBOL_STR(down) },
	{ 0x65a5675c, __VMLINUX_SYMBOL_STR(current_task) },
	{ 0x776835a9, __VMLINUX_SYMBOL_STR(usb_free_coherent) },
	{ 0x4f8b5ddb, __VMLINUX_SYMBOL_STR(_copy_to_user) },
	{ 0xcf21d241, __VMLINUX_SYMBOL_STR(__wake_up) },
	{ 0x71e3cecb, __VMLINUX_SYMBOL_STR(up) },
	{ 0x69acdf38, __VMLINUX_SYMBOL_STR(memcpy) },
	{ 0xf147ecb1, __VMLINUX_SYMBOL_STR(down_trylock) },
	{ 0xd97dcdf2, __VMLINUX_SYMBOL_STR(usb_submit_urb) },
	{ 0xfab0a6d3, __VMLINUX_SYMBOL_STR(usb_free_urb) },
	{ 0x37a0cba, __VMLINUX_SYMBOL_STR(kfree) },
	{ 0x873fd4d1, __VMLINUX_SYMBOL_STR(usb_put_dev) },
	{ 0xbdfb6dbb, __VMLINUX_SYMBOL_STR(__fentry__) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

MODULE_ALIAS("usb:v08DApFC00d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v0FF8p0001d*dc*dsc*dp*ic*isc*ip*in*");

MODULE_INFO(srcversion, "40D1D4403ED84E29751CB96");
