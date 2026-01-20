#
# Copyright (c) 2026
#
# SPDX-License-Identifier: Apache-2.0
#
# Generated using zcbor version 0.9.1
# https://github.com/NordicSemiconductor/zcbor
# Generated with a --default-max-qty of 3
#

add_library(streaming_chunks)
target_sources(streaming_chunks PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/../../src/zcbor_decode.c
    ${CMAKE_CURRENT_LIST_DIR}/../../src/zcbor_encode.c
    ${CMAKE_CURRENT_LIST_DIR}/../../src/zcbor_common.c
    ${CMAKE_CURRENT_LIST_DIR}/../../src/zcbor_print.c
    ${CMAKE_CURRENT_LIST_DIR}/src/streaming_chunks_decode.c
    ${CMAKE_CURRENT_LIST_DIR}/src/streaming_chunks_encode.c
    )
target_include_directories(streaming_chunks PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/../../include
    ${CMAKE_CURRENT_LIST_DIR}/include
    )
