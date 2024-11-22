#include "packager/media/formats/ass/ass_passthrough_parser.h"

#include <absl/log/log.h>
#include <absl/log/check.h>
#include <packager/macros/status.h>
#include <packager/file.h>
#include <packager/media/formats/webvtt/webvtt_parser.h>
#include <absl/strings/numbers.h>
#include <absl/strings/str_format.h>
#include <absl/strings/str_split.h>

#include <packager/kv_pairs/kv_pairs.h>
#include <packager/media/base/text_stream_info.h>
#include <packager/media/formats/webvtt/webvtt_utils.h>
#include <packager/utils/string_trim_split.h>
#include <packager/media/base/text_sample.h>

namespace shaka {
namespace media {

AssPassthroughParser::AssPassthroughParser() {}

const uint64_t kStreamIndex = 0;

void AssPassthroughParser::Init(const InitCB& init_cb,
                                const NewMediaSampleCB& new_media_sample_cb,
                                const NewTextSampleCB& new_text_sample_cb,
                                KeySource* decryption_key_source) {
  DCHECK(init_cb_ == nullptr);
  DCHECK(init_cb != nullptr);
  DCHECK(new_text_sample_cb != nullptr);
  DCHECK(!decryption_key_source) << "Encrypted ASS not supported";

  new_text_sample_cb_ = new_text_sample_cb;
  init_cb_ = init_cb;
}

bool AssPassthroughParser::Flush() {
  if (!new_text_sample_cb_) {
    LOG(WARNING) << "new_text_sample_cb not initialized properly.";
    return false;
  }
  if (buffer_.empty()) {
    LOG(WARNING) << "No data to flush.";
    return true;
  }

  // Create a TextSample with the entire file as its body.
  std::string id = "ass_passthrough";  // Unique ID for the text sample.
  int64_t start_time = 0;              // Start time for the sample.
  int64_t end_time = 0;   // End time (unknown for passthrough, keep 0).
  TextSettings settings;  // Default text settings (empty for passthrough).
  TextFragmentStyle default_style; // Use default TextFragmentStyle and buffer content as the body.
  TextFragment body(default_style, std::string(buffer_.begin(), buffer_.end()));
  auto sample = std::make_shared<TextSample>(id, start_time, end_time, settings, body);

  auto success = new_text_sample_cb_(kStreamIndex, sample);
  if (!success) {
    LOG(ERROR) << "Failed to deliver the text sample.";
    return false;
  }

  new_text_sample_cb_(3, sample);

  //buffer_.clear();
  return true;
}

bool AssPassthroughParser::Parse(const uint8_t* buf, int size) {
  if (!buf || size <= 0) {
    LOG(ERROR) << "Invalid buffer data.";
    return true;
  }

  // Append the new data to the internal buffer.
  buffer_.insert(buffer_.end(), buf, buf + size);
  if (!stream_info_dispatched_) {  
    DispatchTextStreamInfo();
  }

  return true;
}

void AssPassthroughParser::DispatchTextStreamInfo() {
  stream_info_dispatched_ = true;

  const int kTrackId = 0;
  // The resolution of timings are in milliseconds.
  const int kTimescale = 1000;
  // The duration passed here is not very important. Also the whole file
  // must be read before determining the real duration which doesn't
  // work nicely with the current demuxer.
  const int kDuration = 0;
  const char kWebCodecString[] = "ass";
  const int64_t kNoWidth = 0;
  const int64_t kNoHeight = 0;
  // The language of the stream will be overwritten by the Demuxer later.
  const char kNoLanguage[] = "";

  const auto stream = std::make_shared<TextStreamInfo>(
      kTrackId, kTimescale, kDuration, kCodecAss, kWebCodecString, "",
      kNoWidth, kNoHeight, kNoLanguage);
  stream->set_css_styles(css_styles_);
  //for (const auto& pair : regions_)
  //  stream->AddRegion(pair.first, pair.second);

  std::vector<std::shared_ptr<StreamInfo>> streams{stream};
  init_cb_(streams);
}

}  // namespace media
}  // namespace shaka