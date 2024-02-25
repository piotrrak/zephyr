# Copyright (c) Piotr Rak
# SPDX-License-Identifier: Apache-2.0

zephyr_get(LLVMARMEMB_TOOLCHAIN_PATH)
assert(LLVMARMEMB_TOOLCHAIN_PATH "LLVMARMEMB_TOOLCHAIN_PATH is not set")

if(EXISTS ${LLVMARMEMB_TOOLCHAIN_PATH})
  set(TOOLCHAIN_HOME ${LLVMARMEMB_TOOLCHAIN_PATH}/bin)
else()
  message(FATAL_ERROR "Nothing found at LLVMARMEMB_TOOLCHAIN_PATH: '${LLVMARMEMB_TOOLCHAIN_PATH}'")
endif()

set(COMPILER clang)
set(LINKER lld)
set(BINTOOLS llvm)

set(TOOLCHAIN_HAS_NEWLIB ON CACHE BOOL "True if toolchain supports newlib")
