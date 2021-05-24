// SPDX-License-Identifier: GPL-2.1-or-later
/*
 * Copyright (C) 2021 Cyril Hrubis <metan@ucw.cz>
 */

#include <string.h>
#include <widgets/gp_widgets.h>
#include "tst_test.h"
#include "common.h"

struct tcase {
	const char *json;
	const char *label;
	int type;
};

static int button_json_load(struct tcase *t)
{
	gp_widget *button;

	button = gp_widget_from_json_str(t->json, NULL);
	if (!button) {
		tst_msg("Failed to load JSON");
		return TST_FAILED;
	}

	if (button->type != GP_WIDGET_BUTTON) {
		tst_msg("Wrong widget type!");
		return TST_FAILED;
	}

	if (t->label) {
		if (strcmp(t->label, button->button->label)) {
			tst_msg("Wrong button label!");
			return TST_FAILED;
		}
	} else {
		if (button->button->label) {
			tst_msg("Non-NULL label!");
			return TST_FAILED;
		}
	}

	if (t->type != button->button->type) {
		tst_msg("Wrong button type!");
		return TST_FAILED;
	}

	gp_widget_free(button);

	return TST_SUCCESS;
}

static struct tcase labeled_button = {
	.json = "{\"version\": 1, \"type\": \"button\", \"label\": \"Label!\"}",
	.label = "Label!",
};

static struct tcase stock_button = {
	.json = "{\"version\": 1, \"type\": \"button\", \"btype\": \"cancel\"}",
	.type = GP_BUTTON_CANCEL | GP_BUTTON_TEXT_RIGHT,
};

static struct tcase stock_label_button = {
	.json = "{\"version\": 1, \"type\": \"button\", \"label\": \"Label!\", \"btype\": \"ok\"}",
	.label = "Label!",
	.type = GP_BUTTON_OK | GP_BUTTON_TEXT_RIGHT,
};

static struct tcase text_align_button = {
	.json = "{\"version\": 1, \"type\": \"button\", \"label\": \"Label!\","
		" \"btype\": \"ok\", \"text_align\": \"left\"}",
	.label = "Label!",
	.type = GP_BUTTON_OK | GP_BUTTON_TEXT_LEFT,
};

const struct tst_suite tst_suite = {
	.suite_name = "button JSON testsuite",
	.tests = {
		{.name = "button labeled",
		 .tst_fn = button_json_load,
		 .data = &labeled_button,
		 .flags = TST_CHECK_MALLOC},

		{.name = "button stock",
		 .tst_fn = button_json_load,
		 .data = &stock_button,
		 .flags = TST_CHECK_MALLOC},

		{.name = "button stock labeled",
		 .tst_fn = button_json_load,
		 .data = &stock_label_button,
		 .flags = TST_CHECK_MALLOC},

		{.name = "button text align",
		 .tst_fn = button_json_load,
		 .data = &text_align_button,
		 .flags = TST_CHECK_MALLOC},

		{.name = NULL},
	}
};
