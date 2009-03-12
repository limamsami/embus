obj-y += send.o
obj-y += recv.o
obj-y += send_reply.o
obj-y += recv_reply.o

TARGET_EXEC = send recv send_reply recv_reply

send : send.o
	$(cmd_cc_exec)
	$(cmd_strip_exec)

recv : recv.o
	$(cmd_cc_exec)
	$(cmd_strip_exec)

send_reply : send_reply.o
	$(cmd_cc_exec)
	$(cmd_strip_exec)

recv_reply : recv_reply.o
	$(cmd_cc_exec)
	$(cmd_strip_exec)

include $(CONFDIR)/rules.mk
