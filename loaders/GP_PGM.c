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

/*

  PGM portable graymap loader/saver.

  Format:
  
  a magick number value of 'P' and '2'
  whitespace (blanks, TABs, CRs, LFs).
  ascii width
  whitespace
  ascii height
  whitespace
  maximal gray value (interval is 0 ... max)
  width * height ascii gray values 
  
  lines starting with '#' are comments to the end of line
  
 */

#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

#include "GP_PXMCommon.h"
#include "GP_PGM.h"

GP_RetCode GP_LoadPGM(const char *src, GP_Context **res)
{
	FILE *f = fopen(src, "r");
	uint32_t w, h, gray;
	GP_PixelType type;

	if (f == NULL)
		return GP_EBADFILE;

	if (fgetc(f) != 'P' || fgetc(f) != '2')
		goto err1;

	if (fscanf(f, "%"PRIu32"%"PRIu32"%"PRIu32, &w, &h, &gray) < 3)
		goto err1;

	switch (gray) {
	case 1:
		type = GP_PIXEL_G1;
		break;
	case 3:
		type = GP_PIXEL_G2;
		break;
	case 15:
		type = GP_PIXEL_G4;
		break;
	case 255:
		type = GP_PIXEL_G8;
		break;
	default:
		goto err1;
	}

	*res = GP_ContextAlloc(w, h, type);

	switch (gray) {
	case 1:
		if (GP_PXMLoad1bpp(f, *res))
			goto err2;
		break;
	case 3:
		if (GP_PXMLoad2bpp(f, *res))
			goto err2;
		break;
	//TODO
	}

	fclose(f);
	return GP_ESUCCESS;
err2:
	free(*res);
err1:
	fclose(f);
	return GP_EBADFILE;
}

GP_RetCode GP_SavePGM(const char *res, GP_Context *src)
{
	FILE *f;
	uint32_t gray;

	switch (src->pixel_type) {
	case GP_PIXEL_G1:
		gray = 1;
	break;
	case GP_PIXEL_G2:
		gray = 3;
	break;
	case GP_PIXEL_G4:
		gray = 15;
	break;
	case GP_PIXEL_G8:
		gray = 255;
	break;
	default:
		return GP_ENOIMPL;
	}
	
	f = fopen(res, "w");

	if (f == NULL)
		return GP_EBADFILE;

	if (fprintf(f, "P2\n%u %u\n%u\n# Generated by gfxprim\n",
	            (unsigned int) src->w, (unsigned int) src->h, gray) < 3)
		goto err;

	switch (gray) {
	case 1:
		if (GP_PXMSave1bpp(f, src))
			goto err;
		break;
	case 3:
		if (GP_PXMSave2bpp(f, src))
			goto err;
		break;
	//TODO
	}

	if (fclose(f))
		return GP_EBADFILE;

	return GP_ESUCCESS;
err:
	fclose(f);
	return GP_EBADFILE;
}
