//SPDX-License-Identifier: LGPL-2.0-or-later

/*

   Copyright (c) 2014-2021 Cyril Hrubis <metan@ucw.cz>

 */

#include <widgets/gp_widgets.h>

#include "dialog_info.json.h"
#include "dialog_warn.json.h"
#include "dialog_err.json.h"
#include "dialog_question.json.h"

static int do_no(gp_widget_event *ev)
{
	gp_dialog *wd = ev->self->priv;

	if (ev->type != GP_WIDGET_EVENT_WIDGET)
		return 0;

	wd->retval = GP_DIALOG_NO;

	return 0;
}

static int do_yes(gp_widget_event *ev)
{
	gp_dialog *wd = ev->self->priv;

	if (ev->type != GP_WIDGET_EVENT_WIDGET)
		return 0;

	wd->retval = GP_DIALOG_YES;

	return 0;
}

static gp_widget *load_layout(enum gp_dialog_msg_type type, const char *title,
                              gp_dialog *dialog, gp_widget **label)
{
	gp_widget *ret, *w;
	gp_htable *uids = NULL;

	switch (type) {
	case GP_DIALOG_MSG_INFO:
		ret = gp_dialog_layout_load("msg_info", dialog_info, &uids);
	break;
	case GP_DIALOG_MSG_WARN:
		ret = gp_dialog_layout_load("msg_warn", dialog_warn, &uids);
	break;
	case GP_DIALOG_MSG_ERR:
		ret = gp_dialog_layout_load("msg_err", dialog_err, &uids);
	break;
	case GP_DIALOG_MSG_QUESTION:
		ret = gp_dialog_layout_load("msg_question", dialog_question, &uids);
	break;
	default:
		GP_WARN("Invalid dialog type %i", type);
		return NULL;
	}

	if (!ret)
		return NULL;

	if (title) {
		w = gp_widget_by_uid(uids, "title", GP_WIDGET_FRAME);
		gp_widget_frame_title_set(w, title);
	}

	if (type == GP_DIALOG_MSG_QUESTION) {
		w = gp_widget_by_uid(uids, "btn_no", GP_WIDGET_BUTTON);
		if (w)
			gp_widget_event_handler_set(w, do_no, dialog);

		w = gp_widget_by_uid(uids, "btn_yes", GP_WIDGET_BUTTON);
		if (w)
			gp_widget_event_handler_set(w, do_yes, dialog);
	} else {
		w = gp_widget_by_uid(uids, "btn_ok", GP_WIDGET_BUTTON);
		if (w)
			gp_widget_event_handler_set(w, do_yes, dialog);
	}

	*label = gp_widget_by_uid(uids, "text", GP_WIDGET_LABEL);

	gp_htable_free(uids);

	return ret;
}

static int run_and_free(gp_dialog *dialog)
{
	long ret;

	ret = gp_dialog_run(dialog);
	gp_widget_free(dialog->layout);

	return ret;
}

int gp_dialog_msg_run(enum gp_dialog_msg_type type,
                      const char *title, const char *msg)
{
	gp_widget *label = NULL;
	gp_dialog dialog = {};

	dialog.layout = load_layout(type, title, &dialog, &label);
	if (!dialog.layout)
		return GP_DIALOG_ERR;

	if (label)
		gp_widget_label_set(label, msg);

	return run_and_free(&dialog);
}

int gp_dialog_msg_printf_run(enum gp_dialog_msg_type type,
                             const char *title, const char *fmt, ...)
{
	va_list ap;
	gp_widget *label = NULL;
	gp_dialog dialog = {};

	dialog.layout = load_layout(type, title, &dialog, &label);
	if (!dialog.layout)
		return GP_DIALOG_ERR;

	va_start(ap, fmt);
	if (label)
		gp_widget_label_vprintf(label, fmt, ap);
	va_end(ap);

	return run_and_free(&dialog);
}
