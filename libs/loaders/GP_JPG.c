/*****************************************************************************
 * This file is part of gfxprim library.                                     *
 *                                                                           *
 * Gfxprim is free software; you can redistribute it and/or                  *
 * modify it under the terms of the GNU Lesser General Public                *
 * License as published by the Free Software Foundation; either              *
 * version 2.1 of the License, or (at your option) any later version.        *
 *                                                                           *
 * Gfxprim is distributed in the hope that it will be useful,                *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU         *
 * Lesser General Public License for more details.                           *
 *                                                                           *
 * You should have received a copy of the GNU Lesser General Public          *
 * License along with gfxprim; if not, write to the Free Software            *
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,                        *
 * Boston, MA  02110-1301  USA                                               *
 *                                                                           *
 * Copyright (C) 2009-2011 Cyril Hrubis <metan@ucw.cz>                       *
 *                                                                           *
 *****************************************************************************/

/*

  JPG image support using jpeg library.
  
 */

#include <stdint.h>
#include <inttypes.h>

#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <setjmp.h>

#include <jpeglib.h>

#include "GP_JPG.h"
#include "core/GP_Debug.h"

GP_RetCode GP_OpenJPG(const char *src_path, FILE **f)
{
	*f = fopen(src_path, "rb");

	if (*f == NULL) {
		GP_DEBUG(1, "Failed to open '%s' : %s",
		            src_path, strerror(errno));
		return GP_EBADFILE;
	}

	//TODO: check signature and rewind the stream
	
	return GP_ESUCCESS;
}

struct my_jpg_err {
	struct jpeg_error_mgr error_mgr;

	jmp_buf setjmp_buf;
};

static void my_error_exit(j_common_ptr cinfo)
{
	struct my_jpg_err *my_err = (struct my_jpg_err*) cinfo->err;

	GP_DEBUG(1, "ERROR reading/writing jpeg file");

	longjmp(my_err->setjmp_buf, 1);
}

static const char *get_colorspace(J_COLOR_SPACE color_space)
{
	switch (color_space) {
	case JCS_GRAYSCALE:
		return "Grayscale";
	case JCS_RGB:
		return "RGB";
	case JCS_YCbCr:
		return "YCbCr";
	case JCS_CMYK:
		return "CMYK";
	case JCS_YCCK:
		return "YCCK";
	default:
		return "Unknown";
	};
}

GP_RetCode GP_ReadJPG(FILE *f, GP_Context **res,
                      GP_ProgressCallback *callback)
{
	struct jpeg_decompress_struct cinfo;
	struct my_jpg_err my_err;
	GP_Context *ret = NULL;

	cinfo.err = jpeg_std_error(&my_err.error_mgr);
	my_err.error_mgr.error_exit = my_error_exit;

	if (setjmp(my_err.setjmp_buf)) {
		jpeg_destroy_decompress(&cinfo);
		GP_ContextFree(ret);
		fclose(f);
		return GP_EBADFILE;
	}

	jpeg_create_decompress(&cinfo);
	jpeg_stdio_src(&cinfo, f);

	jpeg_read_header(&cinfo, TRUE);

	GP_DEBUG(1, "Have %s JPEG size %ux%u %i channels",
	            get_colorspace(cinfo.jpeg_color_space),
	            cinfo.image_width, cinfo.image_height,
		    cinfo.num_components);

	GP_Pixel pixel_type;

	switch (cinfo.out_color_space) {
	case JCS_GRAYSCALE:
		pixel_type = GP_PIXEL_G8;
	break;
	case JCS_RGB:
		pixel_type = GP_PIXEL_RGB888;
	break;
	default:
		pixel_type = GP_PIXEL_UNKNOWN;
	}

	if (pixel_type == GP_PIXEL_UNKNOWN) {
		GP_DEBUG(1, "Can't handle %s JPEG output format",
		            get_colorspace(cinfo.out_color_space));
		jpeg_destroy_decompress(&cinfo);
		fclose(f);
		return GP_EBADFILE;
	}

	ret = GP_ContextAlloc(cinfo.image_width, cinfo.image_height,
	                      pixel_type);

	if (ret == NULL) {
		GP_DEBUG(1, "Malloc failed :(");
		jpeg_destroy_decompress(&cinfo);
		fclose(f);
		return GP_ENOMEM;
	}

	jpeg_start_decompress(&cinfo);

	while (cinfo.output_scanline < cinfo.output_height) {
		uint32_t y = cinfo.output_scanline;

		JSAMPROW addr = (void*)GP_PIXEL_ADDR(ret, 0, y);
		jpeg_read_scanlines(&cinfo, &addr, 1);

		if (pixel_type != GP_PIXEL_RGB888)
			continue;
		
		//TODO: fixme bigendian?
		/* fix the pixel, as we want in fact BGR */
		uint32_t i;
		
		for (i = 0; i < ret->w; i++) {
			uint8_t *pix = GP_PIXEL_ADDR(ret, i, y);
			GP_SWAP(pix[0], pix[2]);
		}
	
		if (GP_ProgressCallbackReport(callback, y, ret->h, ret->w)) {
			GP_DEBUG(1, "Operation aborted");
			jpeg_destroy_decompress(&cinfo);
			fclose(f);
			GP_ContextFree(ret);
			return GP_EINTR;
		}
	}

	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
	fclose(f);
	*res = ret;

	GP_ProgressCallbackDone(callback);
	return GP_ESUCCESS;
}

GP_RetCode GP_LoadJPG(const char *src_path, GP_Context **res,
                      GP_ProgressCallback *callback)
{
	FILE *f;
	GP_RetCode ret;

	if ((ret = GP_OpenJPG(src_path, &f)))
		return ret;

	return GP_ReadJPG(f, res, callback);
}

GP_RetCode GP_SaveJPG(const char *dst_path, const GP_Context *src,
                      GP_ProgressCallback *callback)
{
	FILE *f;
	GP_RetCode ret;
	struct jpeg_compress_struct cinfo;
	struct my_jpg_err my_err;

	if (src->pixel_type != GP_PIXEL_RGB888 &&
	    src->pixel_type != GP_PIXEL_G8) {
		GP_DEBUG(1, "Can't save png with pixel type %s",
		         GP_PixelTypeName(src->pixel_type));
		return GP_ENOIMPL;
	}

	f = fopen(dst_path, "wb");

	if (f == NULL) {
		GP_DEBUG(1, "Failed to open '%s' for writing: %s",
		         dst_path, strerror(errno));
		return GP_EBADFILE;
	}
	
	if (setjmp(my_err.setjmp_buf)) {
		ret = GP_EBADFILE;;
		goto err1;
	}

	cinfo.err = jpeg_std_error(&my_err.error_mgr);
	my_err.error_mgr.error_exit = my_error_exit;
	
	jpeg_create_compress(&cinfo);

	jpeg_stdio_dest(&cinfo, f);

	cinfo.image_width  = src->w;
	cinfo.image_height = src->h;
	cinfo.input_components = src->pixel_type == GP_PIXEL_RGB888 ? 3 : 1;
	cinfo.in_color_space = JCS_RGB;

	jpeg_set_defaults(&cinfo);

	jpeg_start_compress(&cinfo, TRUE);

	while (cinfo.next_scanline < cinfo.image_height) {
		uint32_t y = cinfo.next_scanline;

		if (src->pixel_type == GP_PIXEL_RGB888) {
			uint32_t i;
			uint8_t tmp[3 * src->w];

			memcpy(tmp, GP_PIXEL_ADDR(src, 0, y), 3 * src->w);

			/* fix the pixels as we want in fact BGR */
			for (i = 0; i < src->w; i++) {
				uint8_t *pix = tmp + 3 * i; 
				GP_SWAP(pix[0], pix[2]);
			}

			JSAMPROW row = (void*)tmp;
			jpeg_write_scanlines(&cinfo, &row, 1);
		} else {
			JSAMPROW row = (void*)GP_PIXEL_ADDR(src, 0, y);
			jpeg_write_scanlines(&cinfo, &row, 1);
		}
	
		if (GP_ProgressCallbackReport(callback, y, src->h, src->w)) {
			GP_DEBUG(1, "Operation aborted");
			ret = GP_EINTR;
			goto err1;
		}
	}

	jpeg_finish_compress(&cinfo);

	if (fclose(f)) {
		GP_DEBUG(1, "Failed to close file '%s': %s",
		         dst_path, strerror(errno));
		return GP_EBADFILE;
	}

	return GP_ESUCCESS;
//TODO: is cinfo allocated?
err1:
	jpeg_destroy_compress(&cinfo);
	fclose(f);
	unlink(dst_path);
	return ret;
}
