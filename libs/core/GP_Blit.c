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
 * Copyright (C) 2011      Tomas Gavenciak <gavento@ucw.cz>                  *
 * Copyright (C) 2011-2013 Cyril Hrubis    <metan@ucw.cz>                    *
 *                                                                           *
 *****************************************************************************/

#include "core/GP_Pixel.h"
#include "core/GP_GetPutPixel.h"
#include "core/GP_Pixmap.h"
#include "core/GP_Convert.h"
#include "core/GP_Debug.h"
#include "core/GP_Blit.h"

/* Generated functions */
void GP_BlitXYXY_Raw_Fast(const GP_Pixmap *src,
                          GP_Coord x0, GP_Coord y0, GP_Coord x1, GP_Coord y1,
                          GP_Pixmap *dst, GP_Coord x2, GP_Coord y2);

void GP_BlitXYXY_Fast(const GP_Pixmap *src,
                      GP_Coord x0, GP_Coord y0, GP_Coord x1, GP_Coord y1,
                      GP_Pixmap *dst, GP_Coord x2, GP_Coord y2);

void GP_BlitXYXY(const GP_Pixmap *src,
                 GP_Coord x0, GP_Coord y0, GP_Coord x1, GP_Coord y1,
                 GP_Pixmap *dst, GP_Coord x2, GP_Coord y2)
{
	/* Normalize source rectangle */
	if (x1 < x0)
		GP_SWAP(x0, x1);

	if (y1 < y0)
		GP_SWAP(y0, y1);

	/* All coordinates are inside of src the pixmap */
	GP_CHECK(x0 < (GP_Coord)GP_PixmapW(src));
	GP_CHECK(y0 < (GP_Coord)GP_PixmapH(src));
	GP_CHECK(x1 < (GP_Coord)GP_PixmapW(src));
	GP_CHECK(y1 < (GP_Coord)GP_PixmapH(src));

	/* Destination is big enough */
	GP_CHECK(x2 + (x1 - x0) < (GP_Coord)GP_PixmapW(dst));
	GP_CHECK(y2 + (y1 - y0) < (GP_Coord)GP_PixmapH(dst));

	GP_BlitXYXY_Fast(src, x0, y0, x1, y1, dst, x2, y2);
}

void GP_BlitXYXY_Clipped(const GP_Pixmap *src,
                         GP_Coord x0, GP_Coord y0, GP_Coord x1, GP_Coord y1,
                         GP_Pixmap *dst, GP_Coord x2, GP_Coord y2)
{
	/* Normalize source rectangle */
	if (x1 < x0)
		GP_SWAP(x0, x1);

	if (y1 < y0)
		GP_SWAP(y0, y1);

	/*
	 * Handle all cases where at least one of dest coordinates are out of
	 * the dest in positive direction -> src is out of dst completly.
	 */
	if (x2 >= (GP_Coord)GP_PixmapW(dst) ||
	    y2 >= (GP_Coord)GP_PixmapH(dst))
		return;

	/*
	 * The coordinates in dest are negative.
	 *
	 * We need to clip the source upper left corner accordingly.
	 *
	 * Notice that x2 and y2 are inside the dst rectangle now.
	 * (>= 0 and < w, < h)
	 */
	if (x2 < 0) {
		x0 -= x2;
		x2 = 0;
	}

	if (y2 < 0) {
		y0 -= y2;
		y2 = 0;
	}

	/* Make sure souce coordinates are inside of the src */
	x0 = GP_MAX(x0, 0);
	y0 = GP_MAX(y0, 0);
	x1 = GP_MIN(x1, (GP_Coord)GP_PixmapW(src) - 1);
	y1 = GP_MIN(y1, (GP_Coord)GP_PixmapH(src) - 1);

	/* And source rectangle fits inside of the destination */
	GP_Coord src_w = x1 - x0 + 1;
	GP_Coord src_h = y1 - y0 + 1;

	GP_Coord dst_w = GP_PixmapW(dst) - x2;
	GP_Coord dst_h = GP_PixmapH(dst) - y2;

	GP_DEBUG(2, "Blitting %ix%i, available %ix%i",
	         src_w, src_h, dst_w, dst_h);

	if (src_w > dst_w)
		x1 -= src_w - dst_w;

	if (src_h > dst_h)
		y1 -= src_h - dst_h;

	GP_DEBUG(2, "Blitting %ix%i->%ix%i in %ux%u to %ix%i in %ux%u",
	         x0, y0, x1, y1, GP_PixmapW(src), GP_PixmapH(src),
		 x2, y2, GP_PixmapW(dst), GP_PixmapH(dst));

	GP_BlitXYXY_Fast(src, x0, y0, x1, y1, dst, x2, y2);
}

void GP_BlitXYWH(const GP_Pixmap *src,
                 GP_Coord x0, GP_Coord y0, GP_Size w0, GP_Size h0,
                 GP_Pixmap *dst, GP_Coord x1, GP_Coord y1)
{
	if (w0 == 0 || h0 == 0)
		return;

	GP_BlitXYXY(src, x0, y0, x0 + w0 - 1, y0 + h0 - 1, dst, x1, y1);
}

void GP_BlitXYWH_Clipped(const GP_Pixmap *src,
                         GP_Coord x0, GP_Coord y0, GP_Size w0, GP_Size h0,
                         GP_Pixmap *dst, GP_Coord x1, GP_Coord y1)
{
	if (w0 == 0 || h0 == 0)
		return;

	GP_BlitXYXY_Clipped(src, x0, y0, x0 + w0 - 1, y0 + h0 - 1, dst, x1, y1);
}

void GP_BlitXYXY_Raw(const GP_Pixmap *src,
                     GP_Coord x0, GP_Coord y0, GP_Coord x1, GP_Coord y1,
                     GP_Pixmap *dst, GP_Coord x2, GP_Coord y2)
{
	/* Normalize source rectangle */
	if (x1 < x0)
		GP_SWAP(x0, x1);

	if (y1 < y0)
		GP_SWAP(y0, y1);

	/* All coordinates are inside of src the pixmap */
	GP_CHECK(x0 < (GP_Coord)src->w);
	GP_CHECK(y0 < (GP_Coord)src->h);
	GP_CHECK(x1 < (GP_Coord)src->w);
	GP_CHECK(y1 < (GP_Coord)src->h);

	/* Destination is big enough */
	GP_CHECK(x2 + (x1 - x0) < (GP_Coord)dst->w);
	GP_CHECK(y2 + (y1 - y0) < (GP_Coord)dst->h);

	GP_BlitXYXY_Raw_Fast(src, x0, y0, x1, y1, dst, x2, y2);
}

void GP_BlitXYWH_Raw(const GP_Pixmap *src,
                     GP_Coord x0, GP_Coord y0, GP_Size w0, GP_Size h0,
                     GP_Pixmap *dst, GP_Coord x2, GP_Coord y2)
{
	if (w0 == 0 || h0 == 0)
		return;

	GP_BlitXYXY_Raw(src, x0, y0, x0 + w0 - 1, y0 + h0 - 1, dst, x2, y2);
}
