#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <tuple>
#include <string>

#include <librsvg/rsvg.h>

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
        filenames.push_back(filename);
    }
    filenames_file.close();

    std::vector<std::tuple<float, float, float, float>> bounds;
    for(auto const& file_name: filenames)
    {
        GError *error = NULL;
        RsvgHandle *handle = rsvg_handle_new_from_file(("../bound-tests-svgs/" + file_name).c_str(), &error);
        if (error)
        {
            std::cout << "Trouble opening the SVG document in librsvg" << std::endl;
            std::cout << error->message << std::endl;
            return -1;
        }
        RsvgRectangle ink_rectangle;
        rsvg_handle_get_geometry_for_element(handle, NULL, &ink_rectangle, NULL, &error);
        if (error)
        {
            std::cout << "Couldn't compute the bounds" << std::endl;
            return -1;
        }
        auto tup = std::tuple<float, float, float, float>(ink_rectangle.x, ink_rectangle.y, ink_rectangle.width, ink_rectangle.height);
        bounds.push_back(tup);
        g_object_unref(handle);
    }
    std::fstream bounds_file;
    bounds_file.open("../svg-bounds.txt", std::ios::out);
    if (!bounds_file)
    {
        std::cout << "Couldn't open the file tot write the bounds to" << std::endl;
        return -1;
    }
    for(auto const& bound: bounds)
    {
        bounds_file << std::setprecision(15) << std::get<0>(bound) << " " << std::get<1>(bound) << " " << std::get<2>(bound) << " " << std::get<3>(bound) << std::endl;
    }
    bounds_file.close();
    return 0;
}
