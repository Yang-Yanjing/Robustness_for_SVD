static av_cold void uninit(AVFilterContext *ctx)
{
    BlendContext *s = ctx->priv;
    int i;
    ff_dualinput_uninit(&s->dinput);
    av_frame_free(&s->prev_frame);
    for (i = 0; i < FF_ARRAY_ELEMS(s->params); i++)
        av_expr_free(s->params[i].e);
}
