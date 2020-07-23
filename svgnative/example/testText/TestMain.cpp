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

#include "svgnative/SVGDocument.h"
#include "svgnative/ports/string/StringSVGRenderer.h"

#include <fstream>
#include <iostream>
#include <string>

int main(int argc, char* const argv[])
{
    if (argc != 3 && argc != 4)
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

    SVGNative::ColorMap colorMap = {
        {"test-red",   {{0.502,   0.0, 0.0, 1.0}}},
        {"test-green", {{  0.0, 0.502, 0.0, 1.0}}},
        {"test-blue",  {{  0.0,   0.0, 1.0, 1.0}}}
    };
    auto renderer = std::make_shared<SVGNative::StringSVGRenderer>();

    auto doc = std::unique_ptr<SVGNative::SVGDocument>(SVGNative::SVGDocument::CreateSVGDocument(svgInput.c_str(), renderer));
    if (!doc)
    {
        std::cout << "Error! Could not parse document." << std::endl;
        exit(EXIT_FAILURE);
    }
    if (argc == 3)
        doc->Render(colorMap);
    else
    {
        std::string id{argv[3]};
        doc->Render(id.c_str(), colorMap);
    }

    std::fstream outputStream;
    outputStream.open(argv[2], std::fstream::out);
    if (!outputStream)
    {
        std::cout << "Error! Could not write file." << std::endl;
        exit(EXIT_FAILURE);
    }
    std::string outputString{renderer->String()};
    outputStream << outputString;
    outputStream.close();

    return 0;
}
