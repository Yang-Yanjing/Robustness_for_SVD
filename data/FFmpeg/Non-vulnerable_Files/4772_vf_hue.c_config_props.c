static int config_props(AVFilterLink *inlink)
{
    HueContext *hue = inlink->dst->priv;
    const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(inlink->format);
    hue->hsub = desc->log2_chroma_w;
    hue->vsub = desc->log2_chroma_h;
    hue->var_values[VAR_N]  = 0;
    hue->var_values[VAR_TB] = av_q2d(inlink->time_base);
    hue->var_values[VAR_R]  = inlink->frame_rate.num == 0 || inlink->frame_rate.den == 0 ?
        NAN : av_q2d(inlink->frame_rate);
    return 0;
}
