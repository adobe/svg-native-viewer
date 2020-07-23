/*
Copyright 2019 Adobe.
Copyright 2019 suzuki toshiya <mpsuzuki@hiroshima-u.ac.jp>.
All rights reserved.

This file is licensed to you under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License. You may obtain a copy
of the License at http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under
the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR REPRESENTATIONS
OF ANY KIND, either express or implied. See the License for the specific language
governing permissions and limitations under the License.
*/

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>

#include "svgnative/SVGNativeCWrapper.h"

char* read_svg_input(const char* pathname)
{
    char*   buff_input;
    FILE*   file_input;
    size_t  size_input;

    file_input = fopen(pathname, "r");
    if (!file_input)
    {
        printf("Error! Could not open input file.");
        exit(EXIT_FAILURE);
    }

    fseek(file_input, 0, SEEK_END);
    size_input = ftell(file_input);
    fseek(file_input, 0, SEEK_SET);
    buff_input = malloc(size_input + 1);
    if (!buff_input)
    {
        printf("Error! Cannot allocate sufficient memory to read input SVG.");
        exit(EXIT_FAILURE);
    }
    fread(buff_input, size_input, 1, file_input);
    fclose(file_input);

    return buff_input;
}

size_t write_data(const char* pathname, const char* buff_output, size_t size_output)
{
    FILE*   file_output;
    size_t  size_written;

    file_output = fopen(pathname, "w+");
    if (!file_output)
    {
        printf("Error! Could not open output file.");
        exit(EXIT_FAILURE);
    }

    size_written = fwrite(buff_output, size_output, 1, file_output);
    fclose(file_output);
    return size_written;
}

int main(int argc, const char* argv[])
{
    svg_native_t*  sn;
    svg_native_color_map_t*  colorMap;

    char*   buff_input;
    char*   buff_output;
    size_t  size_output;

    if (argc != 3)
    {
        printf("Incorrect number of arguments.");
        return 0;
    }
    buff_input = read_svg_input(argv[1]);

    colorMap = svg_native_color_map_create();
    svg_native_color_map_add(colorMap, "test-red",   0.502,   0.0, 0.0, 1.0);
    svg_native_color_map_add(colorMap, "test-green",   0.0, 0.502, 0.0, 1.0);
    svg_native_color_map_add(colorMap, "test-blue",    0.0,   0.0, 1.0, 1.0);

    sn = svg_native_create(SVG_RENDERER_STRING, buff_input);
    svg_native_set_color_map(sn, colorMap);

    svg_native_render(sn);

    svg_native_get_output(sn, &buff_output, &size_output);
    write_data(argv[2], buff_output, size_output);
    free(buff_output);

    svg_native_destroy(sn);
    svg_native_color_map_destroy(colorMap);

    return 0;
}
