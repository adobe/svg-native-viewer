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

#include "Constants.h"
#include "SVGRenderer.h"
#include <array>
#include <tuple>

namespace SVGNative
{

using CSSColorInfo = std::tuple<SVG_C_CHAR, size_t, Color>;

// Generated in web browser with following code. ele must be an element embedded in a loaded document.
// var string = '';
// [/*list of all color names*/].forEach(name => {
//     ele.style.fill = name;
//     let rgb = (window.getComputedStyle(ele).fill).match(/\d+/g);
//     string += String('	CSSColorInfo{SVG_TO_C_CHAR(' + name + ')').padEnd(25) + ', ' + String(name.length).padEnd(2) + ' , {{'
//         + rgb.map(item => { return String(rgb[0] + '.0f / 255.0f').padStart(13)}).join(', ')
//         + ', 1.0f}}},\n';
// });
constexpr std::array<CSSColorInfo, 148> gCSSNamedColors = {{
    CSSColorInfo{SVG_TO_C_CHAR(aliceblue)            , 9  , {{240.0f / 255.0f, 248.0f / 255.0f, 255.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(antiquewhite)         , 12 , {{250.0f / 255.0f, 235.0f / 255.0f, 215.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(aqua)                 , 4  , {{  0.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(aquamarine)           , 10 , {{127.0f / 255.0f, 255.0f / 255.0f, 212.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(azure)                , 5  , {{240.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(beige)                , 5  , {{245.0f / 255.0f, 245.0f / 255.0f, 220.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(bisque)               , 6  , {{255.0f / 255.0f, 228.0f / 255.0f, 196.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(black)                , 5  , {{  0.0f / 255.0f,   0.0f / 255.0f,   0.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(blanchedalmond)       , 14 , {{255.0f / 255.0f, 235.0f / 255.0f, 205.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(blue)                 , 4  , {{  0.0f / 255.0f,   0.0f / 255.0f, 255.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(blueviolet)           , 10 , {{138.0f / 255.0f,  43.0f / 255.0f, 226.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(brown)                , 5  , {{165.0f / 255.0f,  42.0f / 255.0f,  42.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(burlywood)            , 9  , {{222.0f / 255.0f, 184.0f / 255.0f, 135.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(cadetblue)            , 9  , {{ 95.0f / 255.0f, 158.0f / 255.0f, 160.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(chartreuse)           , 10 , {{127.0f / 255.0f, 255.0f / 255.0f,   0.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(chocolate)            , 9  , {{210.0f / 255.0f, 105.0f / 255.0f,  30.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(coral)                , 5  , {{255.0f / 255.0f, 127.0f / 255.0f,  80.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(cornflowerblue)       , 14 , {{100.0f / 255.0f, 149.0f / 255.0f, 237.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(cornsilk)             , 8  , {{255.0f / 255.0f, 248.0f / 255.0f, 220.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(crimson)              , 7  , {{220.0f / 255.0f,  20.0f / 255.0f,  60.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(cyan)                 , 4  , {{  0.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(darkblue)             , 8  , {{  0.0f / 255.0f,   0.0f / 255.0f, 139.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(darkcyan)             , 8  , {{  0.0f / 255.0f, 139.0f / 255.0f, 139.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(darkgoldenrod)        , 13 , {{184.0f / 255.0f, 134.0f / 255.0f,  11.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(darkgray)             , 8  , {{169.0f / 255.0f, 169.0f / 255.0f, 169.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(darkgreen)            , 9  , {{  0.0f / 255.0f, 100.0f / 255.0f,   0.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(darkgrey)             , 8  , {{169.0f / 255.0f, 169.0f / 255.0f, 169.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(darkkhaki)            , 9  , {{189.0f / 255.0f, 183.0f / 255.0f, 107.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(darkmagenta)          , 11 , {{139.0f / 255.0f,   0.0f / 255.0f, 139.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(darkolivegreen)       , 14 , {{ 85.0f / 255.0f, 107.0f / 255.0f,  47.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(darkorange)           , 10 , {{255.0f / 255.0f, 140.0f / 255.0f,   0.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(darkorchid)           , 10 , {{153.0f / 255.0f,  50.0f / 255.0f, 204.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(darkred)              , 7  , {{139.0f / 255.0f,   0.0f / 255.0f,   0.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(darksalmon)           , 10 , {{233.0f / 255.0f, 150.0f / 255.0f, 122.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(darkseagreen)         , 12 , {{143.0f / 255.0f, 188.0f / 255.0f, 143.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(darkslateblue)        , 13 , {{ 72.0f / 255.0f,  61.0f / 255.0f, 139.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(darkslategray)        , 13 , {{ 47.0f / 255.0f,  79.0f / 255.0f,  79.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(darkslategrey)        , 13 , {{ 47.0f / 255.0f,  79.0f / 255.0f,  79.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(darkturquoise)        , 13 , {{  0.0f / 255.0f, 206.0f / 255.0f, 209.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(darkviolet)           , 10 , {{148.0f / 255.0f,   0.0f / 255.0f, 211.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(deeppink)             , 8  , {{255.0f / 255.0f,  20.0f / 255.0f, 147.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(deepskyblue)          , 11 , {{  0.0f / 255.0f, 191.0f / 255.0f, 255.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(dimgray)              , 7  , {{105.0f / 255.0f, 105.0f / 255.0f, 105.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(dimgrey)              , 7  , {{105.0f / 255.0f, 105.0f / 255.0f, 105.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(dodgerblue)           , 10 , {{ 30.0f / 255.0f, 144.0f / 255.0f, 255.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(firebrick)            , 9  , {{178.0f / 255.0f,  34.0f / 255.0f,  34.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(floralwhite)          , 11 , {{255.0f / 255.0f, 250.0f / 255.0f, 240.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(forestgreen)          , 11 , {{ 34.0f / 255.0f, 139.0f / 255.0f,  34.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(fuchsia)              , 7  , {{255.0f / 255.0f,   0.0f / 255.0f, 255.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(gainsboro)            , 9  , {{220.0f / 255.0f, 220.0f / 255.0f, 220.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(ghostwhite)           , 10 , {{248.0f / 255.0f, 248.0f / 255.0f, 255.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(gold)                 , 4  , {{255.0f / 255.0f, 215.0f / 255.0f,   0.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(goldenrod)            , 9  , {{218.0f / 255.0f, 165.0f / 255.0f,  32.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(gray)                 , 4  , {{128.0f / 255.0f, 128.0f / 255.0f, 128.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(green)                , 5  , {{  0.0f / 255.0f, 128.0f / 255.0f,   0.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(greenyellow)          , 11 , {{173.0f / 255.0f, 255.0f / 255.0f,  47.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(grey)                 , 4  , {{128.0f / 255.0f, 128.0f / 255.0f, 128.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(honeydew)             , 8  , {{240.0f / 255.0f, 255.0f / 255.0f, 240.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(hotpink)              , 7  , {{255.0f / 255.0f, 105.0f / 255.0f, 180.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(indianred)            , 9  , {{205.0f / 255.0f,  92.0f / 255.0f,  92.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(indigo)               , 6  , {{ 75.0f / 255.0f,   0.0f / 255.0f, 130.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(ivory)                , 5  , {{255.0f / 255.0f, 255.0f / 255.0f, 240.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(khaki)                , 5  , {{240.0f / 255.0f, 230.0f / 255.0f, 140.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(lavender)             , 8  , {{230.0f / 255.0f, 230.0f / 255.0f, 250.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(lavenderblush)        , 13 , {{255.0f / 255.0f, 240.0f / 255.0f, 245.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(lawngreen)            , 9  , {{124.0f / 255.0f, 252.0f / 255.0f,   0.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(lemonchiffon)         , 12 , {{255.0f / 255.0f, 250.0f / 255.0f, 205.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(darkslateblue)        , 13 , {{ 72.0f / 255.0f,  61.0f / 255.0f, 139.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(lightcoral)           , 10 , {{240.0f / 255.0f, 128.0f / 255.0f, 128.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(lightcyan)            , 9  , {{224.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(lightgoldenrodyellow) , 20 , {{250.0f / 255.0f, 250.0f / 255.0f, 210.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(lightgray)            , 9  , {{211.0f / 255.0f, 211.0f / 255.0f, 211.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(lightgreen)           , 10 , {{144.0f / 255.0f, 238.0f / 255.0f, 144.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(lightgrey)            , 9  , {{211.0f / 255.0f, 211.0f / 255.0f, 211.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(lightpink)            , 9  , {{255.0f / 255.0f, 182.0f / 255.0f, 193.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(lightsalmon)          , 11 , {{255.0f / 255.0f, 160.0f / 255.0f, 122.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(lightseagreen)        , 13 , {{ 32.0f / 255.0f, 178.0f / 255.0f, 170.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(lightskyblue)         , 12 , {{135.0f / 255.0f, 206.0f / 255.0f, 250.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(lightslategray)       , 14 , {{119.0f / 255.0f, 136.0f / 255.0f, 153.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(lightslategrey)       , 14 , {{119.0f / 255.0f, 136.0f / 255.0f, 153.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(lightsteelblue)       , 14 , {{176.0f / 255.0f, 196.0f / 255.0f, 222.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(lightyellow)          , 11 , {{255.0f / 255.0f, 255.0f / 255.0f, 224.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(lime)                 , 4  , {{  0.0f / 255.0f, 255.0f / 255.0f,   0.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(limegreen)            , 9  , {{ 50.0f / 255.0f, 205.0f / 255.0f,  50.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(linen)                , 5  , {{250.0f / 255.0f, 240.0f / 255.0f, 230.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(magenta)              , 7  , {{255.0f / 255.0f,   0.0f / 255.0f, 255.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(maroon)               , 6  , {{128.0f / 255.0f,   0.0f / 255.0f,   0.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(mediumaquamarine)     , 16 , {{102.0f / 255.0f, 205.0f / 255.0f, 170.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(mediumblue)           , 10 , {{  0.0f / 255.0f,   0.0f / 255.0f, 205.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(mediumorchid)         , 12 , {{186.0f / 255.0f,  85.0f / 255.0f, 211.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(mediumpurple)         , 12 , {{147.0f / 255.0f, 112.0f / 255.0f, 219.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(mediumseagreen)       , 14 , {{ 60.0f / 255.0f, 179.0f / 255.0f, 113.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(mediumslateblue)      , 15 , {{123.0f / 255.0f, 104.0f / 255.0f, 238.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(mediumspringgreen)    , 17 , {{  0.0f / 255.0f, 250.0f / 255.0f, 154.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(mediumturquoise)      , 15 , {{ 72.0f / 255.0f, 209.0f / 255.0f, 204.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(mediumvioletred)      , 15 , {{199.0f / 255.0f,  21.0f / 255.0f, 133.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(midnightblue)         , 12 , {{ 25.0f / 255.0f,  25.0f / 255.0f, 112.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(mintcream)            , 9  , {{245.0f / 255.0f, 255.0f / 255.0f, 250.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(mistyrose)            , 9  , {{255.0f / 255.0f, 228.0f / 255.0f, 225.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(moccasin)             , 8  , {{255.0f / 255.0f, 228.0f / 255.0f, 181.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(navajowhite)          , 11 , {{255.0f / 255.0f, 222.0f / 255.0f, 173.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(navy)                 , 4  , {{  0.0f / 255.0f,   0.0f / 255.0f, 128.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(oldlace)              , 7  , {{253.0f / 255.0f, 245.0f / 255.0f, 230.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(olive)                , 5  , {{128.0f / 255.0f, 128.0f / 255.0f,   0.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(olivedrab)            , 9  , {{107.0f / 255.0f, 142.0f / 255.0f,  35.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(orange)               , 6  , {{255.0f / 255.0f, 165.0f / 255.0f,   0.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(orangered)            , 9  , {{255.0f / 255.0f,  69.0f / 255.0f,   0.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(orchid)               , 6  , {{218.0f / 255.0f, 112.0f / 255.0f, 214.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(palegoldenrod)        , 13 , {{238.0f / 255.0f, 232.0f / 255.0f, 170.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(palegreen)            , 9  , {{152.0f / 255.0f, 251.0f / 255.0f, 152.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(paleturquoise)        , 13 , {{175.0f / 255.0f, 238.0f / 255.0f, 238.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(palevioletred)        , 13 , {{219.0f / 255.0f, 112.0f / 255.0f, 147.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(papayawhip)           , 10 , {{255.0f / 255.0f, 239.0f / 255.0f, 213.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(peachpuff)            , 9  , {{255.0f / 255.0f, 218.0f / 255.0f, 185.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(peru)                 , 4  , {{205.0f / 255.0f, 133.0f / 255.0f,  63.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(pink)                 , 4  , {{255.0f / 255.0f, 192.0f / 255.0f, 203.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(plum)                 , 4  , {{221.0f / 255.0f, 160.0f / 255.0f, 221.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(powderblue)           , 10 , {{176.0f / 255.0f, 224.0f / 255.0f, 230.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(purple)               , 6  , {{128.0f / 255.0f,   0.0f / 255.0f, 128.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(rebeccapurple)        , 13 , {{102.0f / 255.0f,  51.0f / 255.0f, 153.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(red)                  , 3  , {{255.0f / 255.0f,   0.0f / 255.0f,   0.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(rosybrown)            , 9  , {{188.0f / 255.0f, 143.0f / 255.0f, 143.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(royalblue)            , 9  , {{ 65.0f / 255.0f, 105.0f / 255.0f, 225.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(saddlebrown)          , 11 , {{139.0f / 255.0f,  69.0f / 255.0f,  19.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(salmon)               , 6  , {{250.0f / 255.0f, 128.0f / 255.0f, 114.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(sandybrown)           , 10 , {{244.0f / 255.0f, 164.0f / 255.0f,  96.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(seagreen)             , 8  , {{ 46.0f / 255.0f, 139.0f / 255.0f,  87.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(seashell)             , 8  , {{255.0f / 255.0f, 245.0f / 255.0f, 238.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(sienna)               , 6  , {{160.0f / 255.0f,  82.0f / 255.0f,  45.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(silver)               , 6  , {{192.0f / 255.0f, 192.0f / 255.0f, 192.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(skyblue)              , 7  , {{135.0f / 255.0f, 206.0f / 255.0f, 235.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(slateblue)            , 9  , {{106.0f / 255.0f,  90.0f / 255.0f, 205.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(slategray)            , 9  , {{112.0f / 255.0f, 128.0f / 255.0f, 144.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(slategrey)            , 9  , {{112.0f / 255.0f, 128.0f / 255.0f, 144.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(snow)                 , 4  , {{255.0f / 255.0f, 250.0f / 255.0f, 250.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(springgreen)          , 11 , {{  0.0f / 255.0f, 255.0f / 255.0f, 127.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(steelblue)            , 9  , {{ 70.0f / 255.0f, 130.0f / 255.0f, 180.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(tan)                  , 3  , {{210.0f / 255.0f, 180.0f / 255.0f, 140.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(teal)                 , 4  , {{  0.0f / 255.0f, 128.0f / 255.0f, 128.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(thistle)              , 7  , {{216.0f / 255.0f, 191.0f / 255.0f, 216.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(tomato)               , 6  , {{255.0f / 255.0f,  99.0f / 255.0f,  71.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(turquoise)            , 9  , {{ 64.0f / 255.0f, 224.0f / 255.0f, 208.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(violet)               , 6  , {{238.0f / 255.0f, 130.0f / 255.0f, 238.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(wheat)                , 5  , {{245.0f / 255.0f, 222.0f / 255.0f, 179.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(white)                , 5  , {{255.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(whitesmoke)           , 10 , {{245.0f / 255.0f, 245.0f / 255.0f, 245.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(yellow)               , 6  , {{255.0f / 255.0f, 255.0f / 255.0f,   0.0f / 255.0f, 1.0f}}},
    CSSColorInfo{SVG_TO_C_CHAR(yellowgreen)          , 11 , {{154.0f / 255.0f, 205.0f / 255.0f,  50.0f / 255.0f, 1.0f}}}
}};

}
