//SPDX-License-Identifier: LGPL-2.0-or-later

/*

   Copyright (c) 2014-2021 Cyril Hrubis <metan@ucw.cz>

 */

#include <string.h>

#include <widgets/gp_widgets.h>
#include <widgets/gp_widget_ops.h>
#include <widgets/gp_widget_render.h>

static unsigned int header_min_w(gp_widget_table *tbl,
                                 const gp_widget_render_ctx *ctx,
                                 unsigned int col)
{
	const char *label = tbl->header[col].label;
	unsigned int text_size = 0;

	if (label)
		text_size += gp_text_width(ctx->font_bold, label);

	if (tbl->header[col].sortable)
		text_size += ctx->padd + gp_text_ascent(ctx->font);

	return text_size;
}

static unsigned int min_w(gp_widget *self, const gp_widget_render_ctx *ctx)
{
	struct gp_widget_table *tbl = self->tbl;
	unsigned int i, sum_cols_w = 0;

	if (tbl->header) {
		for (i = 0; i < tbl->cols; i++)
			tbl->cols_w[i] = header_min_w(tbl, ctx, i);
	}

	for (i = 0; i < tbl->cols; i++) {
		unsigned int col_size;
		col_size = gp_text_max_width(ctx->font, tbl->header[i].col_min_size);
		tbl->cols_w[i] = GP_MAX(tbl->cols_w[i], col_size);
		sum_cols_w += tbl->cols_w[i];
	}

	return sum_cols_w + (2 * tbl->cols) * ctx->padd;
}

static unsigned int header_h(gp_widget *self, const gp_widget_render_ctx *ctx)
{
	unsigned int text_a = gp_text_ascent(ctx->font);

	if (!self->tbl->header)
		return 0;

	return text_a + 2 * ctx->padd;
}

static unsigned int row_h(const gp_widget_render_ctx *ctx)
{
	unsigned int text_a = gp_text_ascent(ctx->font);

	return text_a + ctx->padd;
}

static unsigned int min_h(gp_widget *self, const gp_widget_render_ctx *ctx)
{
	unsigned int h = row_h(ctx) * self->tbl->min_rows;

	if (self->tbl->header)
		h += header_h(self, ctx);

	return h;
}

static unsigned int display_rows(gp_widget *self, const gp_widget_render_ctx *ctx)
{
	unsigned int text_a = gp_text_ascent(ctx->font);
	unsigned int header = header_h(self, ctx);

	return (self->h - header) / (text_a + ctx->padd);
}

static void distribute_size(gp_widget *self, const gp_widget_render_ctx *ctx, int new_wh)
{
	gp_widget_table *tbl = self->tbl;
	unsigned int i, sum_cols_w = 0, sum_fills = 0;

	(void)new_wh;

	for (i = 0; i < tbl->cols; i++) {
		sum_cols_w += tbl->cols_w[i];
		sum_fills += tbl->header[i].col_fill;
	}

	if (!sum_fills)
		return;

	unsigned int table_w = sum_cols_w + (2 * tbl->cols) * ctx->padd;
	unsigned int diff = self->w - table_w;

	for (i = 0; i < tbl->cols; i++)
		tbl->cols_w[i] += tbl->header[i].col_fill * (diff/sum_fills);
}

static unsigned int header_render(gp_widget *self, gp_coord x, gp_coord y,
                                  const gp_widget_render_ctx *ctx)
{
	gp_widget_table *tbl = self->tbl;
	const gp_widget_table_header *header = tbl->header;
	unsigned int text_a = gp_text_ascent(ctx->font);
	unsigned int cx = x + ctx->padd;
	unsigned int cy = y + ctx->padd;
	unsigned int i;

	int render_header = 0;

	for (i = 0; i < tbl->cols; i++) {
		if (tbl->header[i].label) {
			render_header = 1;
			break;
		}

		if (tbl->header[i].sortable) {
			render_header = 1;
			break;
		}
	}

	if (!render_header)
		return 0;

	for (i = 0; i < tbl->cols; i++) {
		if (tbl->header[i].sortable) {
			gp_size sym_size = text_a/3;
			gp_size sx = cx + tbl->cols_w[i] - ctx->padd;
			gp_size sy = cy + text_a/2;

			if (i == tbl->sorted_by_col) {
				if (tbl->sorted_desc)
					gp_symbol(ctx->buf, sx, sy, sym_size, sym_size, GP_TRIANGLE_DOWN, ctx->text_color);
				else
					gp_symbol(ctx->buf, sx, sy, sym_size, sym_size, GP_TRIANGLE_UP, ctx->text_color);
			} else {
				gp_symbol(ctx->buf, sx, sy - sym_size, sym_size, sym_size/2, GP_TRIANGLE_UP, ctx->text_color);
				gp_symbol(ctx->buf, sx, sy + sym_size, sym_size, sym_size/2, GP_TRIANGLE_DOWN, ctx->text_color);
			}
		}

		if (header[i].label) {
			gp_print(ctx->buf, ctx->font_bold, cx, cy,
				GP_ALIGN_RIGHT|GP_VALIGN_BELOW,
				ctx->text_color, ctx->bg_color, "%s", header[i].label);
		}

		cx += tbl->cols_w[i] + ctx->padd;

		if (i < tbl->cols - 1) {
			gp_vline_xyh(ctx->buf, cx, y+1,
			            text_a + 2 * ctx->padd-1, ctx->bg_color);
		}

		cx += ctx->padd;
	}

	cy += text_a + ctx->padd;

	gp_pixel color = self->focused ? ctx->sel_color : ctx->text_color;

	gp_hline_xyw(ctx->buf, x, cy, self->w, color);

	return header_h(self, ctx);
}

static void align_text(gp_pixmap *buf, gp_text_style *font,
                       gp_widget_table *tbl,
                       const gp_widget_render_ctx *ctx,
		       unsigned int x, unsigned int y,
		       unsigned int col, gp_pixel bg, const char *str)
{
	gp_text_fit(buf, font, x, y, tbl->cols_w[col],
	           GP_ALIGN_RIGHT|GP_VALIGN_BELOW,
	           ctx->text_color, bg, str);
}

static unsigned int last_row(gp_widget *self)
{
	int ret;

	ret = self->tbl->row(self, GP_TABLE_ROW_TELL, 0);
	if (ret >= 0)
		return ret;

	ret = 0;

	if (!self->tbl->row(self, GP_TABLE_ROW_RESET, 0))
		return 0;

	while (self->tbl->row(self, GP_TABLE_ROW_ADVANCE, 1))
		ret++;

	return ret + 1;
}

static void fix_selected_row(gp_widget_table *tbl)
{
	if (tbl->selected_row >= tbl->last_rows)
		tbl->selected_row = tbl->last_rows - 1;
}

static void render(gp_widget *self, const gp_offset *offset,
                   const gp_widget_render_ctx *ctx, int flags)
{
	struct gp_widget_table *tbl = self->tbl;
	unsigned int text_a = gp_text_ascent(ctx->font);
	unsigned int x = self->x + offset->x;
	unsigned int y = self->y + offset->y;
	unsigned int w = self->w;
	unsigned int h = self->h;
	unsigned int cy = y;
	unsigned int i, j;

	(void)flags;

	gp_widget_ops_blit(ctx, x, y, w, h);

	gp_pixel color = self->focused ? ctx->sel_color : ctx->text_color;
	gp_fill_rrect_xywh(ctx->buf, x, y, w, h, ctx->bg_color, ctx->fg_color, color);

	cy += header_render(self, x, y, ctx);

	tbl->last_rows = last_row(self);
	fix_selected_row(tbl);

	if (tbl->start_row > tbl->last_rows)
		tbl->start_row = 0;

	tbl->row(self, GP_TABLE_ROW_RESET, 0);
	tbl->row(self, GP_TABLE_ROW_ADVANCE, tbl->start_row);

	unsigned int cur_row = tbl->start_row;
	unsigned int rows = display_rows(self, ctx);

	unsigned int cx = x + ctx->padd;

	for (j = 0; j < tbl->cols-1; j++) {
		cx += tbl->cols_w[j] + ctx->padd;
		gp_vline_xyy(ctx->buf, cx, cy+1, self->y + offset->y + self->h - 2, ctx->bg_color);
		cx += ctx->padd;
	}

	cy += ctx->padd/2;
	for (i = 0; i < rows; i++) {
		cx = x + ctx->padd;
		gp_pixel bg_col = ctx->fg_color;

		if (tbl->row_selected && cur_row == tbl->selected_row) {
			bg_col = self->focused ? ctx->sel_color : ctx->bg_color;

			gp_fill_rect_xywh(ctx->buf, x+1, cy - ctx->padd/2+1,
					self->w - 2,
					text_a + ctx->padd-1, bg_col);
		}

		if (cur_row < tbl->last_rows) {
			for (j = 0; j < tbl->cols; j++) {
				gp_widget_table_cell *cell = tbl->get(self, j);
				gp_text_style *font = cell->bold ? ctx->font_bold : ctx->font;

				align_text(ctx->buf, font, tbl, ctx, cx, cy, j, bg_col, cell->text);

				cx += tbl->cols_w[j] + ctx->padd;

			//	if (j < tbl->cols - 1) {
			//		gp_vline_xyh(ctx->buf, cx, cy,
			//			    text_a, ctx->text_color);
			//	}

				cx += ctx->padd;
			}
		}

		cy += text_a + ctx->padd;

		tbl->row(self, GP_TABLE_ROW_ADVANCE, 1);
		cur_row++;

		gp_hline_xyw(ctx->buf, x+1, cy - ctx->padd/2, w-2, ctx->bg_color);
	}
}

static int move_down(gp_widget *self, const gp_widget_render_ctx *ctx,
                     unsigned int rows)
{
	gp_widget_table *tbl = self->tbl;

	if (!tbl->row_selected) {
		tbl->row_selected = 1;
		tbl->selected_row = tbl->start_row;

		fix_selected_row(tbl);

		goto redraw;
	}

	if (tbl->selected_row < tbl->last_rows) {
		tbl->selected_row += rows;

		fix_selected_row(tbl);

		goto redraw;
	}

	return 0;

redraw:
	rows = display_rows(self, ctx);

	if (tbl->selected_row >= tbl->start_row + rows)
		tbl->start_row = tbl->selected_row - rows + 1;

	gp_widget_redraw(self);
	return 1;
}

static int scroll_down(gp_widget *self, const gp_widget_render_ctx *ctx,
                       unsigned int rows)
{
	gp_widget_table *tbl = self->tbl;
	unsigned int disp_rows = display_rows(self, ctx);

	if (tbl->last_rows < disp_rows)
		return 0;

	unsigned int hidden_rows = tbl->last_rows - disp_rows;

	if (tbl->start_row >= hidden_rows)
		return 0;

	if (tbl->start_row + rows >= hidden_rows)
		tbl->start_row = hidden_rows;
	else
		tbl->start_row += rows;

	gp_widget_redraw(self);
	return 1;
}

static int scroll_up(gp_widget *self, unsigned int rows)
{
	gp_widget_table *tbl = self->tbl;

	if (!tbl->start_row)
		return 0;

	if (rows > tbl->start_row)
		tbl->start_row = 0;
	else
		tbl->start_row -= rows;

	gp_widget_redraw(self);
	return 1;
}

static int move_up(gp_widget *self, const gp_widget_render_ctx *ctx, unsigned int rows)
{
	gp_widget_table *tbl = self->tbl;

	if (!tbl->row_selected) {
		tbl->row_selected = 1;
		tbl->selected_row = tbl->start_row + display_rows(self, ctx) - 1;

		goto redraw;
	}

	if (tbl->selected_row > 0) {
		if (tbl->selected_row > rows)
			tbl->selected_row -= rows;
		else
			tbl->selected_row = 0;

		goto redraw;
	}

	return 0;

redraw:
	if (tbl->selected_row < tbl->start_row)
		tbl->start_row = tbl->selected_row;

	gp_widget_redraw(self);
	return 1;
}

static int header_click(gp_widget *self, const gp_widget_render_ctx *ctx, unsigned int x)
{
	gp_widget_table *tbl = self->tbl;
	unsigned int i, cx = 0;

	//TODO: inverval division?
	for (i = 0; i < tbl->cols-1; i++) {
		cx += tbl->cols_w[i] + 2 * ctx->padd;

		if (x <= cx)
			break;
	}

	if (!tbl->header[i].sortable)
		return 0;

	if (!tbl->sort) {
		GP_BUG("No sort fuction defined!");
		return 1;
	}

	if (tbl->sorted_by_col == i)
		tbl->sorted_desc = !tbl->sorted_desc;
	else
		tbl->sorted_by_col = i;

	tbl->sort(self, tbl->sorted_by_col, tbl->sorted_desc);

	gp_widget_redraw(self);
	return 1;
}

static int row_click(gp_widget *self, const gp_widget_render_ctx *ctx, gp_event *ev)
{
	gp_widget_table *tbl = self->tbl;
	unsigned int row = ev->cursor_y - header_h(self, ctx);

	row /= row_h(ctx);
	row += tbl->start_row;

	if (tbl->row_selected && tbl->selected_row == row) {
		if (gp_timeval_diff_ms(ev->time, tbl->last_ev) < ctx->dclick_ms)
			gp_widget_send_widget_event(self, 0);
		goto ret;
	}

	tbl->selected_row = row;
	if (!tbl->row_selected)
		tbl->row_selected = 1;

	gp_widget_redraw(self);

ret:
	tbl->last_ev = ev->time;
	return 1;
}

static int click(gp_widget *self, const gp_widget_render_ctx *ctx, gp_event *ev)
{
	if (ev->cursor_y <= header_h(self, ctx))
		return header_click(self, ctx, ev->cursor_x);

	return row_click(self, ctx, ev);
}

static int enter(gp_widget *self)
{
	gp_widget_table *tbl = self->tbl;

	if (!tbl->row_selected)
		return 0;

	gp_widget_send_widget_event(self, 0);

	return 1;
}

static int event(gp_widget *self, const gp_widget_render_ctx *ctx, gp_event *ev)
{
	switch (ev->type) {
	case GP_EV_KEY:
		if (ev->code == GP_EV_KEY_UP)
			return 0;

		switch (ev->val) {
		case GP_KEY_DOWN:
			if (gp_widget_event_key_pressed(ev, GP_KEY_LEFT_SHIFT))
				return move_down(self, ctx, 10);

			return move_down(self, ctx, 1);
		break;
		case GP_KEY_UP:
			if (gp_widget_event_key_pressed(ev, GP_KEY_LEFT_SHIFT))
				return move_up(self, ctx, 10);

			return move_up(self, ctx, 1);
		break;
		case GP_KEY_PAGE_UP:
			return move_up(self, ctx, display_rows(self, ctx));
		case GP_KEY_PAGE_DOWN:
			return move_down(self, ctx, display_rows(self, ctx));
		case GP_BTN_LEFT:
			return click(self, ctx, ev);
		case GP_KEY_ENTER:
			return enter(self);
		}
	break;
	case GP_EV_REL:
		if (ev->code != GP_EV_REL_WHEEL)
			return 0;

		if (ev->val < 0)
			scroll_down(self, ctx, -ev->val);
		else
			scroll_up(self, ev->val);
	break;
	}

	return 0;
}

static void free_header(gp_widget_table_header *header, unsigned int cols)
{
	unsigned int i;

	for (i = 0; i < cols; i++)
		free(header[i].label);

	free(header);
}

enum header_keys {
	FILL,
	LABEL,
	MIN_SIZE,
	SORTABLE
};

static const gp_json_obj_attr header_attrs[] = {
	GP_JSON_OBJ_ATTR("fill", GP_JSON_INT),
	GP_JSON_OBJ_ATTR("label", GP_JSON_STR),
	GP_JSON_OBJ_ATTR("min_size", GP_JSON_INT),
	GP_JSON_OBJ_ATTR("sortable", GP_JSON_BOOL),
};

static const gp_json_obj header_obj_filter = {
	.attrs = header_attrs,
	.attr_cnt = GP_ARRAY_SIZE(header_attrs),
};

static gp_widget_table_header *parse_header(gp_json_buf *json, gp_json_val *val, int *cols)
{
	gp_json_state state = gp_json_state_start(json);
	gp_widget_table_header *header;
	int cnt = 0;

	GP_JSON_ARR_FOREACH(json, val) {
		switch (val->type) {
		case GP_JSON_OBJ:
			cnt++;
			gp_json_obj_skip(json);
		break;
		default:
			gp_json_warn(json, "Invalid table header type!");
			//TODO skip array
		}
	}

	if (*cols != -1 && *cols != cnt) {
		gp_json_warn(json, "Table header size is not equal to number of columns");
		return NULL;
	}

	*cols = cnt;

	header = malloc(sizeof(*header) * (*cols));
	if (!header)
		return NULL;

	memset(header, 0, sizeof(*header) * (*cols));

	gp_json_state_load(json, state);

	cnt = 0;

	GP_JSON_ARR_FOREACH(json, val) {
		//TODO SKIP ARR
		if (val->type != GP_JSON_OBJ)
			continue;

		GP_JSON_OBJ_FILTER(json, val, &header_obj_filter, NULL) {
			switch (val->idx) {
			case FILL:
				header[cnt].col_fill = val->val_int;
			break;
			case LABEL:
				header[cnt].label = strdup(val->val_str);
			break;
			case MIN_SIZE:
				header[cnt].col_min_size = val->val_int;
			break;
			case SORTABLE:
				header[cnt].sortable = val->val_bool;
			break;
			}
		}
		cnt++;
	}

	return header;
}

enum keys {
	COLS,
	GET_ELEM,
	HEADER,
	MIN_ROWS,
	SET_ROW,
	SORT,
};

static const gp_json_obj_attr attrs[] = {
	GP_JSON_OBJ_ATTR("cols", GP_JSON_INT),
	GP_JSON_OBJ_ATTR("get_elem", GP_JSON_STR),
	GP_JSON_OBJ_ATTR("header", GP_JSON_ARR),
	GP_JSON_OBJ_ATTR("min_rows", GP_JSON_INT),
	GP_JSON_OBJ_ATTR("set_row", GP_JSON_STR),
	GP_JSON_OBJ_ATTR("sort", GP_JSON_STR),
};

static const gp_json_obj obj_filter = {
	.attrs = attrs,
	.attr_cnt = GP_ARRAY_SIZE(attrs),
};

static gp_widget *json_to_table(gp_json_buf *json, gp_json_val *val, gp_htable **uids)
{
	int cols = -1, min_rows = -1;
	void *set_row = NULL, *get_elem = NULL, *sort = NULL;
	gp_widget_table_header *table_header = NULL;

	(void)uids;

	GP_JSON_OBJ_FILTER(json, val, &obj_filter, gp_widget_json_attrs) {
		switch (val->idx) {
		case COLS:
			cols = val->val_int;
		break;
		case MIN_ROWS:
			min_rows = val->val_int;
		break;
		case SET_ROW:
			set_row = gp_widget_callback_addr(val->val_str);
		break;
		case GET_ELEM:
			get_elem = gp_widget_callback_addr(val->val_str);
		break;
		case SORT:
			sort = gp_widget_callback_addr(val->val_str);
		break;
		case HEADER:
			table_header = parse_header(json, val, &cols);
		break;
		}
	}

	if (!table_header)
		return NULL;

	if (cols < 0) {
		gp_json_warn(json, "Invalid or missing cols!");
		goto err;
	}

	if (min_rows < 0) {
		gp_json_warn(json, "Invalid or missing min_rows!");
		goto err;
	}

	if (!set_row) {
		gp_json_warn(json, "Invalid or missing set_row callback!");
		goto err;
	}

	if (!get_elem) {
		gp_json_warn(json, "Invalid or missing get_elem callback!");
		goto err;
	}

	gp_widget *table = gp_widget_table_new(cols, min_rows, table_header, set_row, get_elem);
	if (!table)
		goto err;

	table->tbl->free = table_header;
	table->tbl->sort = sort;

	return table;
err:
	free_header(table_header, cols);
	return NULL;
}

static void _free(gp_widget *self)
{
	if (!self->tbl->free)
		return;

	free_header(self->tbl->free, self->tbl->cols);
}

struct gp_widget_ops gp_widget_table_ops = {
	.min_w = min_w,
	.min_h = min_h,
	.distribute_size = distribute_size,
	.render = render,
	.event = event,
	.from_json = json_to_table,
	.free = _free,
	.id = "table",
};

gp_widget *gp_widget_table_new(unsigned int cols, unsigned int min_rows,
                               const gp_widget_table_header *header,
                               int (*row)(struct gp_widget *self,
                                          int op, unsigned int pos),
                               gp_widget_table_cell *(get)(struct gp_widget *self,
                                                           unsigned int col))
{
	gp_widget *ret;
	size_t size = sizeof(struct gp_widget_table);

	size += cols * sizeof(unsigned int);

	ret = gp_widget_new(GP_WIDGET_TABLE, GP_WIDGET_CLASS_NONE, size);
	if (!ret)
		return NULL;

	ret->tbl->cols = cols;
	ret->tbl->min_rows = min_rows;
	ret->tbl->start_row = 0;
	ret->tbl->cols_w = (void*)ret->tbl->buf;
	ret->tbl->header = header;

	ret->tbl->get = get;
	ret->tbl->row = row;

	return ret;
}

void gp_widget_table_refresh(gp_widget *self)
{
	gp_widget_redraw(self);
}

void gp_widget_table_set_offset(gp_widget *self, unsigned int off)
{
	GP_WIDGET_ASSERT(self, GP_WIDGET_TABLE, );

	self->tbl->start_row = off;

	gp_widget_redraw(self);
}
