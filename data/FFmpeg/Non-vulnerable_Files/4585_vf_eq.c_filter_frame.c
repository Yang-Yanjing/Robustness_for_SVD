static int filter_frame(AVFilterLink *inlink, AVFrame *in)
{
    AVFilterContext *ctx = inlink->dst;
    AVFilterLink *outlink = inlink->dst->outputs[0];
    EQContext *eq = ctx->priv;
    AVFrame *out;
    int64_t pos = av_frame_get_pkt_pos(in);
    const AVPixFmtDescriptor *desc;
    int i;
    out = ff_get_video_buffer(outlink, inlink->w, inlink->h);
    if (!out)
        return AVERROR(ENOMEM);
    av_frame_copy_props(out, in);
    desc = av_pix_fmt_desc_get(inlink->format);
    eq->var_values[VAR_N]   = inlink->frame_count;
    eq->var_values[VAR_POS] = pos == -1 ? NAN : pos;
    eq->var_values[VAR_T]   = TS2T(in->pts, inlink->time_base);
    if (eq->eval_mode == EVAL_MODE_FRAME) {
        set_gamma(eq);
        set_contrast(eq);
        set_brightness(eq);
        set_saturation(eq);
    }
    for (i = 0; i < desc->nb_components; i++) {
        int w = inlink->w;
        int h = inlink->h;
        if (i == 1 || i == 2) {
            w = FF_CEIL_RSHIFT(w, desc->log2_chroma_w);
            h = FF_CEIL_RSHIFT(h, desc->log2_chroma_h);
        }
        if (eq->param[i].adjust)
            eq->param[i].adjust(&eq->param[i], out->data[i], out->linesize[i],
                                 in->data[i], in->linesize[i], w, h);
        else
            av_image_copy_plane(out->data[i], out->linesize[i],
                                in->data[i], in->linesize[i], w, h);
    }
    av_frame_free(&in);
    return ff_filter_frame(outlink, out);
}
