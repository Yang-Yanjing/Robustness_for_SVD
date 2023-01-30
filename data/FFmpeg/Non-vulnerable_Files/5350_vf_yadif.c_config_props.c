static int config_props(AVFilterLink *link)
{
    AVFilterContext *ctx = link->src;
    YADIFContext *s = link->src->priv;
    link->time_base.num = link->src->inputs[0]->time_base.num;
    link->time_base.den = link->src->inputs[0]->time_base.den * 2;
    link->w             = link->src->inputs[0]->w;
    link->h             = link->src->inputs[0]->h;
    if(s->mode&1)
        link->frame_rate = av_mul_q(link->src->inputs[0]->frame_rate, (AVRational){2,1});
    if (link->w < 3 || link->h < 3) {
        av_log(ctx, AV_LOG_ERROR, "Video of less than 3 columns or lines is not supported\n");
        return AVERROR(EINVAL);
    }
    s->csp = av_pix_fmt_desc_get(link->format);
    if (s->csp->comp[0].depth_minus1 / 8 == 1) {
        s->filter_line  = filter_line_c_16bit;
        s->filter_edges = filter_edges_16bit;
    } else {
        s->filter_line  = filter_line_c;
        s->filter_edges = filter_edges;
    }
    if (ARCH_X86)
        ff_yadif_init_x86(s);
    return 0;
}
