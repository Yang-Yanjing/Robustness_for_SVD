static int config_props(AVFilterLink *outlink)
{
    AVFilterContext *ctx = outlink->src;
    TileContext *tile    = ctx->priv;
    AVFilterLink *inlink = ctx->inputs[0];
    const unsigned total_margin_w = (tile->w - 1) * tile->padding + 2*tile->margin;
    const unsigned total_margin_h = (tile->h - 1) * tile->padding + 2*tile->margin;
    if (inlink->w > (INT_MAX - total_margin_w) / tile->w) {
        av_log(ctx, AV_LOG_ERROR, "Total width %ux%u is too much.\n",
               tile->w, inlink->w);
        return AVERROR(EINVAL);
    }
    if (inlink->h > (INT_MAX - total_margin_h) / tile->h) {
        av_log(ctx, AV_LOG_ERROR, "Total height %ux%u is too much.\n",
               tile->h, inlink->h);
        return AVERROR(EINVAL);
    }
    outlink->w = tile->w * inlink->w + total_margin_w;
    outlink->h = tile->h * inlink->h + total_margin_h;
    outlink->sample_aspect_ratio = inlink->sample_aspect_ratio;
    outlink->frame_rate = av_mul_q(inlink->frame_rate,
                                   av_make_q(1, tile->nb_frames));
    ff_draw_init(&tile->draw, inlink->format, 0);
    ff_draw_color(&tile->draw, &tile->blank, tile->rgba_color);
    outlink->flags |= FF_LINK_FLAG_REQUEST_LOOP;
    return 0;
}
