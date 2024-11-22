// Copyright 2020 Google LLC. All rights reserved.
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#ifndef PACKAGER_MEDIA_FORMATS_ASS_ASS_MUXER_H_
#define PACKAGER_MEDIA_FORMATS_ASS_ASS_MUXER_H_

#include <packager/media/base/text_muxer.h>
#include <packager/media/formats/ass/ass_generator.h>

namespace shaka {
namespace media {
namespace ass {

class AssMuxer : public TextMuxer {
 public:
  explicit AssMuxer(const MuxerOptions& options);
  ~AssMuxer() override;

 private:
  Status InitializeStream(TextStreamInfo* stream) override;
  Status AddTextSampleInternal(const TextSample& sample) override;
  Status WriteToFile(const std::string& filename, uint64_t* size) override;

  AssGenerator generator_;
  // Buffers the entire file data.
  std::string whole_file;
  std::string output_file_name_;
};

}  // namespace ass
}  // namespace media
}  // namespace shaka

#endif  // PACKAGER_MEDIA_FORMATS_ASS_ASS_MUXER_H_
