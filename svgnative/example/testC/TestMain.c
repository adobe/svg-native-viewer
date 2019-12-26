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

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>

#include "SVGNativeCWrapper.h"

char* read_svg_input(char* pathname)
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

size_t write_data(char* pathname, char* buff_output, size_t size_output)
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

int main(int argc, char* const argv[])
{
    char*   buff_input;
    char*   buff_output;
    size_t  size_output;


    if (argc != 3)
    {
        printf("Incorrect number of arguments.");
        return 0;
    }
    buff_input = read_svg_input(argv[1]);

    svg_native_color_map_t* colorMap = svg_native_color_map_create();
    svg_native_color_map_add(colorMap, "test-red",   0.502,   0.0, 0.0, 1.0);
    svg_native_color_map_add(colorMap, "test-green",   0.0, 0.502, 0.0, 1.0);
    svg_native_color_map_add(colorMap, "test-blue",    0.0,   0.0, 1.0, 1.0);

    svg_native_t* sn = svg_native_create(SVG_RENDERER_STRING, buff_input);
    svg_native_set_color_map(sn, colorMap);

    svg_native_render(sn);
    size_output = svg_native_render_output(sn, NULL, 0);
    buff_output = malloc(size_output + 1);
    svg_native_render_output(sn, buff_output, size_output);
    write_data(argv[2], buff_output, size_output);

    svg_native_destroy(sn);
    svg_native_color_map_destroy(colorMap);

    return 0;
}
