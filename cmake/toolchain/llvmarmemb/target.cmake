include(${ZEPHYR_BASE}/cmake/compiler/clang/flag-target.cmake)

clang_target_flag("${ARCH}" triple)

if(DEFINED triple)
  set(CMAKE_C_COMPILER_TARGET   ${triple})
  set(CMAKE_ASM_COMPILER_TARGET ${triple})
  set(CMAKE_CXX_COMPILER_TARGET ${triple})

  unset(triple)
endif()

list(APPEND TOOLCHAIN_C_FLAGS --sysroot "${LLVMARMEMB_TOOLCHAIN_PATH}/lib/clang-runtimes")
set(CMAKE_EXE_LINKER_FLAGS_INIT "-lc")
#list(APPEND TOOLCHAIN_C_FLAGS --config "${LLVMARMEMB_TOOLCHAIN_PATH}/bin/newlib.cfg")


