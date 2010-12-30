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

#ifndef GP_TEXT_METRIC_H
#define GP_TEXT_METRIC_H

#include "GP_TextStyle.h"

/*
 * Calculates a pointer to the start of the character data
 * of the specified character.
 */
const uint8_t *GP_GetCharData(const GP_TextStyle *style, char c);

/*
 * Calculates the width of the string drawn in the given style, in pixels.
 */
unsigned int GP_TextWidth(const GP_TextStyle *style, const char *str);

/*
 * Returns maximal text height, in pixels.
 */
unsigned int GP_TextHeight(const GP_TextStyle *style);

/* Returns the ascent (height from the baseline to the top of characters),
 * for the given text style. (Result is in pixels.)
 */
unsigned int GP_TextAscent(const GP_TextStyle *style);

/* Returns the descent (height from the baseline to the bottom of characters),
 * for the given text style. (Result is in pixels.)
 */
unsigned int GP_TextDescent(const GP_TextStyle *style);

#endif /* GP_TEXT_METRIC_H */
