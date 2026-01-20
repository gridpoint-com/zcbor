# Copyright (c) 2026
#
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.13.3)
project(sample_streaming_chunks)

# Regenerate zcbor-generated files when requested.
if (REGENERATE_ZCBOR)
  set(zcbor_command
    zcbor code
    --decode --encode
    --stream-encode
    --stream-decode
    -c ${CMAKE_CURRENT_LIST_DIR}/streaming_chunks.cddl
    -t StreamItem
    --output-cmake ${CMAKE_CURRENT_LIST_DIR}/streaming_chunks.cmake
    --file-header "Copyright (c) 2026\n\nSPDX-License-Identifier: Apache-2.0"
    )
  execute_process(COMMAND ${zcbor_command})
endif()

include(${CMAKE_CURRENT_LIST_DIR}/streaming_chunks.cmake)

add_executable(app src/main.c)

# Link the library from streaming_chunks.cmake
target_link_libraries(app PRIVATE streaming_chunks)
