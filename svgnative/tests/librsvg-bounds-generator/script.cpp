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

#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <tuple>
#include <string>

#include <librsvg/rsvg.h>
#include <cairo.h>

int main(void)
{
    std::fstream filenames_file;
    filenames_file.open("../svg-filenames.txt", std::ios::in);
    if (!filenames_file)
    {
        std::cout << "Couldn't open the file with all svg filenames" << std::endl;
        return -1;
    }
    std::vector<std::string> filenames;
    std::string filename;
    while(std::getline(filenames_file, filename))
    {
        if (filename != "")
            filenames.push_back(filename);
    }
    filenames_file.close();

    std::vector<std::tuple<float, float, float, float>> bounds;
    for(auto const& line: filenames)
    {
        bool bounds_of_group = false;
        auto loc = line.find_first_of(",");
        std::string file_name = line;
        std::string id;
        if (loc != std::string::npos)
        {
            file_name = line.substr(0, loc);
            id = line.substr(loc + 1, std::string::npos);
            bounds_of_group = true;
        }

        GError *error = NULL;
        RsvgHandle *handle = rsvg_handle_new_from_file(("../bound-tests-svgs/" + file_name).c_str(), &error);
        if (error)
        {
            std::cout << "Trouble opening the SVG document in librsvg" << std::endl;
            std::cout << error->message << std::endl;
            return -1;
        }
        cairo_surface_t *surface = cairo_recording_surface_create(CAIRO_CONTENT_COLOR_ALPHA, NULL);
        cairo_t *ct = cairo_create(surface);
        if (bounds_of_group)
            rsvg_handle_render_cairo_sub(handle, ct, id.c_str());
        else
            rsvg_handle_render_cairo(handle, ct);
        cairo_surface_flush(surface);
        double x, y, width, height;
        cairo_recording_surface_ink_extents(surface, &x, &y, &width, &height);
        cairo_destroy(ct);
        cairo_surface_destroy(surface);
        if (error)
        {
            std::cout << "Couldn't compute the bounds" << std::endl;
            return -1;
        }
        auto tup = std::tuple<float, float, float, float>(x, y, width, height);
        bounds.push_back(tup);
        g_object_unref(handle);
    }
    std::fstream bounds_file;
    bounds_file.open("../svg-bounds.txt", std::ios::out);
    if (!bounds_file)
    {
        std::cout << "Couldn't open the file to write the bounds to" << std::endl;
        return -1;
    }
    for(auto const& bound: bounds)
    {
        bounds_file << std::setprecision(15) << std::get<0>(bound) << "," << std::get<1>(bound) << "," << std::get<2>(bound) << "," << std::get<3>(bound) << std::endl;
    }
    bounds_file.close();
    return 0;
}
