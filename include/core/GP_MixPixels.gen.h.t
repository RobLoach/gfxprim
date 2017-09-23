@ include header.t
/*
 * Macros to mix two pixels accordingly to percentage.
 *
 * Copyright (C) 2011-2014 Cyril Hrubis <metan@ucw.cz>
 */

#include "core/GP_Pixmap.h"
#include "core/GP_Pixel.h"
#include "core/GP_GetPutPixel.h"
#include "core/GP_GammaCorrection.h"

@ for pt in pixeltypes:
@     if not pt.is_unknown():

/*
 * Mixes two {{ pt.name }} pixels.
 *
 * The percentage is expected as 8 bit unsigned integer [0 .. 255]
 */
#define GP_MIX_PIXELS_LINEAR_{{ pt.name }}(pix1, pix2, perc) ({ \
@         for c in pt.chanslist:
	GP_Pixel {{ c[0] }}; \
\
	{{ c[0] }}  = GP_Pixel_GET_{{ c[0] }}_{{ pt.name }}(pix1) * (perc); \
	{{ c[0] }} += GP_Pixel_GET_{{ c[0] }}_{{ pt.name }}(pix2) * (255 - (perc)); \
	{{ c[0] }} = ({{ c[0] }} + 128) / 255; \
\
@         end
\
	GP_Pixel_CREATE_{{ pt.name }}({{ ', '.join(pt.chan_names) }}); \
})

/*
 * Mixes two {{ pt.name }} pixels.
 *
 * The percentage is expected as 8 bit unsigned integer [0 .. 255]
 */
#define GP_MIX_PIXELS_GAMMA_{{ pt.name }}(pix1, pix2, perc) ({ \
@         for c in pt.chanslist:
	GP_Pixel {{ c[0] }}; \
\
	{{ c[0] }}  = GP_Gamma{{ c[2] }}ToLinear10(GP_Pixel_GET_{{ c[0] }}_{{ pt.name }}(pix1)) * (perc); \
	{{ c[0] }} += GP_Gamma{{ c[2] }}ToLinear10(GP_Pixel_GET_{{ c[0] }}_{{ pt.name }}(pix2)) * (255 - (perc)); \
	{{ c[0] }} = ({{ c[0] }} + 128) / 255; \
	{{ c[0] }} = GP_Linear10ToGamma{{ c[2] }}({{ c[0] }}); \
\
@         end
\
	GP_Pixel_CREATE_{{ pt.name }}({{ ", ".join(pt.chan_names) }}); \
})

#define GP_MIX_PIXELS_{{ pt.name }}(pix1, pix2, perc) \
@         if pt.is_rgb():
	GP_MIX_PIXELS_LINEAR_{{ pt.name }}(pix1, pix2, perc)
@         else:
	GP_MIX_PIXELS_LINEAR_{{ pt.name }}(pix1, pix2, perc)
@ end

static inline GP_Pixel GP_MixPixels(GP_Pixel pix1, GP_Pixel pix2,
                                    uint8_t perc, GP_PixelType pixel_type)
{
	switch (pixel_type) {
@ for pt in pixeltypes:
@     if not pt.is_unknown():
	case GP_PIXEL_{{ pt.name }}:
		return GP_MIX_PIXELS_LINEAR_{{ pt.name }}(pix1, pix2, perc);
@ end
	default:
		GP_ABORT("Unknown pixeltype");
	}
}


@ for pt in pixeltypes:
@     if not pt.is_unknown():
static inline void GP_MixPixel_Raw_{{ pt.name }}(GP_Pixmap *pixmap,
			GP_Coord x, GP_Coord y, GP_Pixel pixel, uint8_t perc)
{
	GP_Pixel pix = GP_GetPixel_Raw_{{ pt.pixelsize.suffix }}(pixmap, x, y);
	pix = GP_MIX_PIXELS_{{ pt.name }}(pixel, pix, perc);
	GP_PutPixel_Raw_{{ pt.pixelsize.suffix }}(pixmap, x, y, pix);
}

@ end

@ for pt in pixeltypes:
@     if not pt.is_unknown():
static inline void GP_MixPixel_Raw_Clipped_{{ pt.name }}(GP_Pixmap *pixmap,
			GP_Coord x, GP_Coord y, GP_Pixel pixel, uint8_t perc)
{
	if (GP_PIXEL_IS_CLIPPED(pixmap, x, y))
		return;

	GP_MixPixel_Raw_{{ pt.name }}(pixmap, x, y, pixel, perc);
}

@ end

static inline void GP_MixPixel_Raw(GP_Pixmap *pixmap, GP_Coord x, GP_Coord y,
                                   GP_Pixel pixel, uint8_t perc)
{
	switch (pixmap->pixel_type) {
@ for pt in pixeltypes:
@     if not pt.is_unknown():
	case GP_PIXEL_{{ pt.name }}:
				GP_MixPixel_Raw_{{ pt.name }}(pixmap, x, y, pixel, perc);
	break;
@ end
	default:
		GP_ABORT("Unknown pixeltype");
	}
}

static inline void GP_MixPixel_Raw_Clipped(GP_Pixmap *pixmap,
                                           GP_Coord x, GP_Coord y,
                                           GP_Pixel pixel, uint8_t perc)
{
	switch (pixmap->pixel_type) {
@ for pt in pixeltypes:
@     if not pt.is_unknown():
	case GP_PIXEL_{{ pt.name }}:
		GP_MixPixel_Raw_Clipped_{{ pt.name }}(pixmap, x, y, pixel, perc);
	break;
@ end
	default:
		GP_ABORT("Unknown pixeltype");
	}
}

