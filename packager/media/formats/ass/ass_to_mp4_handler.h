// Copyright 2020 Google LLC. All rights reserved.
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#ifndef PACKAGER_MEDIA_FORMATS_ASS_ASS_TO_MP4_HANDLER_H_
#define PACKAGER_MEDIA_FORMATS_ASS_ASS_TO_MP4_HANDLER_H_

#include <memory>

#include <packager/media/base/media_handler.h>
#include <packager/media/formats/ass/ass_generator.h>

namespace shaka {
namespace media {
namespace ass {

// A media handler that should come after the cue aligner and segmenter and
// should come before the muxer. This handler is to convert text samples
// to media samples so that they can be sent to a mp4 muxer.
class assToMp4Handler : public MediaHandler {
 public:
  assToMp4Handler() = default;
  ~assToMp4Handler() override = default;

 private:
  Status InitializeInternal() override;
  Status Process(std::unique_ptr<StreamData> stream_data) override;

  Status OnStreamInfo(std::unique_ptr<StreamData> stream_data);
  Status OnCueEvent(std::unique_ptr<StreamData> stream_data);
  Status OnSegmentInfo(std::unique_ptr<StreamData> stream_data);
  Status OnTextSample(std::unique_ptr<StreamData> stream_data);

  AssGenerator generator_;
};

}  // namespace ass
}  // namespace media
}  // namespace shaka

#endif  // PACKAGER_MEDIA_FORMATS_ASS_ASS_TO_MP4_HANDLER_H_
