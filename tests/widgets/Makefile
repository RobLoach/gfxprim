TOPDIR=../..

include $(TOPDIR)/pre.mk

CSOURCES=tbox.c tattr.c button.c checkbox.c tabs.c label.c grid.c size_units.c\
	 button_json.c grid_json.c checkbox_json.c label_json.c json.c

APPS=tbox tattr button checkbox tabs label grid size_units button_json\
     grid_json checkbox_json label_json json

LDLIBS+=$(shell $(TOPDIR)/gfxprim-config --libs-widgets)

include ../tests.mk

include $(TOPDIR)/gen.mk
include $(TOPDIR)/app.mk
include $(TOPDIR)/post.mk
