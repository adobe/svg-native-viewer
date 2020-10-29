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

/* FIXME: this C source should be compiled by the compiler supporting C99 or later,
 *        due to some variable declarations in the middle of the functions
 */

#include "CairoImageInfo.h"
#include <string.h>

/* JPEG (image/jpeg)
 */
#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include <jpeglib.h>

struct _cairo_jpeg_error_mgr
{
    struct jpeg_error_mgr pub;
    jmp_buf setjmp_buffer;
};

typedef struct _cairo_jpeg_error_mgr * _cairo_jpeg_error_ptr;

METHODDEF(void)
_cairo_jpeg_error_exit(j_common_ptr cinfo)
{
    _cairo_jpeg_error_ptr _cairo_jpeg_err = (_cairo_jpeg_error_ptr)cinfo->err;
    (*cinfo->err->output_message)(cinfo);
    longjmp(_cairo_jpeg_err->setjmp_buffer, 1);
}

cairo_surface_t *
_cairo_image_surface_create_from_jpeg_stream(const unsigned char* data,
                                             unsigned int length)
{
    cairo_surface_t* _cairo_jpeg_surface = NULL;

    struct jpeg_decompress_struct cinfo;
    struct _cairo_jpeg_error_mgr jerr;
    cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = _cairo_jpeg_error_exit;

    unsigned char* outBuff = NULL;
    if (setjmp(jerr.setjmp_buffer))
    {
        jpeg_destroy_decompress(&cinfo);
        if (_cairo_jpeg_surface)
        {
            cairo_surface_destroy(_cairo_jpeg_surface);
        };
        return NULL;
    };

    jpeg_create_decompress(&cinfo);
    jpeg_mem_src(&cinfo, data, length);
    jpeg_read_header(&cinfo, TRUE);

    cairo_format_t cairo_color_format;
    switch (cinfo.out_color_space)
    {
    case JCS_GRAYSCALE:
        /* Cairo has no special format for grayscale, we must use RGB24 */
        cairo_color_format = CAIRO_FORMAT_RGB24;
        break;
#ifdef JCS_EXT_ARGB
    case JCS_EXT_ARGB:
        cairo_color_format = CAIRO_FORMAT_ARGB32;
        break;
#endif
    case JCS_RGB:
    default:
        cairo_color_format = CAIRO_FORMAT_RGB24;
    };

    /* cinfo.image_width, cinfo.image_height, cinfo.num_components are already filled, but
     * cinfo.output_width, cinfo.output_height, cinfo.output_components are not, because
     * they are output parameters
     */
    jpeg_start_decompress(&cinfo);

    int jpeg_row_stride = cinfo.output_width * cinfo.output_components;
    int cairo_row_stride = cairo_format_stride_for_width(cairo_color_format, cinfo.output_width);

    size_t outLimit = cairo_row_stride * cinfo.output_height;
    outBuff = (unsigned char*)malloc(outLimit);
    bzero(outBuff, outLimit);
    size_t outCur = 0;

    JSAMPARRAY buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr)&cinfo, JPOOL_IMAGE, jpeg_row_stride, 1);

    while (cinfo.output_scanline < cinfo.output_height)
    {
        jpeg_read_scanlines(&cinfo, buffer, 1);
        for (int ipxl = 0; ipxl < cinfo.output_width; ipxl++)
        {
            size_t jpeg_buff_offset = (ipxl * cinfo.output_components); 
            unsigned long rgb = 0;
            for (int iclr = 0; iclr < cinfo.output_components; iclr++)
                rgb = (rgb << 8) | buffer[0][jpeg_buff_offset + iclr];

            /* Assume single component image is grayscale */
            if (cinfo.output_components == 1)
                ((uint32_t*)(outBuff + outCur))[ipxl] = (rgb << 16 | rgb << 8 | rgb);
            else
                ((uint32_t*)(outBuff + outCur))[ipxl] = rgb;
        }
        outCur += cairo_row_stride;
    };
    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);

    _cairo_jpeg_surface = cairo_image_surface_create_for_data(outBuff, cairo_color_format, cinfo.output_width, cinfo.output_height, cairo_row_stride);

    /* transfer ownership of pixmap buffer from this caller to the surface,
     * and let the surface free it when the owner surface is being destroyed.
     */
    cairo_surface_set_mime_data(_cairo_jpeg_surface, "image/x-pixmap", outBuff, outLimit, free, (void*)outBuff);

    return _cairo_jpeg_surface;
}

/* PNG (image/png)
 */

cairo_status_t
_png_blob_read_func(void           *closure,
                    unsigned char  *data,
                    unsigned int    length)
{
    _png_blob_closure_t  *png_blob_closure = (_png_blob_closure_t*)closure;
    
    if (png_blob_closure->limit <= png_blob_closure->cur_pos)
        return CAIRO_STATUS_READ_ERROR;

    if (png_blob_closure->limit <= png_blob_closure->cur_pos + length)
    {
        memset(data, 0, length); 
        length = png_blob_closure->limit - png_blob_closure->cur_pos;
    }

    memcpy(data, png_blob_closure->blob + png_blob_closure->cur_pos, length); 
    png_blob_closure->cur_pos += length;
    return CAIRO_STATUS_SUCCESS;
}
