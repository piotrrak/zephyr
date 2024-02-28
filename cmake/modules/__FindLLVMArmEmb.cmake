# SPDX-License-Identifier: Apache-2.0
#
# Copyright (c) 2024, Piotr Rak

# FindLLVMArmEmb module for supporting module search of LLVM Embedded Toolchain for arm
#
# TODO: document

# Set internal variables if set in environment.
zephyr_get(ZEPHYR_TOOLCHAIN_VARIANT)

zephyr_get(LLVMARMEMB_TOOLCHAIN_PATH)
zephyr_get(LLVMARMEMB_TOOLCHAIN_ROOT)

message(FindLLVMArmEmb)

if("llvmarmemb" STREQUAL "${ZEPHYR_TOOLCHAIN_VARIANT}")

  # This ensure packages are sorted in descending order.
  set(CMAKE_FIND_PACKAGE_SORT_DIRECTION_CURRENT ${CMAKE_FIND_PACKAGE_SORT_DIRECTION})
  set(CMAKE_FIND_PACKAGE_SORT_ORDER_CURRENT ${CMAKE_FIND_PACKAGE_SORT_ORDER})
  set(CMAKE_FIND_PACKAGE_SORT_DIRECTION DEC)
  set(CMAKE_FIND_PACKAGE_SORT_ORDER NATURAL)

  if(DEFINED LLVMARMEMB_TOOLCHAIN_PATH)
  else()
    # Search same paths we're looking for Zephyr SDK versions
    #
    # For the sake of the user's convinience and ease of documenting it's behaviour
    # please do keep it in sync with `FindZephyr-sdk.cmake` `zephyr_sdk_search_paths`
    set(llvmarmemb_search_paths
        /usr
        /usr/local
        /opt
        $ENV{HOME}
        $ENV{HOME}/.local
        $ENV{HOME}/.local/opt
        $ENV{HOME}/bin)

    find_path(LLVMEmbeddedToolchainForArm 0.0.0 EXACT \
      PATHS ${llvmarmemb_search_paths} NO_DEFAULT_PATH \
    )
  endif()

endif()

unset(llvmarmemb_search_paths)

if(DEFINED LLVMARMEMB_TOOLCHAIN_PATH)
  set(LLVMARMEMB_TOOLCHAIN_PATH ${LLVMARMEMB_TOOLCHAIN_PATH} CACHE PATH "LLVM Embedded Toolchain for
Arm install directory")
endif()

