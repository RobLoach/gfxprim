//SPDX-License-Identifier: LGPL-2.0-or-later

/*

   Copyright (c) 2014-2020 Cyril Hrubis <metan@ucw.cz>

 */

#include <stdio.h>
#include <string.h>
#include <widgets/gp_widget_ops.h>
#include <widgets/gp_widget_event.h>

static const char *ev_names[] = {
	[GP_WIDGET_EVENT_NEW] = "new",
	[GP_WIDGET_EVENT_FREE] = "free",
	[GP_WIDGET_EVENT_WIDGET] = "widget",
	[GP_WIDGET_EVENT_INPUT] = "input",
	[GP_WIDGET_EVENT_REDRAW] = "redraw",
	[GP_WIDGET_EVENT_RESIZE] = "resize",
};

void gp_widget_event_mask(gp_widget *self, enum gp_widget_event_type ev_type)
{
	if (!self) {
		GP_WARN("NULL widget!");
		return;
	}

	self->event_mask &= ~(1<<ev_type);
}

void gp_widget_event_unmask(gp_widget *self, enum gp_widget_event_type ev_type)
{
	if (!self) {
		GP_WARN("NULL widget!");
		return;
	}

	self->event_mask |= (1<<ev_type);
}

const char *gp_widget_event_type_name(enum gp_widget_event_type ev_type)
{
	if (ev_type >= GP_WIDGET_EVENT_MAX)
		return "invalid";

	if (ev_type < 0)
		return "invalid";

	return ev_names[ev_type];
}

void gp_widget_event_dump(gp_widget_event *ev)
{
	printf("Event type %s for widget %p type %s sub_type %u\n",
		gp_widget_event_type_name(ev->type),
		ev->self, gp_widget_type_id(ev->self),
		(unsigned int) ev->sub_type);

	switch (ev->type) {
	case GP_WIDGET_EVENT_INPUT:
		gp_event_dump(ev->input_ev);
	break;
	default:
	break;
	}
}
