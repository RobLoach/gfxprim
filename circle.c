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
 * Copyright (C) 2009-2010 Jiri "BlueBear" Dluhos                            *
 *                         <jiri.bluebear.dluhos@gmail.com>                  *
 *                                                                           *
 * Copyright (C) 2009-2010 Cyril Hrubis <metan@ucw.cz>                       *
 *                                                                           *
 *****************************************************************************/

#include "GP_backend.h"
#include "GP_pixel.h"
#include "GP_line.h"
#include "GP_gfx.h"

void GP_Circle(GP_TARGET_TYPE *target, long color, int xcenter, int ycenter, int r)
{
	if (target == NULL || GP_PIXELS(target) == NULL)
		return;

	switch (GP_BYTES_PER_PIXEL(target)) {
	case 1:
		GP_Circle_8bpp(target, color, xcenter, ycenter, r);
		break;
	case 2:
		GP_Circle_16bpp(target, color, xcenter, ycenter, r);
		break;
	case 3:
		GP_Circle_24bpp(target, color, xcenter, ycenter, r);
		break;
	case 4:
		GP_Circle_32bpp(target, color, xcenter, ycenter, r);
		break;
	}
}

/* Build specialized versions of GP_Circle() for specific bit depths. */

#define FN_NAME		GP_Circle_8bpp
#define SETPIXEL	GP_SetPixel_8bpp
#include "generic/circle_generic.c"
#undef SETPIXEL
#undef FN_NAME

#define FN_NAME		GP_Circle_16bpp
#define SETPIXEL	GP_SetPixel_16bpp
#include "generic/circle_generic.c"
#undef SETPIXEL
#undef FN_NAME

#define FN_NAME		GP_Circle_24bpp
#define SETPIXEL	GP_SetPixel_24bpp
#include "generic/circle_generic.c"
#undef SETPIXEL
#undef FN_NAME

#define FN_NAME		GP_Circle_32bpp
#define SETPIXEL	GP_SetPixel_32bpp
#include "generic/circle_generic.c"
#undef SETPIXEL
#undef FN_NAME

void GP_FillCircle(SDL_Surface *surf, long color, int xcenter, int ycenter, int r)
{
	if (surf == NULL || surf->pixels == NULL)
		return;
	if (r < 0)
		return;

	/*
	 * Draw the circle in top-down, line-per-line manner.
	 * For each line, X is calculated and a horizontal line is drawn
	 * between +X and -X, and reflected around the Y axis.
	 * X is computed in the same way as for GP_Circle().
	 */
	int x, y, error;
	for (x = 0, error = -r, y = r; y >= 0; y--) {
		while (error < 0) {
			error += 2*x + 1;
			x++;
		}
		error += -2*y + 1;

		GP_HLine(surf, color, xcenter-x+1, xcenter+x-1, ycenter-y);
		GP_HLine(surf, color, xcenter-x+1, xcenter+x-1, ycenter+y);
	}
}

