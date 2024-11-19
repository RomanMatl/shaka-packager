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
  if (!new_text_sample_cb_ || buffer_.empty()) {
    LOG(WARNING) << "No data to flush.";
    return false;
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

  buffer_.clear();
  return true;
}

bool AssPassthroughParser::Parse(const uint8_t* buf, int size) {
  if (!buf || size <= 0) {
    LOG(ERROR) << "Invalid buffer data.";
    return false;
  }

  // Append the new data to the internal buffer.
  buffer_.insert(buffer_.end(), buf, buf + size);
  return true;
}

}  // namespace media
}  // namespace shaka