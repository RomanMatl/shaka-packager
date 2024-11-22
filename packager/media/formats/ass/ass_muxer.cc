// Copyright 2020 Google LLC. All rights reserved.
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include <packager/media/formats/ass/ass_muxer.h>

#include <packager/file.h>
#include <packager/macros/status.h>

namespace shaka {
namespace media {
namespace ass {

AssMuxer::AssMuxer(const MuxerOptions& options) : TextMuxer(options) {}
AssMuxer::~AssMuxer() {}

Status AssMuxer::InitializeStream(TextStreamInfo* stream) {
  stream->set_codec(kCodecAss);
  stream->set_codec_string("ass");
  generator_.Initialize(stream->regions(), stream->language(),
                        stream->time_scale());
  //output_file_name_ = stream->output_file_name;
  return Status::OK;
}

Status AssMuxer::AddTextSampleInternal(const TextSample& sample) {
  AssMuxer::whole_file = sample.body().body;
  //generator_.AddSample(sample);
  return Status::OK;
}

Status AssMuxer::WriteToFile(const std::string& filename, uint64_t* size) {
  * size = whole_file.size();
  if (!File::WriteStringToFile(filename.c_str(), whole_file))
    return Status(error::FILE_FAILURE, "Failed to write " + filename);
  return Status::OK;
}

}  // namespace ass
}  // namespace media
}  // namespace shaka
