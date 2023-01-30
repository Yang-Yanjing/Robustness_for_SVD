AVFILTER_DEFINE_CLASS(field);
static int config_props_output(AVFilterLink *outlink)
{
    AVFilterContext *ctx = outlink->src;
    FieldContext *field = ctx->priv;
    AVFilterLink *inlink = ctx->inputs[0];
    field->nb_planes = av_pix_fmt_count_planes(outlink->format);
    outlink->w = inlink->w;
    outlink->h = (inlink->h + (field->type == FIELD_TYPE_TOP)) / 2;
    av_log(ctx, AV_LOG_VERBOSE, "w:%d h:%d type:%s -> w:%d h:%d\n",
           inlink->w, inlink->h, field->type == FIELD_TYPE_BOTTOM ? "bottom" : "top",
           outlink->w, outlink->h);
    return 0;
}
