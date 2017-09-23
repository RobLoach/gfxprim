@ def filter_point_ex(op_name, filter_op, fopts):
int GP_Filter{{ op_name }}Ex(const GP_Pixmap *const src,
                             GP_Coord x_src, GP_Coord y_src,
                             GP_Size w_src, GP_Size h_src,
                             GP_Pixmap *dst,
                             GP_Coord x_dst, GP_Coord y_dst,
                             {{ maybe_opts_r(fopts) }}
                             GP_ProgressCallback *callback)
{
	const GP_PixelTypeDescription *desc;
	GP_FilterTables tables;
	unsigned int i;
	GP_Pixel j;
	int ret, err;

	if (GP_FilterTablesInit(&tables, src))
		return 1;

	desc = GP_PixelTypeDesc(src->pixel_type);

	for (i = 0; i < desc->numchannels; i++) {
		GP_Pixel chan_max = (1 << desc->channels[i].size);
		GP_Pixel *table = tables.table[i];

		for (j = 0; j < chan_max; j++)
			table[j] = {@ filter_op('((signed)j)', '((signed)chan_max - 1)') @};
	}

	ret = GP_FilterTablesApply(src, x_src, y_src, w_src, h_src,
	                           dst, x_dst, y_dst, &tables, callback);

	err = errno;
	GP_FilterTablesFree(&tables);
	errno = err;

	return ret;
}
@
@ def filter_point_ex_alloc(op_name, fopts, opts):
GP_Pixmap *GP_Filter{{ op_name }}ExAlloc(const GP_Pixmap *const src,
                                          GP_Coord x_src, GP_Coord y_src,
                                          GP_Size w_src, GP_Size h_src,
                                          {{ maybe_opts_r(fopts) }}
                                          GP_ProgressCallback *callback)
{
	GP_Pixmap *new = GP_PixmapAlloc(w_src, h_src,
	                                  src->pixel_type);

	if (GP_Filter{{ op_name }}Ex(src, x_src, y_src, w_src, h_src, new, 0, 0,
	                             {{ maybe_opts_r(opts) }}callback)) {
		int err = errno;
		GP_PixmapFree(new);
		errno = err;
		return NULL;
	}

	return new;
}
@
@ def filter_point(op_name, filter_op, fopts="", opts=""):
#include <errno.h>

#include "core/GP_Debug.h"

#include "filters/GP_ApplyTables.h"
#include "filters/GP_Point.h"

{@ filter_point_ex(op_name, filter_op, fopts) @}
{@ filter_point_ex_alloc(op_name, fopts, opts) @}
@ end
