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

#ifdef __cplusplus
extern "C" {
#endif

typedef enum svg_native_renderer_type_t_ {
    SVG_RENDERER_UNKNOWN,
    SVG_RENDERER_CG,
    SVG_RENDERER_SKIA,
    SVG_RENDERER_STRING
} svg_native_renderer_type_t;

typedef struct svg_native_t_ svg_native_t;
typedef struct svg_native_color_map_t_ svg_native_color_map_t;
typedef void svg_native_renderer_t;

svg_native_color_map_t* svg_native_color_map_create();
void svg_native_color_map_add(svg_native_color_map_t* color_map, const char* color_key, float red, float green, float blue, float alpha);
void svg_native_color_map_destroy(svg_native_color_map_t* color_map);


svg_native_t* svg_native_create(svg_native_renderer_type_t renderer_type, const char* document_string);

void svg_native_set_color_map(svg_native_t* sn, svg_native_color_map_t* color_map);
void svg_native_set_renderer(svg_native_t* sn, svg_native_renderer_t* renderer);

float svg_native_canvas_width(svg_native_t* sn);
float svg_native_canvas_height(svg_native_t* sn);

void svg_native_render(svg_native_t* sn);
void svg_native_render_size(svg_native_t* sn, float width, float height);

#ifdef USE_TEXT
void svg_native_render_output(svg_native_t* sn, char* output, int& length);
#endif

void svg_native_destroy(svg_native_t*);

#ifdef __cplusplus
}
#endif
