static int config_props(AVFilterLink *inlink)
{
    VignetteContext *s = inlink->dst->priv;
    AVRational sar = inlink->sample_aspect_ratio;
    s->desc = av_pix_fmt_desc_get(inlink->format);
    s->var_values[VAR_W]  = inlink->w;
    s->var_values[VAR_H]  = inlink->h;
    s->var_values[VAR_TB] = av_q2d(inlink->time_base);
    s->var_values[VAR_R]  = inlink->frame_rate.num == 0 || inlink->frame_rate.den == 0 ?
        NAN : av_q2d(inlink->frame_rate);
    if (!sar.num || !sar.den)
        sar.num = sar.den = 1;
    if (sar.num > sar.den) {
        s->xscale = av_q2d(av_div_q(sar, s->aspect));
        s->yscale = 1;
    } else {
        s->yscale = av_q2d(av_div_q(s->aspect, sar));
        s->xscale = 1;
    }
    s->dmax = hypot(inlink->w / 2., inlink->h / 2.);
    av_log(s, AV_LOG_DEBUG, "xscale=%f yscale=%f dmax=%f\n",
           s->xscale, s->yscale, s->dmax);
    s->fmap_linesize = FFALIGN(inlink->w, 32);
    s->fmap = av_malloc_array(s->fmap_linesize, inlink->h * sizeof(*s->fmap));
    if (!s->fmap)
        return AVERROR(ENOMEM);
    if (s->eval_mode == EVAL_MODE_INIT)
        update_context(s, inlink, NULL);
    return 0;
}
