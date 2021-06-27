//SPDX-License-Identifier: LGPL-2.0-or-later

/*

   Copyright (c) 2014-2021 Cyril Hrubis <metan@ucw.cz>

 */

#include <string.h>

#include <widgets/gp_widgets.h>

static unsigned int stock_min_w(gp_widget *self, const gp_widget_render_ctx *ctx)
{
	return GP_ODD_UP(gp_widget_size_units_get(&self->stock->min_size, ctx));
}

static unsigned int stock_min_h(gp_widget *self, const gp_widget_render_ctx *ctx)
{
	return GP_ODD_UP(gp_widget_size_units_get(&self->stock->min_size, ctx));
}

static void render_stock_err_warn(gp_pixmap *pix,
                                  gp_coord x, gp_coord y,
                                  gp_size w, gp_size h, gp_pixel color, gp_pixel bg_col,
                                  const gp_widget_render_ctx *ctx)
{
	gp_coord cx = x + w/2;
	gp_coord cy = y + h/2;
	gp_size c = GP_MIN(w, h)/2 - 2;
	gp_size r = c/5;

	gp_fill_rrect_xywh(pix, x, y, w, h, bg_col, color, ctx->text_color);

	if (r > 1)
		gp_fill_circle(pix, cx, cy+h/4, r, ctx->text_color);
	else
		gp_fill_rect_xyxy(pix, cx-r, cy+h/4-r, cx+r, cy+h/4+r, ctx->text_color);

	gp_fill_rect_xyxy(pix, cx-r, cy-h/4-r, cx+r, cy+r, ctx->text_color);

}

static void render_stock_info(gp_pixmap *pix,
                              gp_coord x, gp_coord y,
                              gp_size w, gp_size h,
                              gp_pixel bg_col, const gp_widget_render_ctx *ctx)
{
	gp_size c = GP_MIN(w, h)/2 - 2;
	gp_size r = c/5;
	gp_coord cx = x + w/2;
	gp_coord cy = y + h/2;

	gp_fill_rrect_xywh(pix, x, y, w, h, bg_col, ctx->sel_color, ctx->text_color);

	if (r > 1)
		gp_fill_circle(pix, cx, cy-h/4, r, ctx->text_color);
	else
		gp_fill_rect_xyxy(pix, cx-r, cy-h/4-r, cx+r, cy-h/4+r, ctx->text_color);

	gp_fill_rect_xyxy(pix, cx-r, cy+h/4, cx+r, cy-r, ctx->text_color);

	gp_fill_rect_xyxy(pix, cx-2*r, cy+h/4, cx+2*r, cy+h/4+r/2, ctx->text_color);

	gp_fill_rect_xyxy(pix, cx-2*r, cy-r, cx, cy-r+r/2, ctx->text_color);
}

static void render_stock_question(gp_pixmap *pix,
                                  gp_coord x, gp_coord y,
                                  gp_size w, gp_size h, gp_pixel bg_col,
				  const gp_widget_render_ctx *ctx)
{
	gp_coord cx = x + w/2;
	gp_coord cy = y + h/2;
	gp_size c = GP_MIN(w, h)/2 - 2;
	gp_size r = c/5;

	gp_fill_rrect_xywh(pix, x, y, w, h, bg_col, ctx->sel_color, ctx->text_color);

	if (r > 1)
		gp_fill_circle(pix, cx, cy+h/4+r, r, ctx->text_color);
	else
		gp_fill_rect_xyxy(pix, cx-r, cy+h/4, cx+r, cy+h/4+2*r, ctx->text_color);

	gp_fill_rect_xyxy(pix, cx-r, cy-h/4+3*r, cx+r, cy+2*r, ctx->text_color);

	gp_fill_ring_seg(pix, cx, cy-h/4 + r, r, 3*r, GP_CIRCLE_SEG1 | GP_CIRCLE_SEG2 |GP_CIRCLE_SEG4, ctx->text_color);

	gp_fill_triangle(pix, cx-r, cy-h/4+3*r, cx, cy-h/4+2*r, cx, cy-h/4+3*r, ctx->text_color);
}

static void render_stock_speaker(gp_pixmap *pix,
                                 gp_coord x, gp_coord y, gp_size w, gp_size h,
                                 enum gp_widget_stock_type type,
                                 const gp_widget_render_ctx *ctx)
{
	gp_pixel col = type & GP_WIDGET_STOCK_FOCUSED ? ctx->sel_color : ctx->text_color;
	gp_coord cx = x + w/2;
	gp_coord cy = y + h/2;

	gp_size rx = w/4;
	gp_size ry = h/2 - rx - h/10;
	gp_size edge = rx/3;

	gp_coord poly[] = {
		cx-rx-edge, cy-ry,
		x, cy-ry,
		x, cy+ry,
		cx-rx-edge, cy+ry,
		cx-edge, cy+rx+ry,
		cx, cy+rx+ry,
		cx, cy-rx-ry,
		cx-edge, cy-rx-ry,
	};

	gp_fill_polygon(pix, GP_ARRAY_SIZE(poly)/2, poly, col);

	/* space between speaker and symbol */
	gp_size sp = GP_MAX(1u, rx/2);
	/* (half of) symbol size */
	gp_size swh = w/4 - (sp+1)/2;
	gp_size sw = 2*swh;
	/* most left x coordinate of symbol */
	gp_coord sx = cx + sp;
	/* line width */
	gp_size lw = (w)/16;
	/* ring radius */
	gp_size r1 = (rx + ry)-1;
	/* circle radius */
	gp_size r2 = r1/2-lw;

	gp_size i;

	gp_coord scx = cx + (sp+1)/2 + 1;

	switch (GP_WIDGET_STOCK_TYPE(type)) {
	case GP_WIDGET_STOCK_SPEAKER_MUTE:
		for (i = 0; i <= (7*lw+2)/5; i++) {
			gp_line(pix, sx+i, cy-swh, sx+sw, cy+swh-i, ctx->alert_color);
			gp_line(pix, sx, cy-swh+i, sx+sw-i, cy+swh, ctx->alert_color);
			gp_line(pix, sx+i, cy+swh, sx+sw, cy-swh+i, ctx->alert_color);
			gp_line(pix, sx, cy+swh-i, sx+sw-i, cy-swh, ctx->alert_color);
		}
	break;
	case GP_WIDGET_STOCK_SPEAKER_MIN:
	break;
	case GP_WIDGET_STOCK_SPEAKER_MAX:
		gp_fill_ring_seg(pix, scx, cy, r1-2*lw, r1, GP_CIRCLE_SEG1 | GP_CIRCLE_SEG4, col);
	/* fallthrough */
	case GP_WIDGET_STOCK_SPEAKER_MID:
		gp_fill_circle_seg(pix, scx, cy, r2, GP_CIRCLE_SEG1 | GP_CIRCLE_SEG4, col);
	break;
	case GP_WIDGET_STOCK_SPEAKER_INC:
		gp_fill_rect(pix, sx+swh-lw, cy-swh, sx+swh+lw, cy+swh, col);
	/* fallthrough */
	case GP_WIDGET_STOCK_SPEAKER_DEC:
		gp_fill_rect(pix, sx, cy-lw, sx+sw, cy+lw, col);
	break;
	default:
	break;
	}
}

static void render_text_lines(gp_pixmap *pix,
                              gp_coord x0, gp_coord y0,
                              gp_coord x1, gp_coord y1,
                              gp_pixel col)
{
	gp_size dy = (y1 - y0)+1;
	gp_size ls = dy/10;
	gp_size sp;

	if (dy < 5)
		sp = dy - 2*(ls + 1);
	else
		sp = (dy - 3*(ls + 1))/2+1;

	gp_fill_rect_xyxy(pix, x0, y1, x1, y1-ls, col);
	y1-=sp+ls;
	gp_fill_rect_xyxy(pix, x0, y1, x1, y1-ls, col);

	if (dy < 5)
		return;

	y1-=sp+ls;
	gp_fill_rect_xyxy(pix, x0, y1, x1, y1-ls, col);
}

static void render_stock_hardware(gp_pixmap *pix,
                                  gp_coord x, gp_coord y,
                                  gp_size w, gp_size h, gp_pixel bg_col,
                                  const gp_widget_render_ctx *ctx)
{
	gp_coord cx = x + w/2;
	gp_coord cy = y + h/2;
	gp_size c = GP_MIN(w, h)/3;
	gp_size dc = c/2;

	gp_coord l_end = GP_MIN(w, h)/2-c;
	gp_coord l_start = dc/10+2;
	gp_coord l_size = 1 + c/9;
	gp_coord sp_size = 1 + c/20;
	gp_coord c_sp_size = 1 + c/8;
	gp_coord legs = (2*c - 2*c_sp_size + sp_size)/(l_size + sp_size);
	gp_coord i;

	gp_fill_rect_xywh(pix, x, y, w, h, bg_col);

	gp_fill_rect_xyxy(pix, cx-c, cy-c, cx+c, cy+c, ctx->text_color);
	gp_fill_rect_xyxy(pix, cx-c/3, cy-c/3, cx+c/3, cy+c/3, ctx->fg_color);
	gp_putpixel(pix, cx-c/3, cy-c/3, ctx->text_color);

	gp_fill_circle(pix, cx-c+c/4+1, cy-c+c/4+1, c/8, ctx->sel_color);

	c_sp_size = (2*c - legs*l_size - (legs-1) * sp_size+1)/2;

	for (i = 0; i < legs; i++) {
		gp_coord off = -c + c_sp_size + i * (l_size + sp_size);

		gp_fill_rect_xyxy(pix, cx+off, cy-c-l_start, cx+off+l_size-1, cy-c-l_end, ctx->text_color);
		gp_fill_rect_xyxy(pix, cx+off, cy+c+l_start, cx+off+l_size-1, cy+c+l_end, ctx->text_color);

		gp_fill_rect_xyxy(pix, cx-c-l_start, cy+off, cx-c-l_end, cy+off+l_size-1, ctx->text_color);
		gp_fill_rect_xyxy(pix, cx+c+l_start, cy+off, cx+c+l_end, cy+off+l_size-1, ctx->text_color);
	}
}

static void render_stock_software(gp_pixmap *pix,
                                  gp_coord x, gp_coord y,
                                  gp_size w, gp_size h, gp_pixel bg_col,
                                  const gp_widget_render_ctx *ctx)
{
	gp_coord cx = x + w/2;
	gp_coord cy = y + h/2;
	gp_coord i;

	gp_size iw = w/2;
	gp_size ih = GP_MIN(iw, 7*h/16);
	gp_size th = GP_MIN(3*w/8, 3*h/8)/8;

	gp_size header = GP_ODD_UP(ih/2);

	gp_fill_rect_xywh(pix, x, y, w, h, bg_col);

	gp_fill_rect_xyxy(pix, cx-iw, cy-ih, cx+iw, cy+ih, ctx->text_color);
	gp_fill_rect_xyxy(pix, cx-iw+th+1, cy-ih+header, cx+iw-th-1, cy+ih-th-1, ctx->fg_color);

	gp_size r = header/8;
	gp_size off = th + 1 + r;

	for (i = 0; i < 3; i++) {
		gp_fill_circle(pix, cx+iw-off, cy-ih+header/2, r, ctx->fg_color);
		off += r ? 3*r+1 : 2;
	}

	render_text_lines(pix, cx-iw+th+ih/2+1, cy-ih+header+ih/3+1,
                          cx+iw-th-ih/2-1, cy+ih-th-1-ih/3-1, ctx->sel_color);
}

#define ROTATE(s, c, cx, x, cy, y) (1.00 * (cx) + (x)*(c) - (y)*(s) + 0.5), (1.00 * (cy) + (x)*(s) + (y)*(c) + 0.5)
#define ROT_PI_4(cx, x, cy, y) ROTATE(0.707, 0.707, cx, x, cy, y)
#define ROT_1_PI_8(cx, x, cy, y) ROTATE(0.38268343, 0.92387953, cx, x, cy, y)
#define ROT_3_PI_8(cx, x, cy, y) ROTATE(0.92387953, 0.38268343, cx, x, cy, y)

static void render_stock_settings(gp_pixmap *pix,
                                  gp_coord x, gp_coord y,
                                  gp_size w, gp_size h, gp_pixel bg_col,
                                  const gp_widget_render_ctx *ctx)
{
	gp_coord cx = x + w/2;
	gp_coord cy = y + h/2;

	gp_coord co = GP_MIN(w/2, h/2);
	gp_coord ci = co - co/4-2;
	gp_coord cc = GP_MIN(w/6, h/6);
	gp_coord tw = co/11;

	gp_fill_rect_xywh(pix, x, y, w, h, bg_col);

	gp_coord poly[] = {
		cx - co, cy + tw,
		cx - co, cy - tw,
		ROT_1_PI_8(cx, -ci, cy,  tw),
		ROT_1_PI_8(cx, -ci, cy, -tw),
		ROT_PI_4(cx, -co, cy,  tw),
		ROT_PI_4(cx, -co, cy, -tw),
		ROT_3_PI_8(cx, -ci, cy,  tw),
		ROT_3_PI_8(cx, -ci, cy, -tw),
		cx - tw, cy - co,
		cx + tw, cy - co,
		ROT_1_PI_8(cx, -tw, cy, -ci),
		ROT_1_PI_8(cx,  tw, cy, -ci),
		ROT_PI_4(cx, -tw, cy, -co),
		ROT_PI_4(cx,  tw, cy, -co),
		ROT_3_PI_8(cx, -tw, cy, -ci),
		ROT_3_PI_8(cx,  tw, cy, -ci),
		cx + co, cy - tw,
		cx + co, cy + tw,
		ROT_1_PI_8(cx, ci, cy, -tw),
		ROT_1_PI_8(cx, ci, cy,  tw),
		ROT_PI_4(cx, co, cy, -tw),
		ROT_PI_4(cx, co, cy,  tw),
		ROT_3_PI_8(cx, ci, cy, -tw),
		ROT_3_PI_8(cx, ci, cy,  tw),
		cx + tw, cy + co,
		cx - tw, cy + co,
		ROT_1_PI_8(cx,  tw, cy, ci),
		ROT_1_PI_8(cx, -tw, cy, ci),
		ROT_PI_4(cx,  tw, cy, co),
		ROT_PI_4(cx, -tw, cy, co),
		ROT_3_PI_8(cx,  tw, cy, ci),
		ROT_3_PI_8(cx, -tw, cy, ci),
	};

	gp_fill_polygon(pix, GP_ARRAY_SIZE(poly)/2, poly, ctx->text_color);

	gp_fill_circle(pix, cx, cy, cc, ctx->fg_color);
}

static void render_stock_save(gp_pixmap *pix,
                              gp_coord x, gp_coord y,
                              gp_size w, gp_size h, gp_pixel bg_col,
                              const gp_widget_render_ctx *ctx)
{
	gp_coord cx = x + w/2;
	gp_coord cy = y + h/2;
	gp_coord sh = GP_MIN(w/2, h/2);
	gp_size th = GP_MIN(3*w/8, 3*h/8)/8;
	gp_size cs = sh/3;
	gp_size cvr_s = (2 * sh - 2 * th) / 4;
	gp_size cvr_d = 3 * cvr_s / 2+1;

	gp_fill_rect_xywh(pix, x, y, w, h, bg_col);

	gp_coord poly[] = {
		cx - sh, cy - sh,
		cx - sh + th, cy - sh,
		/* inner polygon starts here */
		cx - sh + th, cy + sh - th,
		cx + sh - th, cy + sh - th,
		cx + sh - th, cy - sh + th + cs,
		cx + sh - th - cs, cy - sh + th,
		/* cover start */
		cx + cvr_s, cy - sh + th,
		cx + cvr_s, cy - sh + th + cvr_d,
		cx - cvr_s, cy - sh + th + cvr_d,
		cx - cvr_s, cy - sh + th,
		/* cover end */
		cx - sh + th, cy - sh + th,
		/* inner polygon ends here */
		cx - sh + th, cy - sh,
		cx + sh - cs - th/2, cy - sh,
		cx + sh, cy - sh + cs + th/2,
		cx + sh, cy + sh,
		cx - sh, cy + sh,
		cx - sh, cy - sh,
	};

	/* fills inner part of floppy */
	gp_fill_polygon(pix, 9, poly + 2, ctx->warn_color);

	/* label */
	gp_coord lx0 = cx - sh + th + sh/3+1;
	gp_coord ly0 = cy - sh + th + cvr_d + sh/3;
	gp_coord lx1 = cx + sh - th - sh/3-1;
	gp_coord ly1 = cy + sh - th - sh/3;

	gp_fill_rect_xyxy(pix, lx0, ly0, lx1, cy+sh, ctx->fg_color);

	render_text_lines(pix, lx0, ly0, lx1, ly1, ctx->text_color);

	/* draws border and cover */
	gp_fill_polygon(pix, GP_ARRAY_SIZE(poly)/2, poly, ctx->text_color);
	/* window in the cover */
	gp_fill_rect_xyxy(pix, cx+cvr_s-1-cvr_s/4, cy - sh + th+1 + cvr_d/5,
			       cx+cvr_s-1-2*(cvr_s/4), cy - sh + th+cvr_d-1 - cvr_d/5,
			       ctx->fg_color);
}

static void render_stock_file(gp_pixmap *pix,
                              gp_coord x, gp_coord y,
                              gp_size w, gp_size h, gp_pixel bg_col,
                              const gp_widget_render_ctx *ctx)
{
	gp_coord cx = x + w/2;
	gp_coord cy = y + h/2;
	gp_size sh = h/2;
	gp_size cs = sh/2;
	gp_size sw = GP_MIN(5*h/16, 3*w/4);
	gp_size th = GP_MIN(3*w/8, 3*h/8)/8;

	gp_fill_rect_xywh(pix, x, y, w, h, bg_col);

	gp_coord poly[] = {
		cx - sw, cy - sh,
		cx - sw + th, cy - sh,
		/* inner polygon starts here */
		cx - sw + th, cy + sh - th,
		cx + sw - th, cy + sh - th,
		/* corner start */
		cx + sw - th, cy - sh + 2*th + cs,
		cx + sw - cs - 2*th, cy - sh + 2*th + cs,
		cx + sw - cs - 2*th, cy - sh + th,
		cx + sw - cs - th, cy - sh + th,
		cx + sw - cs - th, cy - sh + th + cs,
		cx + sw - th, cy - sh + th + cs,
		/* corner end */
		cx + sw - th - cs, cy - sh + th,
		cx - sw + th, cy - sh + th,
		/* inner polygon ends here */
		cx - sw + th, cy - sh,
		cx + sw - cs - th/2, cy - sh,
		cx + sw, cy - sh + cs + th/2,
		cx + sw, cy + sh,
		cx - sw, cy + sh,
		cx - sw, cy - sh,
	};

	gp_coord in_poly[] = {
		cx - sw + th, cy + sh - th,
		cx + sw - th, cy + sh - th,
		cx + sw - th, cy - sh + th + cs,
		cx + sw - th - cs, cy - sh + th,
		cx - sw + th, cy - sh + th,
	};

	gp_fill_polygon(pix, GP_ARRAY_SIZE(in_poly)/2, in_poly, ctx->fg_color);
	gp_fill_polygon(pix, GP_ARRAY_SIZE(poly)/2, poly, ctx->text_color);

	/* text */
	gp_coord lx0 = cx - sw + th + sh/3;
	gp_coord ly0 = cy - sh + cs + th + sh/3+1;
	gp_coord lx1 = cx + sw - th - sh/3;
	gp_coord ly1 = cy + sh - th - sh/3-1;

	render_text_lines(pix, lx0, ly0, lx1, ly1, ctx->sel_color);
}

static void render_stock_dir(gp_pixmap *pix,
                             gp_coord x, gp_coord y,
                             gp_size w, gp_size h, gp_pixel bg_col,
                             const gp_widget_render_ctx *ctx)
{
	gp_coord cx = x + w/2;
	gp_coord cy = y + h/2;
	gp_size w2 = w/2;
	gp_size h2 = h/2;
	gp_size th = GP_MIN(3*w/8, 3*h/8)/8;
	gp_size cs = GP_MAX(2u, w/8);
	gp_size cw = w2;
	/* corner "roundess" */
	gp_size r = w2/10;
	gp_size fw = th+2;

	gp_fill_rect_xywh(pix, x, y, w, h, bg_col);

	gp_coord poly[] = {
		cx - w2, cy - h2 + r,
		cx - w2 + th, cy - h2,
		cx - w2 + th, cy - h2 + th + cs,
		cx - w2 + th, cy - h2 + th + cs,

		cx - w2 + th, cy + h2 - th,

		cx + w2 - fw - th, cy + h2 - th,
		cx + w2 - th, cy + h2 - th,

		cx + w2 - th, cy - h2 + cs + 2*th+fw,
		/* inner part starts here */
		cx + w2 - th, cy - h2 + cs + fw + th,
		cx + w2 - th, cy - h2 + cs + th,
		cx - w2 + th + cw, cy - h2 + th + cs,
		cx - w2 + th + cw, cy - h2 + th,
		cx - w2 + th, cy - h2 + th,
		/* inner part ends here */
		cx - w2 + th, cy - h2,
		cx - w2 + 2*th + cw-r, cy - h2,
		cx - w2 + 2*th + cw, cy - h2 + r,
		cx - w2 + 2*th + cw, cy - h2 + cs,

		cx + w2 - r, cy - h2 + cs,
		cx + w2, cy - h2 + cs+r,


		cx + w2, cy + h2,
		cx - w2, cy + h2,
		cx - w2, cy - h2 + cs,
	};

	gp_coord in_poly[] = {
		cx + w2 - th, cy - h2 + cs + fw + th,
		cx + w2 - th, cy - h2 + cs + th,
		cx - w2 + th + cw, cy - h2 + th + cs,
		cx - w2 + th + cw, cy - h2 + th,
		cx - w2 + th, cy - h2 + th,
		cx - w2 + th, cy - h2 + cs + fw + th,
	};

	/* fill the innter parts */
	gp_fill_rect_xyxy(pix, cx - w2 + th, cy - h2 + cs + th + fw,
	                       cx + w2 - th, cy + h2 - th, ctx->sel_color);
	gp_fill_polygon(pix, GP_ARRAY_SIZE(in_poly)/2, in_poly, ctx->fg_color);

	/* draw the lines */
	gp_fill_polygon(pix, GP_ARRAY_SIZE(poly)/2, poly, ctx->text_color);
	gp_hline_xxy(pix, cx-w2+th+1, cx+w2-th-1, cy-h2+cs+fw+th, ctx->text_color);

	/* final touch for low res */
	gp_putpixel(pix, cx + w2, cy - h2 + cs, bg_col);
	gp_putpixel(pix, cx - w2, cy - h2, bg_col);
	gp_putpixel(pix, cx - w2 + 2*th + cw, cy - h2, bg_col);
}

static void render_stock_arrow(gp_pixmap *pix,
                               int type,
                               gp_coord x, gp_coord y,
                               gp_size w, gp_size h, gp_pixel bg_col,
                               const gp_widget_render_ctx *ctx)
{
	gp_fill_rect_xywh(pix, x, y, w, h, bg_col);

	h--;
	w--;

	gp_coord up_poly[] = {
		x + w/2, y,
		x + (w+1)/2, y,
		x+w, y+h/2,
		x+w, y+(h+1)/2,
		x+w-3*w/8, y+(h+1)/2,
		x+w-3*w/8, y+h,
		x+3*w/8, y+h,
		x+3*w/8, y+(h+1)/2,
		x, y+(h+1)/2,
		x, y+h/2,
	};

	gp_coord down_poly[] = {
		x + w/2, y+h,
		x + (w+1)/2, y+h,
		x+w, y+(h+1)/2,
		x+w, y+h/2,
		x+w-3*w/8, y+h/2,
		x+w-3*w/8, y,
		x+3*w/8, y,
		x+3*w/8, y+h/2,
		x, y+h/2,
		x, y+(h+1)/2,
	};

	gp_coord left_poly[] = {
		x, y + h/2,
		x, y + (h+1)/2,
		x + w/2, y + h,
		x + (w+1)/2, y + h,
		x + (w+1)/2, y + h - 3*h/8,
		x + w, y + h - 3*h/8,
		x + w, y + 3*h/8,
		x + (w+1)/2, y + 3*h/8,
		x + (w+1)/2, y,
		x + w/2, y,
	};

	gp_coord right_poly[] = {
		x + w, y + h/2,
		x + w, y + (h+1)/2,
		x + (w+1)/2, y + h,
		x + w/2, y + h,
		x + w/2, y + h - 3*h/8,
		x, y + h - 3*h/8,
		x, y + 3*h/8,
		x + w/2, y + 3*h/8,
		x + w/2, y,
		x + (w+1)/2, y,
	};

	switch (GP_WIDGET_STOCK_TYPE(type)) {
	case GP_WIDGET_STOCK_ARROW_UP:
		gp_fill_polygon(pix, GP_ARRAY_SIZE(up_poly)/2, up_poly, ctx->text_color);
	break;
	case GP_WIDGET_STOCK_ARROW_DOWN:
		gp_fill_polygon(pix, GP_ARRAY_SIZE(down_poly)/2, down_poly, ctx->text_color);
	break;
	case GP_WIDGET_STOCK_ARROW_LEFT:
		gp_fill_polygon(pix, GP_ARRAY_SIZE(left_poly)/2, left_poly, ctx->text_color);
	break;
	case GP_WIDGET_STOCK_ARROW_RIGHT:
		gp_fill_polygon(pix, GP_ARRAY_SIZE(right_poly)/2, right_poly, ctx->text_color);
	break;
	}
}

static void render_stock_refresh(gp_pixmap *pix,
                                 gp_coord x, gp_coord y,
                                 gp_size w, gp_size h, gp_pixel bg_col,
                                 const gp_widget_render_ctx *ctx)
{
	gp_coord cx = x + w/2;
	gp_coord cy = y + h/2;

	gp_size th = GP_MIN(w, h)/16;
	gp_size s = GP_MAX(GP_MIN(w, h)/5, 1u);
	gp_size r = GP_MIN(w, h)/2-s;

	gp_fill_rect_xywh(pix, x, y, w, h, bg_col);

	gp_fill_ring_seg(pix, cx, cy, r - th, r + th,
	                 GP_CIRCLE_SEG2|GP_CIRCLE_SEG3|GP_CIRCLE_SEG4,
	                 ctx->text_color);

	gp_size sr = GP_MAX(s/3, 1u);

	gp_coord poly_1[] = {
		cx+th-sr, cy-r-s,
		cx+th, cy-r-s,
		cx+th+s, cy-r,
		cx+th, cy-r+s,
		cx+th-sr, cy-r+s,
	};

	gp_fill_polygon(pix, GP_ARRAY_SIZE(poly_1)/2, poly_1, ctx->text_color);
}

static void render_stock_shuffle_off(gp_pixmap *pix,
                                     gp_coord x, gp_coord y,
                                     gp_size w, gp_size h, gp_pixel bg_col,
                                     const gp_widget_render_ctx *ctx)
{
	gp_coord cx = x + w/2;
	gp_coord cy = y + h/2;

	gp_size wh = w/3;

	gp_size th = GP_MIN(w, h)/16;

	gp_fill_rect_xywh(pix, x, y, w, h, bg_col);

	gp_size yd = h/4;

	gp_coord poly_1[] = {
		cx-wh-3*th, cy+yd+th,
		cx-wh-3*th, cy+yd-th,
		cx+wh-th, cy+yd-th,
		cx+wh-th, cy+yd-3*th,
		cx+wh, cy+yd-3*th,
		cx+wh+3*th, cy+yd,
		cx+wh, cy+yd+3*th,
		cx+wh-th, cy+yd+3*th,
		cx+wh-th, cy+yd+th,
	};

	gp_fill_polygon(pix, GP_ARRAY_SIZE(poly_1)/2, poly_1, ctx->text_color);

	gp_coord poly_2[] = {
		cx-wh-3*th, cy-yd+th,
		cx-wh-3*th, cy-yd-th,
		cx+wh-th, cy-yd-th,
		cx+wh-th, cy-yd-3*th,
		cx+wh, cy-yd-3*th,
		cx+wh+3*th, cy-yd,
		cx+wh, cy-yd+3*th,
		cx+wh-th, cy-yd+3*th,
		cx+wh-th, cy-yd+th,
	};

	gp_fill_polygon(pix, GP_ARRAY_SIZE(poly_2)/2, poly_2, ctx->text_color);
}

static void render_stock_shuffle_on(gp_pixmap *pix,
                                    gp_coord x, gp_coord y,
                                    gp_size w, gp_size h, gp_pixel bg_col,
                                    const gp_widget_render_ctx *ctx)
{
	gp_coord cx = x + w/2;
	gp_coord cy = y + h/2;

	gp_size wh = w/3;

	gp_size th = GP_MIN(w, h)/16;

	gp_fill_rect_xywh(pix, x, y, w, h, bg_col);

	gp_size yd = h/4;

	cx -= 2*th;
	gp_size xs = yd/2;

	gp_coord poly_1[] = {
		cx-wh-th, cy-yd-th,
		cx-wh-th, cy-yd+th,
		cx-wh+xs, cy-yd+th,
		cx+wh-xs, cy+yd+th,
		cx+wh+th, cy+yd+th,

		cx+wh+th, cy+yd+3*th,
		cx+wh+2*th, cy+yd+3*th,
		cx+wh+5*th, cy+yd,
		cx+wh+2*th, cy+yd-3*th,
		cx+wh+th, cy+yd-3*th,

		cx+wh+th, cy+yd-th,
		cx+wh-xs+th, cy+yd-th,
		cx-wh+xs+th, cy-yd-th,
	};

	gp_fill_polygon(pix, GP_ARRAY_SIZE(poly_1)/2, poly_1, ctx->text_color);

	gp_coord poly_2[] = {
		cx-wh-th, cy+yd+th,
		cx-wh-th, cy+yd-th,
		cx-wh+xs, cy+yd-th,
		cx+wh-xs, cy-yd-th,
		cx+wh+th, cy-yd-th,

		cx+wh+th, cy-yd-3*th,
		cx+wh+2*th, cy-yd-3*th,
		cx+wh+5*th, cy-yd,
		cx+wh+2*th, cy-yd+3*th,
		cx+wh+th, cy-yd+3*th,

		cx+wh+th, cy-yd+th,
		cx+wh-xs+th, cy-yd+th,
		cx-wh+xs+th, cy+yd+th,
	};

	gp_fill_polygon(pix, GP_ARRAY_SIZE(poly_2)/2, poly_2, ctx->text_color);
}

static void widget_stock_render(gp_pixmap *pix, enum gp_widget_stock_type type,
                                gp_coord x, gp_coord y, gp_size w, gp_size h,
                                gp_pixel bg_col, const gp_widget_render_ctx *ctx)
{
	switch (GP_WIDGET_STOCK_TYPE(type)) {
	case GP_WIDGET_STOCK_SPEAKER_MIN:
	case GP_WIDGET_STOCK_SPEAKER_MAX:
	case GP_WIDGET_STOCK_SPEAKER_MID:
	case GP_WIDGET_STOCK_SPEAKER_MUTE:
	case GP_WIDGET_STOCK_SPEAKER_INC:
	case GP_WIDGET_STOCK_SPEAKER_DEC:
		render_stock_speaker(pix, x, y, w, h, type, ctx);
	break;
	case GP_WIDGET_STOCK_INFO:
		render_stock_info(pix, x, y, w, h, bg_col, ctx);
	break;
	case GP_WIDGET_STOCK_WARN:
		render_stock_err_warn(pix, x, y, w, h, ctx->warn_color, bg_col, ctx);
	break;
	case GP_WIDGET_STOCK_ERR:
		render_stock_err_warn(pix, x, y, w, h, ctx->alert_color, bg_col, ctx);
	break;
	case GP_WIDGET_STOCK_QUESTION:
		render_stock_question(pix, x, y, w, h, bg_col, ctx);
	break;
	case GP_WIDGET_STOCK_HARDWARE:
		render_stock_hardware(pix, x, y, w, h, bg_col, ctx);
	break;
	case GP_WIDGET_STOCK_SOFTWARE:
		render_stock_software(pix, x, y, w, h, bg_col, ctx);
	break;
	case GP_WIDGET_STOCK_SETTINGS:
		render_stock_settings(pix, x, y, w, h, bg_col, ctx);
	break;
	case GP_WIDGET_STOCK_SAVE:
		render_stock_save(pix, x, y, w, h, bg_col, ctx);
	break;
	case GP_WIDGET_STOCK_FILE:
		render_stock_file(pix, x, y, w, h, bg_col, ctx);
	break;
	case GP_WIDGET_STOCK_DIR:
		render_stock_dir(pix, x, y, w, h, bg_col, ctx);
	break;
	case GP_WIDGET_STOCK_REFRESH:
		render_stock_refresh(pix, x, y, w, h, bg_col, ctx);
	break;
	case GP_WIDGET_STOCK_ARROW_UP:
	case GP_WIDGET_STOCK_ARROW_DOWN:
	case GP_WIDGET_STOCK_ARROW_LEFT:
	case GP_WIDGET_STOCK_ARROW_RIGHT:
		render_stock_arrow(pix, type, x, y, w, h, bg_col, ctx);
	break;
	case GP_WIDGET_STOCK_SHUFFLE_ON:
		render_stock_shuffle_on(pix, x, y, w, h, bg_col, ctx);
	break;
	case GP_WIDGET_STOCK_SHUFFLE_OFF:
		render_stock_shuffle_off(pix, x, y, w, h, bg_col, ctx);
	break;
	}

}

void gp_widget_stock_render(gp_pixmap *pix, enum gp_widget_stock_type type,
                            gp_coord x, gp_coord y, gp_size w, gp_size h,
			    gp_pixel bg_col, const gp_widget_render_ctx *ctx)
{
	gp_fill_rect_xywh(pix, x, y, w, h, bg_col);

	widget_stock_render(pix, type, x, y, w, h, bg_col, ctx);
}

static void stock_render(gp_widget *self, const gp_offset *offset,
                         const gp_widget_render_ctx *ctx, int flags)
{
	gp_coord x = self->x + offset->x;
	gp_coord y = self->y + offset->y;
	gp_coord w = self->w;
	gp_coord h = self->h;

	gp_size off;

	switch (self->stock->type) {
	case GP_WIDGET_STOCK_ARROW_UP:
	case GP_WIDGET_STOCK_ARROW_DOWN:
	case GP_WIDGET_STOCK_ARROW_LEFT:
	case GP_WIDGET_STOCK_ARROW_RIGHT:
		off = GP_MAX(1u, (gp_size)GP_MIN(w/6, h/6));
	break;
	default:
		off = 0;
	break;
	}

	enum gp_widget_stock_type type = self->stock->type;

	if (self->focused)
		type |= GP_WIDGET_STOCK_FOCUSED;

	gp_fill_rect_xywh(ctx->buf, x, y, w, h, ctx->bg_color);

	gp_widget_stock_render(ctx->buf, type,
	                       x+off, y+off, w-2*off, h-2*off,
	                       ctx->bg_color, ctx);

	gp_widget_ops_blit(ctx, x, y, w, h);
}

static struct stock_types {
	const char *str_type;
	enum gp_widget_stock_type type;
} stock_types[] = {
	{"err", GP_WIDGET_STOCK_ERR},
	{"warn", GP_WIDGET_STOCK_WARN},
	{"info", GP_WIDGET_STOCK_INFO},
	{"question", GP_WIDGET_STOCK_QUESTION},

	{"speaker_min", GP_WIDGET_STOCK_SPEAKER_MIN},
	{"speaker_mid", GP_WIDGET_STOCK_SPEAKER_MID},
	{"speaker_max", GP_WIDGET_STOCK_SPEAKER_MAX},
	{"speaker_mute", GP_WIDGET_STOCK_SPEAKER_MUTE},
	{"speaker_inc", GP_WIDGET_STOCK_SPEAKER_INC},
	{"speaker_dec", GP_WIDGET_STOCK_SPEAKER_DEC},

	{"hardware", GP_WIDGET_STOCK_HARDWARE},
	{"software", GP_WIDGET_STOCK_SOFTWARE},
	{"settings", GP_WIDGET_STOCK_SETTINGS},
	{"save", GP_WIDGET_STOCK_SAVE},
	{"file", GP_WIDGET_STOCK_FILE},
	{"dir", GP_WIDGET_STOCK_DIR},

	{"refresh", GP_WIDGET_STOCK_REFRESH},

	{"shuffle_on", GP_WIDGET_STOCK_SHUFFLE_ON},
	{"shuffle_off", GP_WIDGET_STOCK_SHUFFLE_OFF},

	{"arrow_up", GP_WIDGET_STOCK_ARROW_UP},
	{"arrow_down", GP_WIDGET_STOCK_ARROW_DOWN},
	{"arrow_left", GP_WIDGET_STOCK_ARROW_LEFT},
	{"arrow_right", GP_WIDGET_STOCK_ARROW_RIGHT},
};

static int gp_widget_stock_type_from_str(const char *type)
{
	unsigned int i;

	for (i = 0; i < GP_ARRAY_SIZE(stock_types); i++) {
		if (!strcmp(type, stock_types[i].str_type))
			return stock_types[i].type;
	}

	return -1;
}

static const char *gp_widget_stock_type_name(enum gp_widget_stock_type type)
{
	unsigned int i;

	for (i = 0; i < GP_ARRAY_SIZE(stock_types); i++) {
		if (stock_types[i].type == type)
			return stock_types[i].str_type;
	}

	return NULL;
}

enum keys {
	MIN_SIZE,
	STOCK,
};

static const gp_json_obj_attr attrs[] = {
	GP_JSON_OBJ_ATTR("min_size", GP_JSON_STR),
	GP_JSON_OBJ_ATTR("stock", GP_JSON_STR),
};

static const gp_json_obj obj_filter = {
	.attrs = attrs,
	.attr_cnt = GP_ARRAY_SIZE(attrs),
};

static gp_widget *json_to_stock(gp_json_buf *json, gp_json_val *val, gp_htable **uids)
{
	gp_widget_size min_size = GP_WIDGET_SIZE_DEFAULT;
	int type = -1;

	(void)uids;

	GP_JSON_OBJ_FILTER(json, val, &obj_filter, gp_widget_json_attrs) {
		switch (val->idx) {
		case MIN_SIZE:
			if (gp_widget_size_units_parse(val->val_str, &min_size))
				gp_json_warn(json, "Invalid size string!");
		break;
		case STOCK:
			type = gp_widget_stock_type_from_str(val->val_str);
			if (type < 0)
				gp_json_warn(json, "Unknown stock type!");
		break;
		}
	}

	if (type < 0) {
		gp_json_warn(json, "Missing stock type!");
		return NULL;
	}

	return gp_widget_stock_new(type, min_size);
}

static int stock_event(gp_widget *self, const gp_widget_render_ctx *ctx, gp_event *ev)
{
	(void) ctx;

	if (ev->type != GP_EV_KEY)
		return 0;

	if (ev->code == GP_EV_KEY_UP)
		return 0;

	switch (ev->val) {
	case GP_BTN_LEFT:
	case GP_BTN_PEN:
	case GP_KEY_ENTER:
	        gp_widget_send_widget_event(self, 0);
		return 1;
	}

	return 0;
}

struct gp_widget_ops gp_widget_stock_ops = {
	.min_w = stock_min_w,
	.min_h = stock_min_h,
	.render = stock_render,
	.event = stock_event,
	.from_json = json_to_stock,
	.id = "stock",
};

gp_widget *gp_widget_stock_new(enum gp_widget_stock_type type, gp_widget_size min_size)
{
	const gp_widget_render_ctx *ctx = gp_widgets_render_ctx();
	gp_widget *ret;

	if (!gp_widget_stock_type_name(type)) {
		GP_WARN("Invalid stock type %u", type);
		return NULL;
	}

	ret = gp_widget_new(GP_WIDGET_STOCK, GP_WIDGET_CLASS_NONE, sizeof(struct gp_widget_stock));
	if (!ret)
		return NULL;

		GP_ODD_UP(gp_text_ascent(ctx->font) + 2 * ctx->padd);

	if (GP_WIDGET_SIZE_EQ(min_size, GP_WIDGET_SIZE_DEFAULT))
		ret->stock->min_size = GP_WIDGET_SIZE(0, 2, 1);
	else
		ret->stock->min_size = min_size;

	ret->stock->type = type;
	ret->no_events = 1;

	return ret;
}

void gp_widget_stock_type_set(gp_widget *self, enum gp_widget_stock_type type)
{
	GP_WIDGET_ASSERT(self, GP_WIDGET_STOCK, );

	if (self->stock->type == type)
		return;

	if (!gp_widget_stock_type_name(type)) {
		GP_WARN("Invalid stock type %u", type);
		return;
	}

	self->stock->type = type;
	gp_widget_redraw(self);
}
