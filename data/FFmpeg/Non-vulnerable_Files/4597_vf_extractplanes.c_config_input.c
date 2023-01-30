static int config_input(AVFilterLink *inlink)
{
    AVFilterContext *ctx = inlink->dst;
    ExtractPlanesContext *s = ctx->priv;
    const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(inlink->format);
    int plane_avail, ret, i;
    uint8_t rgba_map[4];
    plane_avail = ((desc->flags & AV_PIX_FMT_FLAG_RGB) ? PLANE_R|PLANE_G|PLANE_B :
                                                 PLANE_Y |
                                ((desc->nb_components > 2) ? PLANE_U|PLANE_V : 0)) |
                  ((desc->flags & AV_PIX_FMT_FLAG_ALPHA) ? PLANE_A : 0);
    if (s->requested_planes & ~plane_avail) {
        av_log(ctx, AV_LOG_ERROR, "Requested planes not available.\n");
        return AVERROR(EINVAL);
    }
    if ((ret = av_image_fill_linesizes(s->linesize, inlink->format, inlink->w)) < 0)
        return ret;
    s->depth = (desc->comp[0].depth_minus1 + 1) >> 3;
    s->step = av_get_padded_bits_per_pixel(desc) >> 3;
    s->is_packed = !(desc->flags & AV_PIX_FMT_FLAG_PLANAR) &&
                    (desc->nb_components > 1);
    if (desc->flags & AV_PIX_FMT_FLAG_RGB) {
        ff_fill_rgba_map(rgba_map, inlink->format);
        for (i = 0; i < 4; i++)
            s->map[i] = rgba_map[s->map[i]];
    }
    return 0;
}
