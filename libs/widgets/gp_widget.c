//SPDX-License-Identifier: LGPL-2.0-or-later

/*

   Copyright (c) 2014-2020 Cyril Hrubis <metan@ucw.cz>

 */

#include <string.h>
#include <core/gp_debug.h>
#include <core/gp_common.h>
#include <widgets/gp_widget.h>
#include <widgets/gp_widget_ops.h>

const char *gp_widget_class_name(enum gp_widget_class widget_class)
{
	switch (widget_class) {
	case GP_WIDGET_CLASS_NONE:
		return "none";
	case GP_WIDGET_CLASS_BOOL:
		return "bool";
	case GP_WIDGET_CLASS_INT:
		return "int";
	case GP_WIDGET_CLASS_CHOICE:
		return "choice";
	default:
		return "???";
	}
}

gp_widget *gp_widget_new(enum gp_widget_type type,
                         enum gp_widget_class widget_class,
                         size_t payload_size)
{
	size_t size = sizeof(gp_widget) + payload_size;
	gp_widget *ret = malloc(size);

	GP_DEBUG(1, "Allocating widget %s class %s payload_size=%zu size=%zu",
	         gp_widget_type_name(type), gp_widget_class_name(widget_class),
	         payload_size, size);

	if (!ret) {
		GP_WARN("Malloc failed :-(");
		return NULL;
	}

	memset(ret, 0, size);
	ret->payload = ret->buf;
	ret->type = type;
	ret->widget_class = widget_class;

	ret->event_mask = GP_WIDGET_DEFAULT_EVENT_MASK;

	return ret;
}

void gp_widget_set_parent(gp_widget *self, gp_widget *parent)
{
	if (!self)
		return;

	//TODO: reparent?
	if (self->parent)
		GP_WARN("Widget %p has already parent %p!", self, self->parent);

	self->parent = parent;
}

void gp_widget_free(gp_widget *self)
{
	const struct gp_widget_ops *ops;

	if (!self)
		return;

	gp_widget_send_event(self, GP_WIDGET_EVENT_FREE);

	gp_widget_ops_for_each_child(self, gp_widget_free);

	ops = gp_widget_ops(self);
	if (ops->free)
		ops->free(self);

	free(self);
}
