/*
Copyright 2019 Adobe. All rights reserved.
This file is licensed to you under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License. You may obtain a copy
of the License at http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under
the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR REPRESENTATIONS
OF ANY KIND, either express or implied. See the License for the specific language
governing permissions and limitations under the License.
*/

#pragma once

#include "SVGRenderer.h"
#include <array>

namespace SVGNative
{

struct CSSColorInfo
{
    constexpr CSSColorInfo(const char* aColorName, size_t aLength, Color aColor)
        : colorName{aColorName}
        , length{aLength}
        , color{std::move(aColor)}
    {
    }
    const char* colorName{};
    size_t length{};
    Color color{};
};

// Generated in web browser with following code. ele must be an element embedded in a loaded document.
// var string = '';
// [/*list of all color names*/].forEach(name => {
//     ele.style.fill = name;
//     let rgb = (window.getComputedStyle(ele).fill).match(/\d+/g);
//     string += String('	{"' + name + '"').padEnd(25) + ', ' + String(name.length).padEnd(2) + ' , {{'
//         + rgb.map(item => { return String(rgb[0] + '.0f / 255.0f').padStart(13)}).join(', ')
//         + ', 1.0f}}},\n';
// });
constexpr std::array<CSSColorInfo, 148> gCSSNamedColors = {{
    {"aliceblue"            , 9  , {{240.0f / 255.0f, 248.0f / 255.0f, 255.0f / 255.0f, 1.0f}}},
    {"antiquewhite"         , 12 , {{250.0f / 255.0f, 235.0f / 255.0f, 215.0f / 255.0f, 1.0f}}},
    {"aqua"                 , 4  , {{  0.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f, 1.0f}}},
    {"aquamarine"           , 10 , {{127.0f / 255.0f, 255.0f / 255.0f, 212.0f / 255.0f, 1.0f}}},
    {"azure"                , 5  , {{240.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f, 1.0f}}},
    {"beige"                , 5  , {{245.0f / 255.0f, 245.0f / 255.0f, 220.0f / 255.0f, 1.0f}}},
    {"bisque"               , 6  , {{255.0f / 255.0f, 228.0f / 255.0f, 196.0f / 255.0f, 1.0f}}},
    {"black"                , 5  , {{  0.0f / 255.0f,   0.0f / 255.0f,   0.0f / 255.0f, 1.0f}}},
    {"blanchedalmond"       , 14 , {{255.0f / 255.0f, 235.0f / 255.0f, 205.0f / 255.0f, 1.0f}}},
    {"blue"                 , 4  , {{  0.0f / 255.0f,   0.0f / 255.0f, 255.0f / 255.0f, 1.0f}}},
    {"blueviolet"           , 10 , {{138.0f / 255.0f,  43.0f / 255.0f, 226.0f / 255.0f, 1.0f}}},
    {"brown"                , 5  , {{165.0f / 255.0f,  42.0f / 255.0f,  42.0f / 255.0f, 1.0f}}},
    {"burlywood"            , 9  , {{222.0f / 255.0f, 184.0f / 255.0f, 135.0f / 255.0f, 1.0f}}},
    {"cadetblue"            , 9  , {{ 95.0f / 255.0f, 158.0f / 255.0f, 160.0f / 255.0f, 1.0f}}},
    {"chartreuse"           , 10 , {{127.0f / 255.0f, 255.0f / 255.0f,   0.0f / 255.0f, 1.0f}}},
    {"chocolate"            , 9  , {{210.0f / 255.0f, 105.0f / 255.0f,  30.0f / 255.0f, 1.0f}}},
    {"coral"                , 5  , {{255.0f / 255.0f, 127.0f / 255.0f,  80.0f / 255.0f, 1.0f}}},
    {"cornflowerblue"       , 14 , {{100.0f / 255.0f, 149.0f / 255.0f, 237.0f / 255.0f, 1.0f}}},
    {"cornsilk"             , 8  , {{255.0f / 255.0f, 248.0f / 255.0f, 220.0f / 255.0f, 1.0f}}},
    {"crimson"              , 7  , {{220.0f / 255.0f,  20.0f / 255.0f,  60.0f / 255.0f, 1.0f}}},
    {"cyan"                 , 4  , {{  0.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f, 1.0f}}},
    {"darkblue"             , 8  , {{  0.0f / 255.0f,   0.0f / 255.0f, 139.0f / 255.0f, 1.0f}}},
    {"darkcyan"             , 8  , {{  0.0f / 255.0f, 139.0f / 255.0f, 139.0f / 255.0f, 1.0f}}},
    {"darkgoldenrod"        , 13 , {{184.0f / 255.0f, 134.0f / 255.0f,  11.0f / 255.0f, 1.0f}}},
    {"darkgray"             , 8  , {{169.0f / 255.0f, 169.0f / 255.0f, 169.0f / 255.0f, 1.0f}}},
    {"darkgreen"            , 9  , {{  0.0f / 255.0f, 100.0f / 255.0f,   0.0f / 255.0f, 1.0f}}},
    {"darkgrey"             , 8  , {{169.0f / 255.0f, 169.0f / 255.0f, 169.0f / 255.0f, 1.0f}}},
    {"darkkhaki"            , 9  , {{189.0f / 255.0f, 183.0f / 255.0f, 107.0f / 255.0f, 1.0f}}},
    {"darkmagenta"          , 11 , {{139.0f / 255.0f,   0.0f / 255.0f, 139.0f / 255.0f, 1.0f}}},
    {"darkolivegreen"       , 14 , {{ 85.0f / 255.0f, 107.0f / 255.0f,  47.0f / 255.0f, 1.0f}}},
    {"darkorange"           , 10 , {{255.0f / 255.0f, 140.0f / 255.0f,   0.0f / 255.0f, 1.0f}}},
    {"darkorchid"           , 10 , {{153.0f / 255.0f,  50.0f / 255.0f, 204.0f / 255.0f, 1.0f}}},
    {"darkred"              , 7  , {{139.0f / 255.0f,   0.0f / 255.0f,   0.0f / 255.0f, 1.0f}}},
    {"darksalmon"           , 10 , {{233.0f / 255.0f, 150.0f / 255.0f, 122.0f / 255.0f, 1.0f}}},
    {"darkseagreen"         , 12 , {{143.0f / 255.0f, 188.0f / 255.0f, 143.0f / 255.0f, 1.0f}}},
    {"darkslateblue"        , 13 , {{ 72.0f / 255.0f,  61.0f / 255.0f, 139.0f / 255.0f, 1.0f}}},
    {"darkslategray"        , 13 , {{ 47.0f / 255.0f,  79.0f / 255.0f,  79.0f / 255.0f, 1.0f}}},
    {"darkslategrey"        , 13 , {{ 47.0f / 255.0f,  79.0f / 255.0f,  79.0f / 255.0f, 1.0f}}},
    {"darkturquoise"        , 13 , {{  0.0f / 255.0f, 206.0f / 255.0f, 209.0f / 255.0f, 1.0f}}},
    {"darkviolet"           , 10 , {{148.0f / 255.0f,   0.0f / 255.0f, 211.0f / 255.0f, 1.0f}}},
    {"deeppink"             , 8  , {{255.0f / 255.0f,  20.0f / 255.0f, 147.0f / 255.0f, 1.0f}}},
    {"deepskyblue"          , 11 , {{  0.0f / 255.0f, 191.0f / 255.0f, 255.0f / 255.0f, 1.0f}}},
    {"dimgray"              , 7  , {{105.0f / 255.0f, 105.0f / 255.0f, 105.0f / 255.0f, 1.0f}}},
    {"dimgrey"              , 7  , {{105.0f / 255.0f, 105.0f / 255.0f, 105.0f / 255.0f, 1.0f}}},
    {"dodgerblue"           , 10 , {{ 30.0f / 255.0f, 144.0f / 255.0f, 255.0f / 255.0f, 1.0f}}},
    {"firebrick"            , 9  , {{178.0f / 255.0f,  34.0f / 255.0f,  34.0f / 255.0f, 1.0f}}},
    {"floralwhite"          , 11 , {{255.0f / 255.0f, 250.0f / 255.0f, 240.0f / 255.0f, 1.0f}}},
    {"forestgreen"          , 11 , {{ 34.0f / 255.0f, 139.0f / 255.0f,  34.0f / 255.0f, 1.0f}}},
    {"fuchsia"              , 7  , {{255.0f / 255.0f,   0.0f / 255.0f, 255.0f / 255.0f, 1.0f}}},
    {"gainsboro"            , 9  , {{220.0f / 255.0f, 220.0f / 255.0f, 220.0f / 255.0f, 1.0f}}},
    {"ghostwhite"           , 10 , {{248.0f / 255.0f, 248.0f / 255.0f, 255.0f / 255.0f, 1.0f}}},
    {"gold"                 , 4  , {{255.0f / 255.0f, 215.0f / 255.0f,   0.0f / 255.0f, 1.0f}}},
    {"goldenrod"            , 9  , {{218.0f / 255.0f, 165.0f / 255.0f,  32.0f / 255.0f, 1.0f}}},
    {"gray"                 , 4  , {{128.0f / 255.0f, 128.0f / 255.0f, 128.0f / 255.0f, 1.0f}}},
    {"green"                , 5  , {{  0.0f / 255.0f, 128.0f / 255.0f,   0.0f / 255.0f, 1.0f}}},
    {"greenyellow"          , 11 , {{173.0f / 255.0f, 255.0f / 255.0f,  47.0f / 255.0f, 1.0f}}},
    {"grey"                 , 4  , {{128.0f / 255.0f, 128.0f / 255.0f, 128.0f / 255.0f, 1.0f}}},
    {"honeydew"             , 8  , {{240.0f / 255.0f, 255.0f / 255.0f, 240.0f / 255.0f, 1.0f}}},
    {"hotpink"              , 7  , {{255.0f / 255.0f, 105.0f / 255.0f, 180.0f / 255.0f, 1.0f}}},
    {"indianred"            , 9  , {{205.0f / 255.0f,  92.0f / 255.0f,  92.0f / 255.0f, 1.0f}}},
    {"indigo"               , 6  , {{ 75.0f / 255.0f,   0.0f / 255.0f, 130.0f / 255.0f, 1.0f}}},
    {"ivory"                , 5  , {{255.0f / 255.0f, 255.0f / 255.0f, 240.0f / 255.0f, 1.0f}}},
    {"khaki"                , 5  , {{240.0f / 255.0f, 230.0f / 255.0f, 140.0f / 255.0f, 1.0f}}},
    {"lavender"             , 8  , {{230.0f / 255.0f, 230.0f / 255.0f, 250.0f / 255.0f, 1.0f}}},
    {"lavenderblush"        , 13 , {{255.0f / 255.0f, 240.0f / 255.0f, 245.0f / 255.0f, 1.0f}}},
    {"lawngreen"            , 9  , {{124.0f / 255.0f, 252.0f / 255.0f,   0.0f / 255.0f, 1.0f}}},
    {"lemonchiffon"         , 12 , {{255.0f / 255.0f, 250.0f / 255.0f, 205.0f / 255.0f, 1.0f}}},
    {"darkslateblue"        , 13 , {{ 72.0f / 255.0f,  61.0f / 255.0f, 139.0f / 255.0f, 1.0f}}},
    {"lightcoral"           , 10 , {{240.0f / 255.0f, 128.0f / 255.0f, 128.0f / 255.0f, 1.0f}}},
    {"lightcyan"            , 9  , {{224.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f, 1.0f}}},
    {"lightgoldenrodyellow" , 20 , {{250.0f / 255.0f, 250.0f / 255.0f, 210.0f / 255.0f, 1.0f}}},
    {"lightgray"            , 9  , {{211.0f / 255.0f, 211.0f / 255.0f, 211.0f / 255.0f, 1.0f}}},
    {"lightgreen"           , 10 , {{144.0f / 255.0f, 238.0f / 255.0f, 144.0f / 255.0f, 1.0f}}},
    {"lightgrey"            , 9  , {{211.0f / 255.0f, 211.0f / 255.0f, 211.0f / 255.0f, 1.0f}}},
    {"lightpink"            , 9  , {{255.0f / 255.0f, 182.0f / 255.0f, 193.0f / 255.0f, 1.0f}}},
    {"lightsalmon"          , 11 , {{255.0f / 255.0f, 160.0f / 255.0f, 122.0f / 255.0f, 1.0f}}},
    {"lightseagreen"        , 13 , {{ 32.0f / 255.0f, 178.0f / 255.0f, 170.0f / 255.0f, 1.0f}}},
    {"lightskyblue"         , 12 , {{135.0f / 255.0f, 206.0f / 255.0f, 250.0f / 255.0f, 1.0f}}},
    {"lightslategray"       , 14 , {{119.0f / 255.0f, 136.0f / 255.0f, 153.0f / 255.0f, 1.0f}}},
    {"lightslategrey"       , 14 , {{119.0f / 255.0f, 136.0f / 255.0f, 153.0f / 255.0f, 1.0f}}},
    {"lightsteelblue"       , 14 , {{176.0f / 255.0f, 196.0f / 255.0f, 222.0f / 255.0f, 1.0f}}},
    {"lightyellow"          , 11 , {{255.0f / 255.0f, 255.0f / 255.0f, 224.0f / 255.0f, 1.0f}}},
    {"lime"                 , 4  , {{  0.0f / 255.0f, 255.0f / 255.0f,   0.0f / 255.0f, 1.0f}}},
    {"limegreen"            , 9  , {{ 50.0f / 255.0f, 205.0f / 255.0f,  50.0f / 255.0f, 1.0f}}},
    {"linen"                , 5  , {{250.0f / 255.0f, 240.0f / 255.0f, 230.0f / 255.0f, 1.0f}}},
    {"magenta"              , 7  , {{255.0f / 255.0f,   0.0f / 255.0f, 255.0f / 255.0f, 1.0f}}},
    {"maroon"               , 6  , {{128.0f / 255.0f,   0.0f / 255.0f,   0.0f / 255.0f, 1.0f}}},
    {"mediumaquamarine"     , 16 , {{102.0f / 255.0f, 205.0f / 255.0f, 170.0f / 255.0f, 1.0f}}},
    {"mediumblue"           , 10 , {{  0.0f / 255.0f,   0.0f / 255.0f, 205.0f / 255.0f, 1.0f}}},
    {"mediumorchid"         , 12 , {{186.0f / 255.0f,  85.0f / 255.0f, 211.0f / 255.0f, 1.0f}}},
    {"mediumpurple"         , 12 , {{147.0f / 255.0f, 112.0f / 255.0f, 219.0f / 255.0f, 1.0f}}},
    {"mediumseagreen"       , 14 , {{ 60.0f / 255.0f, 179.0f / 255.0f, 113.0f / 255.0f, 1.0f}}},
    {"mediumslateblue"      , 15 , {{123.0f / 255.0f, 104.0f / 255.0f, 238.0f / 255.0f, 1.0f}}},
    {"mediumspringgreen"    , 17 , {{  0.0f / 255.0f, 250.0f / 255.0f, 154.0f / 255.0f, 1.0f}}},
    {"mediumturquoise"      , 15 , {{ 72.0f / 255.0f, 209.0f / 255.0f, 204.0f / 255.0f, 1.0f}}},
    {"mediumvioletred"      , 15 , {{199.0f / 255.0f,  21.0f / 255.0f, 133.0f / 255.0f, 1.0f}}},
    {"midnightblue"         , 12 , {{ 25.0f / 255.0f,  25.0f / 255.0f, 112.0f / 255.0f, 1.0f}}},
    {"mintcream"            , 9  , {{245.0f / 255.0f, 255.0f / 255.0f, 250.0f / 255.0f, 1.0f}}},
    {"mistyrose"            , 9  , {{255.0f / 255.0f, 228.0f / 255.0f, 225.0f / 255.0f, 1.0f}}},
    {"moccasin"             , 8  , {{255.0f / 255.0f, 228.0f / 255.0f, 181.0f / 255.0f, 1.0f}}},
    {"navajowhite"          , 11 , {{255.0f / 255.0f, 222.0f / 255.0f, 173.0f / 255.0f, 1.0f}}},
    {"navy"                 , 4  , {{  0.0f / 255.0f,   0.0f / 255.0f, 128.0f / 255.0f, 1.0f}}},
    {"oldlace"              , 7  , {{253.0f / 255.0f, 245.0f / 255.0f, 230.0f / 255.0f, 1.0f}}},
    {"olive"                , 5  , {{128.0f / 255.0f, 128.0f / 255.0f,   0.0f / 255.0f, 1.0f}}},
    {"olivedrab"            , 9  , {{107.0f / 255.0f, 142.0f / 255.0f,  35.0f / 255.0f, 1.0f}}},
    {"orange"               , 6  , {{255.0f / 255.0f, 165.0f / 255.0f,   0.0f / 255.0f, 1.0f}}},
    {"orangered"            , 9  , {{255.0f / 255.0f,  69.0f / 255.0f,   0.0f / 255.0f, 1.0f}}},
    {"orchid"               , 6  , {{218.0f / 255.0f, 112.0f / 255.0f, 214.0f / 255.0f, 1.0f}}},
    {"palegoldenrod"        , 13 , {{238.0f / 255.0f, 232.0f / 255.0f, 170.0f / 255.0f, 1.0f}}},
    {"palegreen"            , 9  , {{152.0f / 255.0f, 251.0f / 255.0f, 152.0f / 255.0f, 1.0f}}},
    {"paleturquoise"        , 13 , {{175.0f / 255.0f, 238.0f / 255.0f, 238.0f / 255.0f, 1.0f}}},
    {"palevioletred"        , 13 , {{219.0f / 255.0f, 112.0f / 255.0f, 147.0f / 255.0f, 1.0f}}},
    {"papayawhip"           , 10 , {{255.0f / 255.0f, 239.0f / 255.0f, 213.0f / 255.0f, 1.0f}}},
    {"peachpuff"            , 9  , {{255.0f / 255.0f, 218.0f / 255.0f, 185.0f / 255.0f, 1.0f}}},
    {"peru"                 , 4  , {{205.0f / 255.0f, 133.0f / 255.0f,  63.0f / 255.0f, 1.0f}}},
    {"pink"                 , 4  , {{255.0f / 255.0f, 192.0f / 255.0f, 203.0f / 255.0f, 1.0f}}},
    {"plum"                 , 4  , {{221.0f / 255.0f, 160.0f / 255.0f, 221.0f / 255.0f, 1.0f}}},
    {"powderblue"           , 10 , {{176.0f / 255.0f, 224.0f / 255.0f, 230.0f / 255.0f, 1.0f}}},
    {"purple"               , 6  , {{128.0f / 255.0f,   0.0f / 255.0f, 128.0f / 255.0f, 1.0f}}},
    {"rebeccapurple"        , 13 , {{102.0f / 255.0f,  51.0f / 255.0f, 153.0f / 255.0f, 1.0f}}},
    {"red"                  , 3  , {{255.0f / 255.0f,   0.0f / 255.0f,   0.0f / 255.0f, 1.0f}}},
    {"rosybrown"            , 9  , {{188.0f / 255.0f, 143.0f / 255.0f, 143.0f / 255.0f, 1.0f}}},
    {"royalblue"            , 9  , {{ 65.0f / 255.0f, 105.0f / 255.0f, 225.0f / 255.0f, 1.0f}}},
    {"saddlebrown"          , 11 , {{139.0f / 255.0f,  69.0f / 255.0f,  19.0f / 255.0f, 1.0f}}},
    {"salmon"               , 6  , {{250.0f / 255.0f, 128.0f / 255.0f, 114.0f / 255.0f, 1.0f}}},
    {"sandybrown"           , 10 , {{244.0f / 255.0f, 164.0f / 255.0f,  96.0f / 255.0f, 1.0f}}},
    {"seagreen"             , 8  , {{ 46.0f / 255.0f, 139.0f / 255.0f,  87.0f / 255.0f, 1.0f}}},
    {"seashell"             , 8  , {{255.0f / 255.0f, 245.0f / 255.0f, 238.0f / 255.0f, 1.0f}}},
    {"sienna"               , 6  , {{160.0f / 255.0f,  82.0f / 255.0f,  45.0f / 255.0f, 1.0f}}},
    {"silver"               , 6  , {{192.0f / 255.0f, 192.0f / 255.0f, 192.0f / 255.0f, 1.0f}}},
    {"skyblue"              , 7  , {{135.0f / 255.0f, 206.0f / 255.0f, 235.0f / 255.0f, 1.0f}}},
    {"slateblue"            , 9  , {{106.0f / 255.0f,  90.0f / 255.0f, 205.0f / 255.0f, 1.0f}}},
    {"slategray"            , 9  , {{112.0f / 255.0f, 128.0f / 255.0f, 144.0f / 255.0f, 1.0f}}},
    {"slategrey"            , 9  , {{112.0f / 255.0f, 128.0f / 255.0f, 144.0f / 255.0f, 1.0f}}},
    {"snow"                 , 4  , {{255.0f / 255.0f, 250.0f / 255.0f, 250.0f / 255.0f, 1.0f}}},
    {"springgreen"          , 11 , {{  0.0f / 255.0f, 255.0f / 255.0f, 127.0f / 255.0f, 1.0f}}},
    {"steelblue"            , 9  , {{ 70.0f / 255.0f, 130.0f / 255.0f, 180.0f / 255.0f, 1.0f}}},
    {"tan"                  , 3  , {{210.0f / 255.0f, 180.0f / 255.0f, 140.0f / 255.0f, 1.0f}}},
    {"teal"                 , 4  , {{  0.0f / 255.0f, 128.0f / 255.0f, 128.0f / 255.0f, 1.0f}}},
    {"thistle"              , 7  , {{216.0f / 255.0f, 191.0f / 255.0f, 216.0f / 255.0f, 1.0f}}},
    {"tomato"               , 6  , {{255.0f / 255.0f,  99.0f / 255.0f,  71.0f / 255.0f, 1.0f}}},
    {"turquoise"            , 9  , {{ 64.0f / 255.0f, 224.0f / 255.0f, 208.0f / 255.0f, 1.0f}}},
    {"violet"               , 6  , {{238.0f / 255.0f, 130.0f / 255.0f, 238.0f / 255.0f, 1.0f}}},
    {"wheat"                , 5  , {{245.0f / 255.0f, 222.0f / 255.0f, 179.0f / 255.0f, 1.0f}}},
    {"white"                , 5  , {{255.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f, 1.0f}}},
    {"whitesmoke"           , 10 , {{245.0f / 255.0f, 245.0f / 255.0f, 245.0f / 255.0f, 1.0f}}},
    {"yellow"               , 6  , {{255.0f / 255.0f, 255.0f / 255.0f,   0.0f / 255.0f, 1.0f}}},
    {"yellowgreen"          , 11 , {{154.0f / 255.0f, 205.0f / 255.0f,  50.0f / 255.0f, 1.0f}}}
}};

}