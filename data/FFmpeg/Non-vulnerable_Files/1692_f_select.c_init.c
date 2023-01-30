static int request_frame(AVFilterLink *outlink);
static av_cold int init(AVFilterContext *ctx)
{
    SelectContext *select = ctx->priv;
    int i, ret;
    if ((ret = av_expr_parse(&select->expr, select->expr_str,
                             var_names, NULL, NULL, NULL, NULL, 0, ctx)) < 0) {
        av_log(ctx, AV_LOG_ERROR, "Error while parsing expression '%s'\n",
               select->expr_str);
        return ret;
    }
    select->do_scene_detect = !!strstr(select->expr_str, "scene");
    for (i = 0; i < select->nb_outputs; i++) {
        AVFilterPad pad = { 0 };
        pad.name = av_asprintf("output%d", i);
        if (!pad.name)
            return AVERROR(ENOMEM);
        pad.type = ctx->filter->inputs[0].type;
        pad.request_frame = request_frame;
        ff_insert_outpad(ctx, i, &pad);
    }
    return 0;
}
