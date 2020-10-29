/*
Copyright 2019 suzuki toshiya <mpsuzuki@hiroshima-u.ac.jp>. All rights reserved.
This file is licensed to you under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License. You may obtain a copy
of the License at http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under
the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR REPRESENTATIONS
OF ANY KIND, either express or implied. See the License for the specific language
governing permissions and limitations under the License.
*/

#include "svgnative/SVGDocument.h"
#include <list>
#include "svgnative/ports/cairo/CairoSVGRenderer.h"
#include "cairo.h"

#if CAIRO_HAS_SCRIPT_SURFACE
#include "cairo-script.h"
#endif
#if CAIRO_HAS_SVG_SURFACE
#include "cairo-svg.h"
#endif
#if CAIRO_HAS_XML_SURFACE
#include "cairo-xml.h"
#endif

#include <fstream>
#include <iostream>
#include <string>
#include <cctype>

void shutdown_cairo_surface(cairo_surface_t* cairo_surface)
{
    cairo_surface_flush(cairo_surface);
    cairo_surface_finish(cairo_surface);
    cairo_surface_destroy(cairo_surface);
}

void shutdown_cairo_device(cairo_device_t* cairo_device)
{
    cairo_device_flush(cairo_device);
    cairo_device_finish(cairo_device);
    cairo_device_destroy(cairo_device);
}

bool try_emit_svg(const char* pathSvg, int width, int height, cairo_surface_t* cairoRecSurface)
{
    assert(pathSvg);
    assert(cairoRecSurface);

#ifndef CAIRO_HAS_SVG_SURFACE
    return false;
#else
    auto cairoSvgSurface = cairo_svg_surface_create(pathSvg, width, height);
    auto cairoSvgCtx = cairo_create(cairoSvgSurface);
    cairo_set_source_surface(cairoSvgCtx, cairoRecSurface, 0, 0);

    /* XXX: it seems that source surface should be destroyed before SVG painting
     *       to prevent leaks ???
     */
    shutdown_cairo_surface(cairoRecSurface);

    cairo_paint(cairoSvgCtx);
    cairo_show_page(cairoSvgCtx);
    cairo_destroy(cairoSvgCtx);
    shutdown_cairo_surface(cairoSvgSurface);

    return true;
#endif
}

bool try_emit_xml(const char* pathXml, cairo_surface_t* cairoRecSurface)
{
    assert(pathXml);
    assert(cairoRecSurface);

#ifndef CAIRO_HAS_XML_SURFACE
    return false;
#else
    auto cairoXmlDev = cairo_xml_create(pathXml);
    cairo_xml_for_recording_surface(cairoXmlDev, cairoRecSurface);
    shutdown_cairo_device(cairoXmlDev);

    return true;
#endif
}

bool try_emit_cairo_script(const char* pathCairoScript, cairo_surface_t* cairoRecSurface)
{
    assert(pathCairoScript);
    assert(cairoRecSurface);

#ifndef CAIRO_HAS_SCRIPT_SURFACE
    return false;
#else
    auto cairoScriptDev = cairo_script_create(pathCairoScript);
    cairo_script_from_recording_surface(cairoScriptDev, cairoRecSurface);
    shutdown_cairo_device(cairoScriptDev);

    return true;
#endif
}

bool try_emit_png(const char* pathPng, cairo_surface_t* cairoRecSurface)
{
    assert(pathPng);
    assert(cairoRecSurface);

#ifndef CAIRO_HAS_PNG_FUNCTIONS
    // TODO: rather than using shortcut PNG function, getting the pixel data and
    //       pack it in specified image format would be better to support various
    //       raster image formats, like JPEG and GIF.

    return false;
#else
    cairo_surface_write_to_png(cairoRecSurface, pathPng);

    return true;
#endif
}

int main(int argc, char* const argv[])
{
    if (argc < 3)
    {
        std::cerr << argv[0] << " <input.svg> <output1.png> [<output2.xml> <output3.cs> <output4.svg> ...]" << std::endl;
        return 0;
    }

    std::string svgInput{};
    std::ifstream input(argv[1]);
    if (!input)
    {
        std::cerr << "Error! Could not open input file." << std::endl;
        exit(EXIT_FAILURE);
    }
    for (std::string line; std::getline(input, line);)
        svgInput.append(line);
    input.close();

    auto renderer = std::make_shared<SVGNative::CairoSVGRenderer>();

    auto doc = std::unique_ptr<SVGNative::SVGDocument>(SVGNative::SVGDocument::CreateSVGDocument(svgInput.c_str(), renderer));

    cairo_rectangle_t docExtents{ 0, 0, 0, 0 };
    docExtents.width = doc->Width();
    docExtents.height = doc->Height();

    for (int i = 2; i < argc; i++ )
    {
        std::string outPath = argv[i];
        std::string suffix = outPath.substr(outPath.rfind('.') + 1).c_str();
        std::transform(suffix.begin(), suffix.end(), suffix.begin(), ::tolower);

        cairo_surface_t* cairoRecSurface = cairo_recording_surface_create(CAIRO_CONTENT_COLOR_ALPHA, &docExtents);

        auto cairoRecContext = cairo_create(cairoRecSurface);

        renderer->SetCairo(cairoRecContext);
        doc->Render();
        cairo_destroy(cairoRecContext);

        if (suffix == "svg" && try_emit_svg(outPath.c_str(), doc->Width(), doc->Height(), cairoRecSurface))
            continue;

        if (suffix == "xml" && try_emit_xml(outPath.c_str(), cairoRecSurface))
            continue;

        if (suffix == "cs" && try_emit_cairo_script(outPath.c_str(), cairoRecSurface))
            continue;

        if (suffix == "png" && try_emit_png(outPath.c_str(), cairoRecSurface))
            continue;

        std::cerr << "Specified format " << suffix << " does not match any output format available in linked Cairo" << std::endl;
        shutdown_cairo_surface(cairoRecSurface);
    }

    doc.reset();
    renderer.reset();

    return 0;
}
