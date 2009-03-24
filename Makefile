ARCH	:=	arm
DEBUG	:=	1

export ARCH DEBUG

TOPDIR	:=	$(shell pwd)
CONFDIR :=	$(TOPDIR)/config
INCDIR	:=	-I$(TOPDIR)/include
SRCDIR	:=	$(TOPDIR)/src
LIBDIR	:=	-L$(TOPDIR)

export TOPDIR CONFDIR INCDIR LIBDIR 

LIB	:=	
EMBUS_LIB:=	-lembus
ALL_LIB	:=	$(LIB) $(EMBUS_LIB)

export LIB EBUS_LIB ALL_LIB

.PHONY: all clean mkdep complie lib link

all: mkdep complie lib link

mkdep:
	@echo
	@echo =============================================
	@echo "  ARCH=$(ARCH)   Making source depends..."
	@echo =============================================
	@echo
	$(cmd_tranverse_src_dir)

complie:
	@echo
	@echo =============================================
	@echo "  ARCH=$(ARCH)   Compling source..."
	@echo =============================================
	@echo
	$(cmd_tranverse_src_dir)

lib:
	@echo
	@echo =============================================
	@echo "  ARCH=$(ARCH)   Making Libraries..."
	@echo =============================================
	@echo
	$(cmd_tranverse_src_dir)

link:	
	@echo
	@echo =============================================
	@echo "  ARCH=$(ARCH)   Linking objects..."
	@echo =============================================
	@echo
	$(cmd_tranverse_src_dir)

clean:
	$(cmd_tranverse_src_dir)

define cmd_tranverse_src_dir
	@for i in $(SRCDIR); do		\
		make $@ -C $$i;	\
	done
endef
