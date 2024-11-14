
#include <packager/media/formats/ass/ass_parser.h>

#include <packager/file.h>
#include <packager/macros/status.h>

#include <packager/media/formats/webvtt/webvtt_parser.h>

#include <absl/log/check.h>
#include <absl/log/log.h>
#include <absl/strings/numbers.h>
#include <absl/strings/str_format.h>
#include <absl/strings/str_split.h>

#include <packager/kv_pairs/kv_pairs.h>
#include <packager/media/base/text_stream_info.h>
#include <packager/media/formats/webvtt/webvtt_utils.h>
#include <packager/utils/string_trim_split.h>

namespace shaka {
namespace media {

AssParser::AssParser() {}

void AssParser::Init(const InitCB& init_cb,
                     const NewMediaSampleCB& new_media_sample_cb,
                     const NewTextSampleCB& new_text_sample_cb,
                     KeySource* decryption_key_source) {
  /* DCHECK(init_cb_ == nullptr);
  DCHECK(init_cb != nullptr);
  DCHECK(new_text_sample_cb != nullptr);
  DCHECK(!decryption_key_source) << "Encrypted Ass not supported";

  init_cb_ = init_cb;
  new_text_sample_cb_ = new_text_sample_cb;*/
}

bool AssParser::Flush() {
  /* reader_.Flush();
  return Parse();*/
  return false;
}

bool AssParser::Parse(const uint8_t* buf, int size) {
  /* reader_.PushData(buf, size);
  return Parse();*/
  return false;
}

}  // namespace media
}  // namespace shaka
