// Copyright 2020 Google LLC. All rights reserved.
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include <packager/media/formats/ass/ass_generator.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace shaka {
namespace media {
namespace ass {

namespace {

const int64_t kMsTimeScale = 1000;

const TextFragmentStyle kNoStyles{};
const bool kNewline = true;
const std::string kNoId = "";

TextSettings DefaultSettings() {
  TextSettings settings;
  // Override default value so ass doesn't print this setting by default.
  settings.text_alignment = TextAlignment::kStart;
  return settings;
}

struct TestProperties {
  std::string id;
  int64_t start = 5000;
  int64_t end = 6000;
  TextSettings settings = DefaultSettings();
  TextFragment body;

  std::map<std::string, TextRegion> regions;
  std::string language = "";
  int32_t time_scale = kMsTimeScale;
};

}  // namespace

class assMuxerTest : public testing::Test {
 protected:
  void ParseSingleCue(const std::string& expected_body,
                      const TestProperties& properties) {
    AssGenerator generator;
    generator.Initialize(properties.regions, properties.language,
                         properties.time_scale);
    generator.AddSample(TextSample(properties.id, properties.start,
                                   properties.end, properties.settings,
                                   properties.body));

    std::string results;
    ASSERT_TRUE(generator.Dump(&results));
    ASSERT_EQ(results, expected_body);
  }
};

TEST_F(assMuxerTest, WithOneSegmentAndWithOneSample) {
  const char* kExpectedOutput =
      "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
      "<tt xmlns=\"http://www.w3.org/ns/ass\" "
      "xmlns:tts=\"http://www.w3.org/ns/ass#styling\" xml:lang=\"\">\n"
      "  <head>\n"
      "    <metadata/>\n"
      "    <styling/>\n"
      "    <layout/>\n"
      "  </head>\n"
      "  <body>\n"
      "    <div>\n"
      "      <p xml:space=\"preserve\" begin=\"00:00:05.000\" "
      "end=\"00:00:06.000\">payload</p>\n"
      "    </div>\n"
      "  </body>\n"
      "</tt>\n";

  TestProperties properties;
  properties.body.body = "payload";
  ParseSingleCue(kExpectedOutput, properties);
}

TEST_F(assMuxerTest, MultipleFragmentsWithNewlines) {
  const char* kExpectedOutput =
      "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
      "<tt xmlns=\"http://www.w3.org/ns/ass\" "
      "xmlns:tts=\"http://www.w3.org/ns/ass#styling\" xml:lang=\"\">\n"
      "  <head>\n"
      "    <metadata/>\n"
      "    <styling/>\n"
      "    <layout/>\n"
      "  </head>\n"
      "  <body>\n"
      "    <div>\n"
      "      <p xml:space=\"preserve\" begin=\"00:00:05.000\" "
      "end=\"00:00:06.000\">foo bar<br/>baz</p>\n"
      "    </div>\n"
      "  </body>\n"
      "</tt>\n";

  TestProperties properties;
  properties.body.sub_fragments.emplace_back(kNoStyles, "foo ");
  properties.body.sub_fragments.emplace_back(kNoStyles, "bar");
  properties.body.sub_fragments.emplace_back(kNoStyles, kNewline);
  properties.body.sub_fragments.emplace_back(kNoStyles, "baz");

  ParseSingleCue(kExpectedOutput, properties);
}

TEST_F(assMuxerTest, HandlesStyles) {
  const char* kExpectedOutput =
      "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
      "<tt xmlns=\"http://www.w3.org/ns/ass\" "
      "xmlns:tts=\"http://www.w3.org/ns/ass#styling\" xml:lang=\"\">\n"
      "  <head>\n"
      "    <metadata/>\n"
      "    <styling/>\n"
      "    <layout/>\n"
      "  </head>\n"
      "  <body>\n"
      "    <div>\n"
      "      <p xml:space=\"preserve\" begin=\"00:00:05.000\" "
      "end=\"00:00:06.000\">\n"
      "        <span tts:fontWeight=\"bold\">foo</span>\n"
      "        <span tts:fontStyle=\"italic\">bar</span>\n"
      "        <span tts:textDecoration=\"underline\">baz</span>\n"
      "      </p>\n"
      "    </div>\n"
      "  </body>\n"
      "</tt>\n";

  TestProperties properties;
  properties.body.sub_fragments.emplace_back(kNoStyles, "foo");
  properties.body.sub_fragments.back().style.bold = true;
  properties.body.sub_fragments.emplace_back(kNoStyles, "bar");
  properties.body.sub_fragments.back().style.italic = true;
  properties.body.sub_fragments.emplace_back(kNoStyles, "baz");
  properties.body.sub_fragments.back().style.underline = true;

  ParseSingleCue(kExpectedOutput, properties);
}

TEST_F(assMuxerTest, HandlesRegions) {
  const char* kExpectedOutput =
      "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
      "<tt xmlns=\"http://www.w3.org/ns/ass\" "
      "xmlns:tts=\"http://www.w3.org/ns/ass#styling\" xml:lang=\"\">\n"
      "  <head>\n"
      "    <metadata/>\n"
      "    <styling/>\n"
      "    <layout>\n"
      "      <region xml:id=\"foo\" tts:origin=\"20px 40px\" "
      "tts:extent=\"22% 33%\" tts:overflow=\"visible\"/>\n"
      "    </layout>\n"
      "  </head>\n"
      "  <body>\n"
      "    <div>\n"
      "      <p xml:space=\"preserve\" begin=\"00:00:05.000\" "
      "end=\"00:00:06.000\" region=\"foo\">bar</p>\n"
      "    </div>\n"
      "  </body>\n"
      "</tt>\n";

  TestProperties properties;
  properties.settings.region = "foo";
  properties.body.body = "bar";

  TextRegion region;
  region.width = TextNumber(22, TextUnitType::kPercent);
  region.height = TextNumber(33, TextUnitType::kPercent);
  region.window_anchor_x = TextNumber(20, TextUnitType::kPixels);
  region.window_anchor_y = TextNumber(40, TextUnitType::kPixels);
  properties.regions.emplace("foo", region);

  ParseSingleCue(kExpectedOutput, properties);
}

TEST_F(assMuxerTest, HandlesLanguage) {
  const char* kExpectedOutput =
      "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
      "<tt xmlns=\"http://www.w3.org/ns/ass\" "
      "xmlns:tts=\"http://www.w3.org/ns/ass#styling\" xml:lang=\"foo\">\n"
      "  <head>\n"
      "    <metadata/>\n"
      "    <styling/>\n"
      "    <layout/>\n"
      "  </head>\n"
      "  <body>\n"
      "    <div>\n"
      "      <p xml:space=\"preserve\" begin=\"00:00:05.000\" "
      "end=\"00:00:06.000\">bar</p>\n"
      "    </div>\n"
      "  </body>\n"
      "</tt>\n";

  TestProperties properties;
  properties.body.body = "bar";
  properties.language = "foo";

  ParseSingleCue(kExpectedOutput, properties);
}

TEST_F(assMuxerTest, HandlesPosition) {
  const char* kExpectedOutput =
      "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
      "<tt xmlns=\"http://www.w3.org/ns/ass\" "
      "xmlns:tts=\"http://www.w3.org/ns/ass#styling\" xml:lang=\"\">\n"
      "  <head>\n"
      "    <metadata/>\n"
      "    <styling/>\n"
      "    <layout/>\n"
      "  </head>\n"
      "  <body>\n"
      "    <div>\n"
      "      <region xml:id=\"_shaka_region_0\" tts:origin=\"30% 4em\" "
      "tts:extent=\"100px 1em\"/>\n"
      "      <p xml:space=\"preserve\" begin=\"00:00:05.000\" "
      "end=\"00:00:06.000\" region=\"_shaka_region_0\">bar</p>\n"
      "    </div>\n"
      "  </body>\n"
      "</tt>\n";

  TestProperties properties;
  properties.settings.position.emplace(30, TextUnitType::kPercent);
  properties.settings.line.emplace(4, TextUnitType::kLines);
  properties.settings.width.emplace(100, TextUnitType::kPixels);
  properties.settings.height.emplace(1, TextUnitType::kLines);
  properties.body.body = "bar";

  ParseSingleCue(kExpectedOutput, properties);
}

TEST_F(assMuxerTest, HandlesOtherSettings) {
  const char* kExpectedOutput =
      "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
      "<tt xmlns=\"http://www.w3.org/ns/ass\" "
      "xmlns:tts=\"http://www.w3.org/ns/ass#styling\" xml:lang=\"\">\n"
      "  <head>\n"
      "    <metadata/>\n"
      "    <styling/>\n"
      "    <layout/>\n"
      "  </head>\n"
      "  <body>\n"
      "    <div>\n"
      "      <p xml:space=\"preserve\" begin=\"00:00:05.000\" "
      "end=\"00:00:06.000\" tts:writingMode=\"tblr\" "
      "tts:textAlign=\"end\">bar</p>\n"
      "    </div>\n"
      "  </body>\n"
      "</tt>\n";

  TestProperties properties;
  properties.settings.writing_direction =
      WritingDirection::kVerticalGrowingRight;
  properties.settings.text_alignment = TextAlignment::kEnd;
  properties.body.body = "bar";

  ParseSingleCue(kExpectedOutput, properties);
}

TEST_F(assMuxerTest, HandlesCueId) {
  const char* kExpectedOutput =
      "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
      "<tt xmlns=\"http://www.w3.org/ns/ass\" "
      "xmlns:tts=\"http://www.w3.org/ns/ass#styling\" xml:lang=\"\">\n"
      "  <head>\n"
      "    <metadata/>\n"
      "    <styling/>\n"
      "    <layout/>\n"
      "  </head>\n"
      "  <body>\n"
      "    <div>\n"
      "      <p xml:space=\"preserve\" begin=\"00:00:05.000\" "
      "end=\"00:00:06.000\" xml:id=\"foo\">bar</p>\n"
      "    </div>\n"
      "  </body>\n"
      "</tt>\n";

  TestProperties properties;
  properties.id = "foo";
  properties.body.body = "bar";

  ParseSingleCue(kExpectedOutput, properties);
}

TEST_F(assMuxerTest, EscapesSpecialChars) {
  const char* kExpectedOutput =
      "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
      "<tt xmlns=\"http://www.w3.org/ns/ass\" "
      "xmlns:tts=\"http://www.w3.org/ns/ass#styling\" "
      "xml:lang=\"foo&amp;&quot;a\">\n"
      "  <head>\n"
      "    <metadata/>\n"
      "    <styling/>\n"
      "    <layout>\n"
      "      <region xml:id=\"&lt;a&amp;&quot;\" tts:origin=\"0% 0%\" "
      "tts:extent=\"100% 100%\" tts:overflow=\"visible\"/>\n"
      "    </layout>\n"
      "  </head>\n"
      "  <body>\n"
      "    <div>\n"
      "      <p xml:space=\"preserve\" begin=\"00:00:05.000\" "
      "end=\"00:00:06.000\" xml:id=\"foo&lt;a&amp;&quot;\" "
      "region=\"&lt;a&amp;&quot;\">&lt;tag&gt;\"foo&amp;bar\"</p>\n"
      "    </div>\n"
      "  </body>\n"
      "</tt>\n";

  TestProperties properties;
  properties.id = "foo<a&\"";
  properties.settings.region = "<a&\"";
  properties.body.body = "<tag>\"foo&bar\"";
  properties.language = "foo&\"a";
  properties.regions.emplace("<a&\"", TextRegion());

  ParseSingleCue(kExpectedOutput, properties);
}

TEST_F(assMuxerTest, HandlesReset) {
  const char* kExpectedOutput1 =
      "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
      "<tt xmlns=\"http://www.w3.org/ns/ass\" "
      "xmlns:tts=\"http://www.w3.org/ns/ass#styling\" xml:lang=\"foobar\">\n"
      "  <head>\n"
      "    <metadata/>\n"
      "    <styling/>\n"
      "    <layout/>\n"
      "  </head>\n"
      "  <body>\n"
      "    <div>\n"
      "      <p xml:space=\"preserve\" begin=\"00:00:05.000\" "
      "end=\"00:00:06.000\">foo</p>\n"
      "    </div>\n"
      "  </body>\n"
      "</tt>\n";
  const char* kExpectedOutput2 =
      "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
      "<tt xmlns=\"http://www.w3.org/ns/ass\" "
      "xmlns:tts=\"http://www.w3.org/ns/ass#styling\" xml:lang=\"foobar\">\n"
      "  <head>\n"
      "    <metadata/>\n"
      "    <styling/>\n"
      "    <layout/>\n"
      "  </head>\n"
      "  <body>\n"
      "    <div>\n"
      "      <p xml:space=\"preserve\" begin=\"00:00:08.000\" "
      "end=\"00:00:09.000\">bar</p>\n"
      "    </div>\n"
      "  </body>\n"
      "</tt>\n";

  AssGenerator generator;
  generator.Initialize({}, "foobar", kMsTimeScale);
  generator.AddSample(TextSample(kNoId, 5000, 6000, DefaultSettings(),
                                 TextFragment(kNoStyles, "foo")));

  std::string results;
  ASSERT_TRUE(generator.Dump(&results));
  ASSERT_EQ(results, kExpectedOutput1);

  results.clear();
  generator.Reset();
  generator.AddSample(TextSample(kNoId, 8000, 9000, DefaultSettings(),
                                 TextFragment(kNoStyles, "bar")));

  ASSERT_TRUE(generator.Dump(&results));
  ASSERT_EQ(results, kExpectedOutput2);
}

TEST_F(assMuxerTest, HandlesImage) {
  const char* kExpectedOutput =
      "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
      "<tt xmlns=\"http://www.w3.org/ns/ass\" "
      "xmlns:tts=\"http://www.w3.org/ns/ass#styling\" xml:lang=\"\" "
      "xmlns:smpte=\"http://www.smpte-ra.org/schemas/2052-1/2010/smpte-tt\">\n"
      "  <head>\n"
      "    <metadata>\n"
      "      <smpte:image imageType=\"PNG\" encoding=\"Base64\" "
      "xml:id=\"img_1\">"
      "AQID</smpte:image>\n"
      "    </metadata>\n"
      "    <styling/>\n"
      "    <layout/>\n"
      "  </head>\n"
      "  <body>\n"
      "    <div>\n"
      "      <p xml:space=\"preserve\" begin=\"00:00:05.000\" "
      "end=\"00:00:06.000\" smpte:backgroundImage=\"#img_1\" xml:id=\"foo\"/>\n"
      "    </div>\n"
      "  </body>\n"
      "</tt>\n";

  TestProperties properties;
  properties.id = "foo";
  properties.body.image = {1, 2, 3};

  ParseSingleCue(kExpectedOutput, properties);
}

TEST_F(assMuxerTest, FormatsTimeWithFixedNumberOfDigits) {
  const char* kExpectedOutput =
      "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
      "<tt xmlns=\"http://www.w3.org/ns/ass\" "
      "xmlns:tts=\"http://www.w3.org/ns/ass#styling\" xml:lang=\"\">\n"
      "  <head>\n"
      "    <metadata/>\n"
      "    <styling/>\n"
      "    <layout/>\n"
      "  </head>\n"
      "  <body>\n"
      "    <div>\n"
      "      <p xml:space=\"preserve\" begin=\"00:00:00.000\" "
      "end=\"00:00:00.001\">payload</p>\n"
      "    </div>\n"
      "  </body>\n"
      "</tt>\n";

  TestProperties properties;
  properties.body.body = "payload";
  properties.start = 0;
  properties.end = 1;
  ParseSingleCue(kExpectedOutput, properties);
}

TEST_F(assMuxerTest, HandlesTeleTextToEbuTTD) {
  const char* kExpectedOutput =
      "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
      "<tt xmlns=\"http://www.w3.org/ns/ass\" "
      "xmlns:tts=\"http://www.w3.org/ns/ass#styling\" xml:lang=\"\" "
      "xmlns:ttp=\"http://www.w3.org/ns/ass#parameter\" "
      "xmlns:ttm=\"http://www.w3.org/ns/ass#metadata\" "
      "xmlns:ebuttm=\"urn:ebu:tt:metadata\" xmlns:ebutts=\"urn:ebu:tt:style\" "
      "xml:space=\"default\" "
      "ttp:timeBase=\"media\" ttp:cellResolution=\"32 15\">\n"
      "  <head>\n"
      "    <metadata/>\n"
      "    <styling>\n"
      "      <style xml:id=\"default\" tts:fontStyle=\"normal\" "
      "tts:fontFamily=\"sansSerif\" tts:fontSize=\"100%\" "
      "tts:lineHeight=\"normal\" "
      "tts:textAlign=\"center\" ebutts:linePadding=\"0.5c\"/>\n"
      "      <style xml:id=\"red_cyan\" tts:backgroundColor=\"cyan\" "
      "tts:color=\"red\"/>\n"
      "    </styling>\n"
      "    <layout>\n"
      "      <region xml:id=\"ttx_9\" tts:origin=\"10% 70%\" "
      "tts:extent=\"80% 15%\" tts:overflow=\"visible\"/>\n"
      "    </layout>\n"
      "  </head>\n"
      "  <body style=\"default\">\n"
      "    <div>\n"
      "      <p begin=\"00:00:05.000\" end=\"00:00:06.000\" region=\"ttx_9\">\n"
      "        <span style=\"red_cyan\">teletext to EBU-TT-D</span>\n"
      "      </p>\n"
      "    </div>\n"
      "  </body>\n"
      "</tt>\n";

  TestProperties properties;
  properties.settings.line.emplace(9, TextUnitType::kLines);
  properties.settings.region = "ttx_9";
  properties.settings.height.emplace(1, TextUnitType::kLines);
  properties.body.body = "teletext to EBU-TT-D";
  properties.body.style.color = "red";
  properties.body.style.backgroundColor = "cyan";

  ParseSingleCue(kExpectedOutput, properties);
}

}  // namespace ass
}  // namespace media
}  // namespace shaka
