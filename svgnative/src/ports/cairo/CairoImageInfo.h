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

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>
#include "cairo.h"

cairo_surface_t *
_cairo_image_surface_create_from_jpeg_stream(const unsigned char* data,
                                             unsigned int length);

typedef struct _png_blob_closure
{
    const unsigned char*  blob;
    size_t                cur_pos;
    size_t                limit;
} _png_blob_closure_t;

cairo_status_t
_png_blob_read_func(void           *closure,
                    unsigned char  *data,
                    unsigned int    length);

#ifdef __cplusplus
}
#endif
