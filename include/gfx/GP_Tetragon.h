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
 * Copyright (C) 2009-2011 Jiri "BlueBear" Dluhos                            *
 *                         <jiri.bluebear.dluhos@gmail.com>                  *
 *                                                                           *
 * Copyright (C) 2009-2011 Cyril Hrubis <metan@ucw.cz>                       *
 *                                                                           *
 *****************************************************************************/

#ifndef GP_TETRAGON_H
#define GP_TETRAGON_H

#include "core/GP_Pixmap.h"

/* Tetragon */

void GP_Tetragon(GP_Pixmap *pixmap, GP_Coord x0, GP_Coord y0,
                 GP_Coord x1, GP_Coord y1, GP_Coord x2, GP_Coord y2,
                 GP_Coord x3, GP_Coord y3, GP_Pixel pixel);

void GP_Tetragon_Raw(GP_Pixmap *pixmap, GP_Coord x0, GP_Coord y0,
                     GP_Coord x1, GP_Coord y1, GP_Coord x2, GP_Coord y2,
                     GP_Coord x3, GP_Coord y3, GP_Pixel pixel);

/* Filled Tetragon */

void GP_FillTetragon(GP_Pixmap *pixmap, GP_Coord x0, GP_Coord y0,
                     GP_Coord x1, GP_Coord y1, GP_Coord x2, GP_Coord y2,
		     GP_Coord x3, GP_Coord y3, GP_Pixel pixel);

void GP_FillTetragon_Raw(GP_Pixmap *pixmap, GP_Coord x0, GP_Coord y0,
                         GP_Coord x1, GP_Coord y1, GP_Coord x2, GP_Coord y2,
                         GP_Coord x3, GP_Coord y3, GP_Pixel pixel);

#endif /* GP_TETRAGON_H */
