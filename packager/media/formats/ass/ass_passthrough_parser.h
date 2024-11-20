// Copyright 2024
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#ifndef PACKAGER_MEDIA_FORMATS_ASS_ASS_PASSTHROUGH_PARSER_H_
#define PACKAGER_MEDIA_FORMATS_ASS_ASS_PASSTHROUGH_PARSER_H_

#include <map>

#include <packager/media/base/media_parser.h>

#include <packager/media/base/text_sample.h>
#include <packager/media/base/text_stream_info.h>
#include <packager/media/formats/webvtt/text_readers.h>

namespace shaka {
namespace media {

// A simple ASS subtitle passthrough parser that forwards the entire file
// downstream.
class AssPassthroughParser : public MediaParser {
 public:
  AssPassthroughParser();

  void Init(const InitCB& init_cb,
            const NewMediaSampleCB& new_media_sample_cb,
            const NewTextSampleCB& new_text_sample_cb,
            KeySource* decryption_key_source) override;
  bool Flush() override;
  bool Parse(const uint8_t* buf, int size) override;

 private:
  // Buffers the entire file data.
  std::vector<uint8_t> buffer_;

  InitCB init_cb_;

  // Callback for passing the entire file downstream.
  NewTextSampleCB new_text_sample_cb_;
  void DispatchTextStreamInfo();
  bool stream_info_dispatched_ = false;
  std::string css_styles_ = "";
  std::map<std::string, TextRegion> regions_;
};

}  // namespace media
}  // namespace shaka

#endif  // PACKAGER_MEDIA_FORMATS_ASS_ASS_PASSTHROUGH_PARSER_H_