/*************************************************************************
 * ADOBE CONFIDENTIAL
 * ___________________
 *
 * Copyright 2019 Adobe
 * All Rights Reserved.
 *
 * NOTICE: All information contained herein is, and remains
 * the property of Adobe and its suppliers,
 * if any. The intellectual and technical concepts contained
 * herein are proprietary to Adobe and its
 * suppliers and are protected by all applicable intellectual property
 * laws, including trade secret and copyright laws.
 * Dissemination of this information or reproduction of this material
 * is strictly forbidden unless prior written permission is obtained
 * from Adobe.
 **************************************************************************/

#include "SVGNativeCWrapper.h"
#include "StringSVGRenderer.h"

#include <fstream>
#include <iostream>
#include <string>

int main(int argc, char* const argv[])
{
    if (argc != 3)
    {
        std::cout << "Incorrect number of arguments." << std::endl;
        return 0;
    }

    std::string svgInput{};
    std::ifstream input(argv[1]);
    if (!input)
    {
        std::cout << "Error! Could not open input file." << std::endl;
        exit(EXIT_FAILURE);
    }
    for (std::string line; std::getline(input, line);)
        svgInput.append(line);
    input.close();

    svg_native_color_map_t* colorMap = svg_native_color_map_create();
    svg_native_color_map_add(colorMap, "test-red",   0.502,   0.0, 0.0, 1.0);
    svg_native_color_map_add(colorMap, "test-green",   0.0, 0.502, 0.0, 1.0);
    svg_native_color_map_add(colorMap, "test-blue",    0.0,   0.0, 1.0, 1.0);

    svg_native_t* sn = svg_native_create(SVG_RENDERER_STRING, svgInput.c_str());
    svg_native_set_color_map(sn, colorMap);

    svg_native_render(sn);

    std::fstream outputStream;
    outputStream.open(argv[2], std::fstream::out);
    if (!outputStream)
    {
        std::cout << "Error! Could not write file." << std::endl;
        exit(EXIT_FAILURE);
    }

    int length = 2048;
    char output[length];
    svg_native_render_output(sn, output, length);

    outputStream << output;
    outputStream.close();


    svg_native_destroy(sn);
    svg_native_color_map_destroy(colorMap);

    return 0;
}
