#include <linux/module.h>
#include <linux/export-internal.h>
#include <linux/compiler.h>

MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};



static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0xeb081dc5, "i2c_register_driver" },
	{ 0x9f9a8a99, "misc_deregister" },
	{ 0x7eb87600, "devm_kmalloc" },
	{ 0xcefb0c9f, "__mutex_init" },
	{ 0x3f4c09e, "misc_register" },
	{ 0x1a283f39, "_dev_info" },
	{ 0xf810f451, "_dev_err" },
	{ 0x5b08cec9, "i2c_del_driver" },
	{ 0x4dfa8d4b, "mutex_lock" },
	{ 0x61f9c02a, "i2c_smbus_write_byte" },
	{ 0xf9a482f9, "msleep" },
	{ 0xfd84e1d5, "i2c_transfer_buffer_flags" },
	{ 0x656e4a6e, "snprintf" },
	{ 0x3213f038, "mutex_unlock" },
	{ 0x6cbbfc54, "__arch_copy_to_user" },
	{ 0xf0fdf6cb, "__stack_chk_fail" },
	{ 0x39ff040a, "module_layout" },
};

MODULE_INFO(depends, "");

MODULE_ALIAS("of:N*T*Cbh,bh1750");
MODULE_ALIAS("of:N*T*Cbh,bh1750C*");
MODULE_ALIAS("i2c:bh1750");

MODULE_INFO(srcversion, "1617DA2E1DDF5D98143BB47");
