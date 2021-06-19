//SPDX-License-Identifier: LGPL-2.0-or-later

/*

   Copyright (c) 2014-2020 Cyril Hrubis <metan@ucw.cz>

 */

#include <string.h>

#include <utils/gp_vec_str.h>

#include <widgets/gp_widgets.h>
#include <widgets/gp_widget_ops.h>
#include <widgets/gp_widget_render.h>

static unsigned int min_w(gp_widget *self, const gp_widget_render_ctx *ctx)
{
	unsigned int max_width;
	const gp_text_style *font = gp_widget_tattr_font(self->label->tattr, ctx);

	if (self->label->width)
		max_width = gp_text_max_width_chars(font, self->label->set, self->label->width);
	else
		max_width = gp_text_width(font, self->label->text);

	if (self->label->frame)
		max_width += 2 * ctx->padd;

	return max_width;
}

static unsigned int min_h(gp_widget *self, const gp_widget_render_ctx *ctx)
{
	const gp_text_style *font = gp_widget_tattr_font(self->label->tattr, ctx);

	return 2 * ctx->padd + gp_text_ascent(font);
}

static void render(gp_widget *self, const gp_offset *offset,
                   const gp_widget_render_ctx *ctx, int flags)
{
	(void) flags;
	unsigned int align;

	unsigned int x = self->x + offset->x;
	unsigned int y = self->y + offset->y;
	unsigned int w = self->w;
	unsigned int h = self->h;

	gp_widget_ops_blit(ctx, x, y, w, h);

	const gp_text_style *font = gp_widget_tattr_font(self->label->tattr, ctx);

	if (self->label->frame) {
		gp_fill_rrect_xywh(ctx->buf, x, y, w, h, ctx->bg_color,
		                   ctx->fg_color, ctx->text_color);

		x += ctx->padd;
		w -= 2 * ctx->padd;
	} else {
		gp_fill_rect_xywh(ctx->buf, x, y, w, h, ctx->bg_color);
	}

	if (self->label->ralign) {
		x += w - 1;
		align = GP_ALIGN_LEFT;
	} else {
		align = GP_ALIGN_RIGHT;
	}

	gp_text_fit(ctx->buf, font, x, y + ctx->padd, w,
	            align|GP_VALIGN_BELOW,
	            ctx->text_color, ctx->bg_color, self->label->text);
}

enum keys {
	FRAME,
	RALIGN,
	TATTR,
	TEXT,
	WIDTH
};

static const gp_json_obj_attr attrs[] = {
	GP_JSON_OBJ_ATTR("frame", GP_JSON_BOOL),
	GP_JSON_OBJ_ATTR("ralign", GP_JSON_BOOL),
	GP_JSON_OBJ_ATTR("tattr", GP_JSON_STR),
	GP_JSON_OBJ_ATTR("text", GP_JSON_STR),
	GP_JSON_OBJ_ATTR("width", GP_JSON_INT),
};

static const gp_json_obj obj_filter = {
	.attrs = attrs,
	.attr_cnt = GP_ARRAY_SIZE(attrs),
};

static gp_widget *json_to_label(gp_json_buf *json, gp_json_val *val, gp_htable **uids)
{
	char *label = NULL;
	int width = 0;
	int ralign = 0;
	int frame = 0;
	gp_widget_tattr attr = 0;

	(void)uids;

	GP_JSON_OBJ_FILTER(json, val, &obj_filter, gp_widget_json_attrs) {
		switch (val->idx) {
		case FRAME:
			frame = val->val_bool;
		break;
		case RALIGN:
			ralign = val->val_bool;
		break;
		case TATTR:
			if (gp_widget_tattr_parse(val->val_str, &attr))
				gp_json_warn(json, "Invalid text attribute '%s'", val->val_str);
		break;
		case TEXT:
			label = strdup(val->val_str);
		break;
		case WIDTH:
			width = val->val_int;
		break;
		}
	}

	gp_widget *ret = gp_widget_label_new(label, attr, width);

	ret->label->ralign = ralign;
	ret->label->frame = frame;

	free(label);

	return ret;
}

static void free_(gp_widget *self)
{
	gp_vec_free(self->label->text);
}

struct gp_widget_ops gp_widget_label_ops = {
	.min_w = min_w,
	.min_h = min_h,
	.render = render,
	.free = free_,
	.from_json = json_to_label,
	.id = "label",
};

static void redraw_resize(gp_widget *self)
{
	gp_widget_redraw(self);

	if (self->label->width)
		return;

	gp_widget_resize(self);
}

void gp_widget_label_set(gp_widget *self, const char *text)
{
	GP_WIDGET_ASSERT(self, GP_WIDGET_LABEL, );

	GP_DEBUG(3, "Setting widget label (%p) text '%s' -> '%s'",
		 self, self->label->text, text);

	self->label->text = gp_vec_printf(self->label->text, "%s", text);

	redraw_resize(self);
}

void gp_widget_label_append(gp_widget *self, const char *text)
{
	GP_WIDGET_ASSERT(self, GP_WIDGET_LABEL, );

	GP_DEBUG(3, "Appending to label widget (%p) '%s' += '%s'",
		 self, self->label->text, text);

	char *new_text = gp_vec_str_append(self->label->text, text);
	if (!new_text)
		return;

	self->label->text = new_text;

	redraw_resize(self);
}

gp_widget *gp_widget_label_new(const char *text, gp_widget_tattr tattr, unsigned int width)
{
	gp_widget *ret;

	ret = gp_widget_new(GP_WIDGET_LABEL, GP_WIDGET_CLASS_NONE, sizeof(struct gp_widget_label));
	if (!ret)
		return NULL;

	if (text)
		ret->label->text = gp_vec_strdup(text);
	else
		ret->label->text = gp_vec_str_new();

	ret->label->tattr = tattr;
	ret->label->width = width;
	ret->no_shrink = 1;

	return ret;
}

static char *vasprintf(const char *fmt, va_list ap)
{
	va_list ac;
	size_t len;

	va_copy(ac, ap);
	len = vsnprintf(NULL, 0, fmt, ac);
	va_end(ac);

	char *buf = malloc(len+1);
	if (!buf) {
		GP_WARN("Malloc failed :(");
		return NULL;
	}

	vsnprintf(buf, len+1, fmt, ap);

	return buf;
}

gp_widget *gp_widget_label_printf_new(gp_widget_tattr tattr, const char *fmt, ...)
{
	va_list ap;
	char *buf;

	va_start(ap, fmt);
	buf = vasprintf(fmt, ap);
	va_end(ap);

	gp_widget *ret = gp_widget_label_new(buf, tattr, 0);

	free(buf);

	return ret;
}

int gp_widget_label_printf(gp_widget *self, const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	self->label->text = gp_vec_vprintf(self->label->text, fmt, ap);
	va_end(ap);

	redraw_resize(self);

	return 0;
}

void gp_widget_label_vprintf(gp_widget *self, const char *fmt, va_list ap)
{
	self->label->text = gp_vec_vprintf(self->label->text, fmt, ap);
	redraw_resize(self);
}
