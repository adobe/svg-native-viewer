/*
Copyright 2023 Adobe. All rights reserved.
This file is licensed to you under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License. You may obtain a copy
of the License at http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under
the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR REPRESENTATIONS
OF ANY KIND, either express or implied. See the License for the specific language
governing permissions and limitations under the License.
*/

#include "gtest/gtest.h"
#include "gtest/gtest-spi.h"

#include <svgnative/SVGRenderer.h>
#include "SVGDocumentImpl.h"
#include "SVGStringParser.h"

using namespace SVGNative;

using BeforeAfterPath = std::pair<std::string, Color>;

class ParsingTestFixture : public ::testing::TestWithParam<BeforeAfterPath> {
};

TEST_P(ParsingTestFixture, color_mix)
{
    const auto& param = GetParam();

    static const ColorMap colorMap{
        { "myBlue",     Color{0.f, 0.f, 1.f, 1.f}},
        { "myRed",      Color{1.f, 0.f, 0.f, 1.f}},
        { "myGreen",    Color{0.f, 0.5f, 0.f, 1.f}},
    };

    ColorImpl colorImpl;
    auto parsingResult = SVGStringParser::ParseColor(param.first, colorImpl);

    auto wasParsingSuccessful = parsingResult == SVGDocumentImpl::Result::kSuccess;
    EXPECT_EQ(wasParsingSuccessful, true);
    EXPECT_EQ(colorImpl.type() == typeid(ColorMixPtr), true);

    auto blendedColor = SVGNative::get<ColorMixPtr>(colorImpl)->BlendedColor(colorMap);
    auto& expectedColor = param.second;
    EXPECT_FLOAT_EQ(blendedColor[0], expectedColor[0]);
    EXPECT_FLOAT_EQ(blendedColor[1], expectedColor[1]);
    EXPECT_FLOAT_EQ(blendedColor[2], expectedColor[2]);
    EXPECT_FLOAT_EQ(blendedColor[3], expectedColor[3]);
}

INSTANTIATE_TEST_SUITE_P(ColorMixParsing, ParsingTestFixture,
    ::testing::ValuesIn(std::vector<BeforeAfterPath>{
        {
            "color-mix(in srgb, black, white 80%)",
            Color{{.8f, .8f, .8f, 1.f}}
        },
        {
            "color-mix(in srgb, black 80%, white)",
            Color{{.2f, .2f, .2f, 1.f}}
        },
        {
            "color-mix(in srgb, rgb(255, 127, 0) 80%, rgba(0, 127, 0, 1.0))",
            Color{{.2f, .2f, .2f, 1.f}}
        }
    }));
