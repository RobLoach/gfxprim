//SPDX-License-Identifier: LGPL-2.0-or-later

/*

   Copyright (c) 2014-2021 Cyril Hrubis <metan@ucw.cz>

 */

#include <string.h>
#include <json-c/json.h>

#include <widgets/gp_widgets.h>
#include <widgets/gp_widget_ops.h>
#include <widgets/gp_widget_render.h>

static unsigned int pbar_min_w(gp_widget *self, const gp_widget_render_ctx *ctx)
{
	(void)self;

	return 2 * ctx->padd + gp_text_max_width(ctx->font, 7);
}

static unsigned int pbar_min_h(gp_widget *self, const gp_widget_render_ctx *ctx)
{
	(void)self;

	return 2 * ctx->padd + gp_text_ascent(ctx->font);
}

static unsigned int val_to_secs(float sec)
{
	return (unsigned int)(sec + 0.5) % 60;
}

static unsigned int val_to_mins(float sec)
{
	return ((unsigned int)sec/60) % 60;
}

static unsigned int val_to_hours(float sec)
{
	return ((unsigned int)sec/3600) % 60;
}

static void pbar_render(gp_widget *self, const gp_offset *offset,
                        const gp_widget_render_ctx *ctx, int flags)
{
	unsigned int x = self->x + offset->x;
	unsigned int y = self->y + offset->y;
	unsigned int w = self->w;
	unsigned int h = self->h;

	(void)flags;

	gp_widget_ops_blit(ctx, x, y, w, h);

	unsigned int wd = self->pbar->val * w / self->pbar->max;

	gp_pixmap p;

	gp_sub_pixmap(ctx->buf, &p, x, y, wd, h);
	if (p.w > 0) {
		gp_fill_rrect_xywh(&p, 0, 0, w, h, ctx->bg_color,
		                   ctx->fg2_color, ctx->text_color);
	}

	gp_sub_pixmap(ctx->buf, &p, x+wd, y, w-wd, h);
	if (p.w > 0) {
		gp_fill_rrect_xywh(&p, -wd, 0, w, h, ctx->bg_color,
		                   ctx->fg_color, ctx->text_color);
	}

	char buf[64];
	float val = self->pbar->val;

	unsigned int hours, mins, secs;

	if (self->pbar->unit & GP_WIDGET_PBAR_INVERSE)
		val = self->pbar->max - val;

	switch (self->pbar->unit & GP_WIDGET_PBAR_TMASK) {
	case GP_WIDGET_PBAR_NONE:
		return;
	case GP_WIDGET_PBAR_PERCENTS:
		snprintf(buf, sizeof(buf), "%.2f%%",
		         100 * val / self->pbar->max);
	break;
	case GP_WIDGET_PBAR_SECONDS:
		hours = val_to_hours(val);
		mins = val_to_mins(val);
		secs = val_to_secs(val);
		if (hours) {
			snprintf(buf, sizeof(buf), "%uh %um %us",
			         hours, mins, secs);
		} else if (mins) {
			snprintf(buf, sizeof(buf), "%um %us",
			         mins, secs);
		} else {
			snprintf(buf, sizeof(buf), "%us", secs);
		}
	break;
	}

	gp_print(ctx->buf, ctx->font, x + w/2, y + ctx->padd,
		 GP_ALIGN_CENTER | GP_VALIGN_BELOW | GP_TEXT_NOBG,
		 ctx->text_color, ctx->bg_color, "%s", buf);
}

static int check_val(double val, double max)
{
	if (val < 0 || val > max) {
		GP_WARN("Invalid progressbar value %lf", val);
		return 1;
	}

	return 0;
}

static int check_max(double max)
{
	if (max <= 0) {
		GP_WARN("Invalid progressbar max %lf", max);
		return 1;
	}

	return 0;
}

static gp_widget *json_to_pbar(json_object *json, gp_htable **uids)
{
	double val = 0, max = 100;
	const char *type = NULL;
	enum gp_widget_pbar_unit unit = GP_WIDGET_PBAR_PERCENTS;
	int inverse = 0;

	(void)uids;

	json_object_object_foreach(json, key, jval) {
		if (!strcmp(key, "val"))
			val = json_object_get_double(jval);
		else if (!strcmp(key, "unit"))
			type = json_object_get_string(jval);
		else if (!strcmp(key, "max"))
			max = json_object_get_double(jval);
		else if (!strcmp(key, "inverse"))
			inverse = json_object_get_boolean(jval);
		else
			GP_WARN("Invalid int pbar '%s'", key);
	}

	if (check_max(max))
		max = 100;

	if (check_val(val, max))
		val = 0;

	if (type) {
		if (!strcmp(type, "none"))
			unit = GP_WIDGET_PBAR_NONE;
		else if (!strcmp(type, "percents"))
			unit = GP_WIDGET_PBAR_PERCENTS;
		else if (!strcmp(type, "seconds"))
			unit = GP_WIDGET_PBAR_SECONDS;
		else
			GP_WARN("Invalid type '%s'", type);
	}

	if (inverse)
		unit |= GP_WIDGET_PBAR_INVERSE;

	return gp_widget_pbar_new(val, max, unit);
}

struct gp_widget_ops gp_widget_pbar_ops = {
	.min_w = pbar_min_w,
	.min_h = pbar_min_h,
	.render = pbar_render,
	.from_json = json_to_pbar,
	.id = "pbar",
};

gp_widget *gp_widget_pbar_new(float val, float max, enum gp_widget_pbar_unit unit)
{
	size_t size = sizeof(struct gp_widget_pbar);
	gp_widget *ret;

	if (check_val(val, max))
		val = 0;

	ret = gp_widget_new(GP_WIDGET_PROGRESSBAR, GP_WIDGET_CLASS_NONE, size);
	if (!ret)
		return NULL;

	ret->pbar->val = val;
	ret->pbar->unit = unit;
	ret->pbar->max = max;

	return ret;
}

void gp_widget_pbar_set(gp_widget *self, float val)
{
	GP_WIDGET_ASSERT(self, GP_WIDGET_PROGRESSBAR, );

	GP_DEBUG(3, "Setting widget (%p) progressbar val '%.2f' -> '%.2f'",
		 self, self->pbar->val, val);

	if (check_val(val, self->pbar->max))
		return;

	if ((int)(100 * self->pbar->val) != (int)(100 * val)) {
		gp_widget_redraw(self);
		self->pbar->val = val;
	}
}

void gp_widget_pbar_set_max(gp_widget *self, float max)
{
	GP_WIDGET_ASSERT(self, GP_WIDGET_PROGRESSBAR, );

	GP_DEBUG(3, "Setting widget (%p) progressbar max '%.2f' -> '%.2f'",
		 self, self->pbar->max, max);

	if (check_max(max))
		return;

	self->pbar->val = GP_MIN(self->pbar->val, max);
	self->pbar->max = max;

	gp_widget_redraw(self);
}
