static av_cold void uninit(AVFilterContext *ctx)
{
    SelectContext *select = ctx->priv;
    int i;
    av_expr_free(select->expr);
    select->expr = NULL;
    for (i = 0; i < ctx->nb_outputs; i++)
        av_freep(&ctx->output_pads[i].name);
    if (select->do_scene_detect) {
        av_frame_free(&select->prev_picref);
    }
}
