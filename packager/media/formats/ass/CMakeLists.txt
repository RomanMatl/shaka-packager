# Copyright 2020 Google LLC. All rights reserved.
#
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file or at
# https://developers.google.com/open-source/licenses/bsd

add_library(ass STATIC
  ass_generator.cc
  ass_generator.h
  ass_muxer.cc
  ass_muxer.h
  ass_parser.cc
  ass_parser.h
  ass_passthrough_parser.cc
  ass_passthrough_parser.h
  ass_to_mp4_handler.cc
  ass_to_mp4_handler.h
  )
target_link_libraries(ass
  media_base
  mpd_builder
)

add_executable(ass_unittest
  ass_generator_unittest.cc
  )
target_link_libraries(ass_unittest
  file
  file_test_util
  test_data_util
  absl::flags
  media_event
  ass
  gmock
  gtest
  gtest_main
  )
add_test(NAME dvb_unittest COMMAND dvb_unittest)