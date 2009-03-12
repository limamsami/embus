include $(CONFDIR)/$(ARCH).mk

CINCLUDE := $(strip $(INCDIR))

O_TARGET := built-in.o

ifeq ($(DEBUG), 1)
	CDEFS += -D__EMBUS_DEBUG__
	CFLAGS += -g
endif

obj-y := $(sort $(strip $(obj-y)))
subdir-y := $(dir $(filter-out %.o, $(obj-y)))

obj-y := $(filter %.o, $(obj-y))
o_target-subdir-y := $(patsubst %/, %/$(O_TARGET), $(subdir-y))

src-y := $(patsubst %.o, %.c, $(obj-y))
dep-y := $(patsubst %.o, %.d, $(obj-y))

.PHONY: mkdep complie lib link clean go_subdir

%.o : %.c
	$(cmd_cc_c_o)

%.d : %.c
	$(cmd_mk_dep)

go_subdir:
	$(cmd_go_subdir)

mkdep: go_subdir $(dep-y)

ifeq ($(IS_O_TARGET), 1)
complie: go_subdir $(obj-y) $(O_TARGET)

$(O_TARGET): $(obj-y) $(o_target-subdir-y)
	$(cmd_ld_o_target)
else
complie: go_subdir $(obj-y)
endif

lib: go_subdir $(TARGET_LIB)

link: go_subdir $(TARGET_EXEC)

clean: go_subdir
	rm -rf $(obj-y) $(dep-y) $(O_TARGET) $(TARGET_EXEC) $(TARGET_LIB)
	$(cmd_clear_lib)

-include $(dep-y)

define cmd_cc_c_o
	$(CC) $(CFLAGS) $(CWARN) $(CDEFS) $(CINCLUDE) -c -o $@ $<
endef

define cmd_cc_exec
	$(CC) $(CFLAGS) $(CWARN) $(CDEFS) $(CINCLUDE) $(LIBDIR) $(ALL_LIB) -o $@ $^
endef

ifneq ($(DEBUG),1)
define cmd_strip_exec
	$(STRIP) $@
endef
endif

define cmd_ld_o_target
	$(LD) $(LDFLAGS) -r -o $(O_TARGET) $(obj-y) $(o_target-subdir-y)
endef

define cmd_cc_o_lib
	$(CC) $(CFLAGS) $(CWAN) $(CDEFS) $(CINCLUDE) $(LIBDIR) $(LIB) --shared -fPIC -o $@ $^
endef

define cmd_setup_lib
	@ln -sf $(shell pwd)/$@ $(TOPDIR)/$@
endef

define cmd_clear_lib
	@for i in $(TARGET_LIB); do	\
		rm -rf $(TOPDIR)/$$i;	\
	done
endef

define cmd_mk_dep
	@set -e; rm -rf $@;	\
	$(CC) -MM $(CFLAGS) $(CWARN) $(CDEFS) $(CINCLUDE) $< > $@;	\
	sed "s#\($*\)\.o[ :]*#\1.o $@ : #g" -i $@
endef

define cmd_go_subdir
	@for i in $(subdir-y); do	\
		make $(MAKECMDGOALS) -C $$i;	\
	done
endef
