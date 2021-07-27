#include <fstream>
#include <iostream>
#include <tuple>
#include <vector>

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
        std::getline(bounds_file, bounds_line);
        float x, y, width, height;
        sscanf(bounds_line.c_str(), "%f,%f,%f,%f", &x, &y, &width, &height);
        svg_documents_bounds.push_back(std::tuple<std::string, Rect>(filename, Rect{x, y, width, height}));
    }
    filenames_file.close();
    bounds_file.close();
    for(auto const& item: svg_documents_bounds)
    {
        std::string filename = std::get<0>(item);
        Rect standard_bounds = std::get<1>(item);
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
        Rect bounds = doc->Bounds();
        EXPECT_EQ(bounds.contains(standard_bounds), true);
        float width_ratio = std::min(bounds.width, standard_bounds.width)/std::max(bounds.width, standard_bounds.width);
        float height_ratio = std::min(bounds.height, standard_bounds.height)/std::max(bounds.height, standard_bounds.height);
        EXPECT_EQ(width_ratio > 0.70, true);
        EXPECT_EQ(height_ratio > 0.70, true);
    }
}
