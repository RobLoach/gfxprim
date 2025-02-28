//SPDX-License-Identifier: LGPL-2.0-or-later

/*

   Copyright (c) 2014-2021 Cyril Hrubis <metan@ucw.cz>

 */

#include <string.h>
#include <json-c/json.h>

#include <widgets/gp_widgets.h>
#include <widgets/gp_widget_ops.h>
#include <widgets/gp_widget_render.h>
#include <widgets/gp_string.h>

static unsigned int min_w(gp_widget *self, const gp_widget_render_ctx *ctx)
{
	unsigned int text_a = gp_text_ascent(ctx->font);
	unsigned int text_w = 0;
	unsigned int i, w;

	for (i = 0; i < self->choice->max; i++) {
		w = gp_text_width(ctx->font, self->choice->choices[i]);
		text_w = GP_MAX(text_w, w);
	}

	return ctx->padd + text_a + text_w;
}

static unsigned int min_h(gp_widget *self, const gp_widget_render_ctx *ctx)
{
	return ctx->padd +
	       self->choice->max * (gp_text_ascent(ctx->font) + ctx->padd);
}

static void render(gp_widget *self, const gp_offset *offset,
                   const gp_widget_render_ctx *ctx, int flags)
{
	unsigned int text_a = gp_text_ascent(ctx->font);
	unsigned int x = self->x + offset->x;
	unsigned int y = self->y + offset->y;
	unsigned int w = self->w;
	unsigned int h = self->h;
	unsigned int i;

	(void)flags;

	gp_widget_ops_blit(ctx, x, y, w, h);

	gp_fill_rect_xywh(ctx->buf, x, y, w, h, ctx->bg_color);

	y += ctx->padd;

	for (i = 0; i < self->choice->max; i++) {
		unsigned int r = text_a/2;
		unsigned int cy = y + r;
		unsigned int cx = x + r;

		gp_fill_circle(ctx->buf, cx, cy, r, ctx->fg_color);
		gp_pixel color = self->focused ? ctx->sel_color : ctx->text_color;
		gp_circle(ctx->buf, cx, cy, r, color);

		if (i == self->choice->sel) {
			gp_fill_circle(ctx->buf, cx, cy, r - 3,
			              ctx->text_color);
		}

		gp_text(ctx->buf, ctx->font,
			x + ctx->padd + text_a, y,
		        GP_ALIGN_RIGHT|GP_VALIGN_BELOW,
                        ctx->text_color, ctx->bg_color,
			self->choice->choices[i]);

		y += text_a + ctx->padd;
	}
}

static void select_choice(gp_widget *self, unsigned int select)
{
	if (self->choice->sel == select)
		return;

	if (select >= self->choice->max) {
		GP_BUG("Widget %p trying to select %i max %i",
			self, select, self->choice->max);
		return;
	}

	self->choice->sel = select;

	gp_widget_redraw(self);

	gp_widget_send_widget_event(self, 0);
}

static void key_up(gp_widget *self)
{
	if (self->choice->sel > 0)
		select_choice(self, self->choice->sel - 1);
	else
		select_choice(self, self->choice->max - 1);
}

static void key_down(gp_widget *self)
{
	if (self->choice->sel + 1 >= self->choice->max)
		select_choice(self, 0);
	else
		select_choice(self, self->choice->sel + 1);
}

static void radio_click(gp_widget *self, const gp_widget_render_ctx *ctx, gp_event *ev)
{
	unsigned int min_x = 0;
	unsigned int max_x = self->w;
	unsigned int min_y = ctx->padd;
	unsigned int max_y = self->h - ctx->padd;
	unsigned int text_h = gp_text_ascent(ctx->font) + ctx->padd;

	if (ev->cursor_x < min_x || ev->cursor_x > max_x)
		return;

	if (ev->cursor_y < min_y || ev->cursor_y > max_y)
		return;

	unsigned int select = (ev->cursor_y - min_y) / text_h;

	select_choice(self, select);
}

static int event(gp_widget *self, const gp_widget_render_ctx *ctx, gp_event *ev)
{
	switch (ev->type) {
	case GP_EV_KEY:
		if (ev->code == GP_EV_KEY_UP)
			return 0;

		switch (ev->val) {
		case GP_BTN_PEN:
		case GP_BTN_LEFT:
			radio_click(self, ctx, ev);
			return 1;
		case GP_KEY_DOWN:
			key_down(self);
			return 1;
		case GP_KEY_UP:
			key_up(self);
			return 1;
		case GP_KEY_HOME:
			select_choice(self, 0);
			return 1;
		case GP_KEY_END:
			select_choice(self, self->choice->max - 1);
			return 1;
		}
	}

	return 0;
}

static gp_widget *json_to_radiobutton(json_object *json, gp_htable **uids)
{
	return gp_widget_choice_from_json(GP_WIDGET_RADIOBUTTON, json, uids);
}

struct gp_widget_ops gp_widget_radiobutton_ops = {
	.min_w = min_w,
	.min_h = min_h,
	.render = render,
	.event = event,
	.from_json = json_to_radiobutton,
	.id = "radiobutton",
};
