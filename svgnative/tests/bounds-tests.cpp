/*
Copyright 2022 Adobe. All rights reserved.
This file is licensed to you under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License. You may obtain a copy
of the License at http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under
the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR REPRESENTATIONS
OF ANY KIND, either express or implied. See the License for the specific language
governing permissions and limitations under the License.
*/

#include <fstream>
#include <iostream>
#include <tuple>
#include <vector>
#include <cstdlib>
#include <cmath>

#include "gtest/gtest.h"
#include "gtest/gtest-spi.h"

#include <include/core/SkImage.h>
#include <include/core/SkPictureRecorder.h>
#include <include/core/SkSurface.h>

#include <svgnative/SVGRenderer.h>
#include <svgnative/SVGDocument.h>
#include <svgnative/ports/skia/SkiaSVGRenderer.h>

using namespace SVGNative;

TEST(bounds_tests, bounds_basic_test)
{
    auto surface = SkSurface::MakeNull(1000, 1000);
    auto canvas = surface->getCanvas();
    SkiaSVGRenderer renderer;
    renderer.SetSkCanvas(canvas);
    auto path = renderer.CreatePath();
    path->MoveTo(0, 0);
    path->LineTo(10, 10);
    path->LineTo(10, 0);
    path->ClosePath();
    Rect bounds = renderer.GetBounds(*path.get(), GraphicStyle{}, FillStyle{}, StrokeStyle{});
    EXPECT_EQ(bounds.x, 0);
    EXPECT_EQ(bounds.y, 0);
    EXPECT_EQ(bounds.width, 10);
    EXPECT_EQ(bounds.height, 10);
}

// TODO: Fix these file paths which are relative and dependent
// on the install location. Right now we assume it's build/**/
TEST(bounds_tests, bounds_functional_test)
{
    // get the surface and the canvas
    auto surface = SkSurface::MakeNull(1000, 1000);
    auto canvas = surface->getCanvas();
    auto renderer = std::make_shared<SVGNative::SkiaSVGRenderer>();
    renderer->SetSkCanvas(canvas);

    // load the filenames file
    std::fstream filenames_file;
    std::fstream bounds_file;
    filenames_file.open("../../tests/svg-filenames.txt");
    bounds_file.open("../../tests/svg-bounds.txt");
    if (!filenames_file || !bounds_file)
    {
        std::cout << "Error! Could not open input file." << std::endl;
        FAIL();
    }
    std::string filename;
    std::string bounds_line;
    std::vector<std::tuple<std::string, Rect>> svg_documents_bounds;
    while(std::getline(filenames_file, filename))
    {
        if (filename == "")
            break;
        std::getline(bounds_file, bounds_line);
        float x, y, width, height;
        sscanf(bounds_line.c_str(), "%f,%f,%f,%f", &x, &y, &width, &height);
        svg_documents_bounds.push_back(std::tuple<std::string, Rect>(filename, Rect{x, y, width, height}));
    }
    filenames_file.close();
    bounds_file.close();
    for(auto const& item: svg_documents_bounds)
    {
        std::string line = std::get<0>(item);
        Rect standard_bounds = std::get<1>(item);

        bool bounds_of_group = false;
        auto loc = line.find_first_of(",");
        std::string filename = line;
        std::string id;
        if (loc != std::string::npos)
        {
            filename = line.substr(0, loc);
            id = line.substr(loc + 2, std::string::npos);
            bounds_of_group = true;
        }

        std::string full_file_path = "../../tests/bound-tests-svgs/" + filename;
        std::string svgInput{};
        std::ifstream input(full_file_path);
        if (!input)
        {
            std::cout << "Error! Could not open input file." << std::endl;
            FAIL();
        }
        for (std::string line; std::getline(input, line);)
            svgInput.append(line);
        input.close();
        auto doc = std::unique_ptr<SVGNative::SVGDocument>(SVGNative::SVGDocument::CreateSVGDocument(svgInput.c_str(), renderer));
        Rect bounds;
        if (bounds_of_group)
            doc->GetBoundingBox(id.c_str(), bounds);
        else
            doc->GetBoundingBox(bounds);
        EXPECT_EQ((bounds.IsEmpty() && !standard_bounds.IsEmpty()) ||
                  (!bounds.IsEmpty() && standard_bounds.IsEmpty()), false);
        if (bounds.IsEmpty() && standard_bounds.IsEmpty())
        {
            printf("\033[32m%s, PASS because both empty \033[0m\n", filename.c_str());
            continue;
        }
        float diff = bounds.MaxDiffVertex(standard_bounds);
        //printf("diff: %f\n", diff);
        diff = diff / (std::max(standard_bounds.width, standard_bounds.height));
        if (diff < 0.1)
        {
            printf("\033[32m%s, %f, PASS\033[0m\n", line.c_str(), diff);
        }
        else
        {
            const int buffer = 2;
            bounds.x -= buffer;
            bounds.y -= buffer;
            bounds.width += buffer * 2;
            bounds.height += buffer * 2;
            if (bounds.Contains(standard_bounds))
            {
                printf("\033[35m%s, %f, PASS BECAUSE CONTAINS!\033[0m\n", line.c_str(), diff);
            }
            else
            {
                printf("\033[31m%s, %f, FAIL\033[0m\n", line.c_str(), diff);
                printf("standard: %f %f %f %f\n", standard_bounds.x, standard_bounds.y, standard_bounds.width, standard_bounds.height);
                printf("calculated: %f %f %f %f\n", bounds.x, bounds.y, bounds.width, bounds.height);
                FAIL();
            }
        }
    }
}
