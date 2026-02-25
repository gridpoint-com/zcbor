#
# Copyright (c) 2025 Nordic Semiconductor ASA
#
# SPDX-License-Identifier: Apache-2.0
#

cmake_minimum_required(VERSION 3.13.1)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(test6_iter_beyond_max_qty)
include(../../cmake/test_template.cmake)

set(py_command
  zcbor
  code
  -c ${CMAKE_CURRENT_LIST_DIR}/../../cases/iter_beyond_max_qty.cddl
  --output-cmake ${PROJECT_BINARY_DIR}/iter_beyond_max_qty.cmake
  -t collection_t
  -e
  ${bit_arg}
  --short-names
  --stream-encode
  --default-max-qty 3
  )

execute_process(
  COMMAND ${py_command}
  COMMAND_ERROR_IS_FATAL ANY
  )

include(${PROJECT_BINARY_DIR}/iter_beyond_max_qty.cmake)

target_link_libraries(iter_beyond_max_qty PRIVATE zephyr_interface)
target_link_libraries(app PRIVATE iter_beyond_max_qty)
