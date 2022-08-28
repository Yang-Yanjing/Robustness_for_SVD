static av_cold void framepack_uninit(AVFilterContext *ctx)
{
    FramepackContext *s = ctx->priv;
    
    av_frame_free(&s->input_views[LEFT]);
    av_frame_free(&s->input_views[RIGHT]);
}
