LIB =	server

SRCS =				\
	common			\
	componentbridge		\
	components		\
	entity_app		\
	forward_messagebuffer	\
	globaldata_client	\
	globaldata_server	\
	idallocate		\
	pendingLoginmgr		\
	script_timers		\
	serverapp		\
	serverconfig		\
	serverinfos		\
	signal_handler		

ifndef KBE_ROOT
export KBE_ROOT := $(subst /kbe/src/lib/$(LIB),,$(CURDIR))
endif

USE_PYTHON=1
include $(KBE_ROOT)/kbe/src/build/common.mak