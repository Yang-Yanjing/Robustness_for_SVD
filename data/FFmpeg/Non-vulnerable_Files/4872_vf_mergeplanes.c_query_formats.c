static int query_formats(AVFilterContext *ctx)
{
    MergePlanesContext *s = ctx->priv;
    AVFilterFormats *formats = NULL;
    int i;
    s->outdesc = av_pix_fmt_desc_get(s->out_fmt);
    for (i = 0; av_pix_fmt_desc_get(i); i++) {
        const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(i);
        if (desc->comp[0].depth_minus1 == s->outdesc->comp[0].depth_minus1 &&
            av_pix_fmt_count_planes(i) == desc->nb_components)
            ff_add_format(&formats, i);
    }
    for (i = 0; i < s->nb_inputs; i++)
        ff_formats_ref(formats, &ctx->inputs[i]->out_formats);
    formats = NULL;
    ff_add_format(&formats, s->out_fmt);
    ff_formats_ref(formats, &ctx->outputs[0]->in_formats);
    return 0;
}
