# CMake toolchain file for cross-compiling Io to WASM via wasi-sdk.
#
# Usage:
#   cmake .. -DCMAKE_TOOLCHAIN_FILE=../cmake/wasi-sdk.cmake \
#            -DIO2C_EXECUTABLE=../build-native/_build/binaries/io2c

set(CMAKE_SYSTEM_NAME WASI)
set(CMAKE_SYSTEM_PROCESSOR wasm32)

# Find wasi-sdk — check environment variable, then common install paths
if(DEFINED ENV{WASI_SDK_PREFIX})
	set(WASI_SDK_PREFIX "$ENV{WASI_SDK_PREFIX}" CACHE PATH "wasi-sdk install path")
elseif(EXISTS "/opt/wasi-sdk")
	set(WASI_SDK_PREFIX "/opt/wasi-sdk" CACHE PATH "wasi-sdk install path")
elseif(EXISTS "$ENV{HOME}/wasi-sdk")
	set(WASI_SDK_PREFIX "$ENV{HOME}/wasi-sdk" CACHE PATH "wasi-sdk install path")
else()
	message(FATAL_ERROR "wasi-sdk not found. Set WASI_SDK_PREFIX or install to /opt/wasi-sdk")
endif()

set(CMAKE_C_COMPILER "${WASI_SDK_PREFIX}/bin/clang")
set(CMAKE_CXX_COMPILER "${WASI_SDK_PREFIX}/bin/clang++")
set(CMAKE_AR "${WASI_SDK_PREFIX}/bin/llvm-ar")
set(CMAKE_RANLIB "${WASI_SDK_PREFIX}/bin/llvm-ranlib")
set(CMAKE_SYSROOT "${WASI_SDK_PREFIX}/share/wasi-sysroot")

# WASM-specific defines
add_definitions(-D__wasi__ -DUSE_BUILTIN_NAN)

# No shared libraries in WASM
set(BUILD_SHARED_LIBS OFF)

# No threads in WASI (yet)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-exceptions")

# WASI emulation libraries for clock(), signal(), etc.
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_WASI_EMULATED_PROCESS_CLOCKS -D_WASI_EMULATED_SIGNAL")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lwasi-emulated-process-clocks -lwasi-emulated-signal")

# Ensure static linking
set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
