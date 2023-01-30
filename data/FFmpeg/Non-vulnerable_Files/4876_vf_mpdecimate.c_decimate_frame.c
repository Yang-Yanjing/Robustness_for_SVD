static int decimate_frame(AVFilterContext *ctx,
                          AVFrame *cur, AVFrame *ref)
{
    DecimateContext *decimate = ctx->priv;
    int plane;
    if (decimate->max_drop_count > 0 &&
        decimate->drop_count >= decimate->max_drop_count)
        return 0;
    if (decimate->max_drop_count < 0 &&
        (decimate->drop_count-1) > decimate->max_drop_count)
        return 0;
    for (plane = 0; ref->data[plane] && ref->linesize[plane]; plane++) {
        




        int vsub = plane == 1 || plane == 2 ? decimate->vsub : 0;
        int hsub = plane == 1 || plane == 2 ? decimate->hsub : 0;
        if (diff_planes(ctx,
                        cur->data[plane], cur->linesize[plane],
                        ref->data[plane], ref->linesize[plane],
                        FF_CEIL_RSHIFT(ref->width,  hsub),
                        FF_CEIL_RSHIFT(ref->height, vsub)))
            return 0;
    }
    return 1;
}
