SBN_CLIENT_SRC := ./fsw/src
SBN_CLIENT_INC := -I./fsw/public_inc
cFS_ROOT := ../cfs
SBN_INC := -I$(cFS_ROOT)/apps/sbn/fsw/platform_inc -I$(cFS_ROOT)/apps/sbn/fsw/src

# cfe_platform_cfg.h is build specific, and comes from the defs folder.
# Link the platform cfg you want (such as x86-64_platform_cfg.h) as cfe_defs/cfe_platform_cfg.h
# The same goes for cfe_defs/cfe_mission_cfg.h, cfe_defs/cfe_msgids.h, cfe_defs/cfe_perfids.h
#CFE_DEFS = -I./cfe_defs

CPU1_ROOT := $(cFS_ROOT)/build/native/default_cpu1
CONFIG_INC := -I$(CPU1_ROOT)/inc
BUILD_INC := -I$(cFS_ROOT)/build/inc
OSAL_INC_OSCONFIG := -I$(CPU1_ROOT)/osal/inc

CFE_INC := -I$(cFS_ROOT)/cfe/modules/core_api/fsw/inc
OSAL_INC := -I$(cFS_ROOT)/osal/src/os/inc
MSG_INC = -I$(cFS_ROOT)/cfe/modules/msg/fsw/inc
PSP_INC := -I$(cFS_ROOT)/psp/fsw/inc

ALL_INC := $(CONFIG_INC) $(OSAL_INC_OSCONFIG) $(BUILD_INC) $(SBN_CLIENT_INC) $(SBN_INC) $(CFE_INC) $(OSAL_INC) $(MSG_INC) $(PSP_INC)

LIBS := -lpthread

# SC_OBJS := sbn_client.a
# SC_OBJS += sbn_client_ingest.a
# SC_OBJS += sbn_client_init.a
# SC_OBJS += sbn_client_minders.a
# SC_OBJS += sbn_client_utils.a
# SC_OBJS += sbn_client_wrappers.a

A_FILES := $(patsubst %.c,%.a,$(wildcard $(SBN_CLIENT_SRC)/*.c))

all: libsbn_client.so

libsbn_client.so: $(A_FILES)
	gcc -shared $^ -o libsbn_client.so

%.a : %.c
	gcc -Wall -Werror -c -fPIC $< $(ALL_INC) $(LIBS) -o $@
	objcopy --redefine-syms=unwrap_symbols.txt $@

clean:
	rm -f $(A_FILES)
	rm -f libsbn_client.so
