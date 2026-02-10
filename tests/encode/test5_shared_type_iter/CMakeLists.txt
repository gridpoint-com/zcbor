#
# Copyright (c) 2025 Nordic Semiconductor ASA
#
# SPDX-License-Identifier: Apache-2.0
#

cmake_minimum_required(VERSION 3.13.1)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(test5_shared_type_iter)
include(../../cmake/test_template.cmake)

set(py_command
  zcbor
  code
  -c ${CMAKE_CURRENT_LIST_DIR}/../../cases/shared_iter.cddl
  --output-cmake ${PROJECT_BINARY_DIR}/shared_iter.cmake
  -t container_t
  -e
  -d
  ${bit_arg}
  --short-names
  --stream-encode
  )

execute_process(
  COMMAND ${py_command}
  COMMAND_ERROR_IS_FATAL ANY
  )

include(${PROJECT_BINARY_DIR}/shared_iter.cmake)

target_link_libraries(shared_iter PRIVATE zephyr_interface)
target_link_libraries(app PRIVATE shared_iter)
