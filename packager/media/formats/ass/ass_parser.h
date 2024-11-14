// Copyright 2017 Google LLC. All rights reserved.
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#ifndef PACKAGER_MEDIA_FORMATS_ASS_ASS_PARSER_H_
#define PACKAGER_MEDIA_FORMATS_ASS_ASS_PARSER_H_

#include <map>
#include <string>
#include <vector>

#include <packager/media/base/media_parser.h>
#include <packager/media/base/text_sample.h>
#include <packager/media/base/text_stream_info.h>

namespace shaka {
namespace media {

// Used to parse a WebVTT source into Cues that will be sent downstream.
class AssParser : public MediaParser {
 public:
  AssParser();

  void Init(const InitCB& init_cb,
            const NewMediaSampleCB& new_media_sample_cb,
            const NewTextSampleCB& new_text_sample_cb,
            KeySource* decryption_key_source) override;
  bool Flush() override;
  bool Parse(const uint8_t* buf, int size) override;
};

}  // namespace media
}  // namespace shaka
#endif  // PACKAGER_MEDIA_FORMATS_WEBVTT_WEBVTT_MUXER_H_