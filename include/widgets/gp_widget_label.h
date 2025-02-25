//SPDX-License-Identifier: LGPL-2.0-or-later
/*

   Copyright (c) 2014-2020 Cyril Hrubis <metan@ucw.cz>

 */

#ifndef GP_WIDGET_LABEL_H
#define GP_WIDGET_LABEL_H

#include <stdlib.h>

struct gp_widget_label {
	/* Text buffer */
	char *text;
	/* widget size hints */
	const char *set;
	unsigned int width:8;
	/* attributes */
	unsigned int ralign:1;
	unsigned int frame:1;
	/* text attributes */
	gp_widget_tattr tattr;
};

/**
 * @brief Sets the label text.
 *
 * @self A label widget.
 * @param text New widget label text.
 */
void gp_widget_label_set(gp_widget *self, const char *text);

/**
 * @brief Appends text to the label.
 *
 * @self A label widget.
 * @param text Text to be appended.
 */
void gp_widget_label_append(gp_widget *self, const char *text);

/**
 * @brief Printf-like function to set label text.
 * @self A label widget.
 * @param fmt  Printf formatting string.
 * @param ...  Printf parameters.
 * @return Number of characters printed.
 */
int gp_widget_label_printf(gp_widget *self, const char *fmt, ...)
                           __attribute__((format (printf, 2, 3)));

/**
 * @brief VPrintf-like function to set label text.
 * @self A label widget.
 * @param fmt  Printf formatting string.
 * @param ...  Printf parameters.
 */
void gp_widget_label_vprintf(gp_widget *self, const char *fmt, va_list ap);

/**
 * @brief Changes text attributes.
 *
 * @self A label widget.
 * @tattr New text attributes.
 */
static inline void gp_widget_label_tattr_set(gp_widget *self, gp_widget_tattr tattr)
{
	GP_WIDGET_ASSERT(self, GP_WIDGET_LABEL, );

	self->label->tattr = tattr;

	gp_widget_redraw(self);
}

/**
 * @brief Changes widget minimal width.
 *
 * @self Pointer to a label widget.
 * @width New label width, the unit for the width is font characters.
 */
static inline void gp_widget_label_width_set(gp_widget *self, unsigned int width)
{
	self->label->width = width;
	gp_widget_resize(self);
}

/**
 * @brief Allocates a label widget.
 *
 * Label minimal width is a bit more complicated topic since labels can change
 * content and hence size dynamically. There are two different modes of
 * operation.
 *
 * - If width is set, the size is set to stone so that the label can hold width
 *   characters. The main problem is that for proprotional fonts the allocated
 *   space is huge because we account for maximal character width. To
 *   accomodate for that you can also pass a string with subset of characters
 *   that are expected to appear in the label to make that computation more
 *   precise.
 *
 * - If width is not set, i.e. set to zero, the size is computed accordintly to
 *   the current label text, however by default the widget does not shrink,
 *   e.g. if the new string width is shorter than the last one we do not
 *   trigger resize, which avoids for the layout to size to jump on label text
 *   changes. You can manually trigger shrinking by calling gp_widget_resize().
 *
 * @param text A label text.
 * @param tattr Text attributes, e.g. bold.
 * @param width Maximal expected text width, if set to non-zero it's used to
 *              callculate the label size.
 * @return Newly allocated label widget.
 */
gp_widget *gp_widget_label_new(const char *text, gp_widget_tattr tattr, unsigned int width);

/**
 * @brief Printf-like function to create a label widget.
 *
 * @param tattr Text attributes, e.g. bold.
 * @param fmt Printf formatting string.
 * @param ... Printf parameters.
 * @return Newly allocated label widget.
 */
gp_widget *gp_widget_label_printf_new(gp_widget_tattr tattr, const char *fmt, ...)
                                      __attribute__((format (printf, 2, 3)));

#endif /* GP_WIDGET_LABEL_H */
