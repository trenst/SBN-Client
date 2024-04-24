cmake_minimum_required(VERSION 3.11)
project(CFS_SBN_CLIENT C)


set(cFS_ROOT /home/sdev/src/cfs)
set(sbn_MISSION_DIR ${cFS_ROOT}/apps/sbn)
set(SBN_INC ${sbn_MISSION_DIR}/fsw/src ${sbn_MISSION_DIR}/fsw/platform_inc)
set(SBN_CLIENT_INC fsw/public_inc)

set(CPU1_ROOT ${cFS_ROOT}/build/native/default_cpu1)
set(CONFIG_INC ${CPU1_ROOT}/inc)
set(BUILD_INC ${cFS_ROOT}/build/inc)
set(OSAL_INC_OSCONFIG ${CPU1_ROOT}/osal/inc)
set(CFE_INC ${cFS_ROOT}/cfe/modules/core_api/fsw/inc)
set(OSAL_INC ${cFS_ROOT}/osal/src/os/inc)
set(MSG_INC ${cFS_ROOT}/cfe/modules/msg/fsw/inc)
set(PSP_INC ${cFS_ROOT}/psp/fsw/inc)

set(ALL_INC ${CONFIG_INC} ${OSAL_INC_OSCONFIG} ${BUILD_INC} ${SBN_CLIENT_INC} ${SBN_INC} ${CFE_INC} ${OSAL_INC} ${MSG_INC} ${PSP_INC})


aux_source_directory(fsw/src APP_SRC_FILES)


add_library(sbn_client "SHARED" ${APP_SRC_FILES})

target_include_directories(sbn_client PUBLIC ${ALL_INC})

# Override the default behavior of attaching a "lib" prefix
#set_target_properties(${APP_NAME} PROPERTIES PREFIX "")
# SBN?

# Create the install targets for this shared/modular app
# foreach(TGT ${APP_INSTALL_LIST})
#   install(TARGETS ${APP_NAME} DESTINATION ${TGT}/${INSTALL_SUBDIR})
# endforeach()

# if (ENABLE_UNIT_TESTS)
#add_subdirectory(unit-test)
# endif (ENABLE_UNIT_TESTS)
