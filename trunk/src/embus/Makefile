obj-y += embus.o
obj-y += embus_common.o
obj-y += embus_local.o
obj-y += embus_msg.o
obj-y += embus_debug.o
obj-y += embus_notifier.o

TARGET_LIB = libembus.so

$(TARGET_LIB) : $(obj-y)
	$(cmd_cc_o_lib)	
	$(cmd_setup_lib)

include $(CONFDIR)/rules.mk
