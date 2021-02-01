#! /usr/bin/env bash
# SPDX-License-Identifier: zlib-acknowledgement

# cpu has different execution modes? when transitioning to kernel, can access all memory addresses and registers
# *BSDs are packaged as kernel, user space combined

# TODO(Ryan): Investigate naming conventions of kernel and the compatibilty,
# e.g, would releasing for 5.0 be sufficient or have to specify 5.8, etc.

[[ ! -d 5.8-build ]] && mkdir 5.8-build

g++ -g 5.8-lkp.cpp -o 5.8-build/5.8-lkp
