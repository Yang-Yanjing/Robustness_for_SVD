AVRational av_buffersink_get_frame_rate(AVFilterContext *ctx)
{
    av_assert0(   !strcmp(ctx->filter->name, "buffersink")
               || !strcmp(ctx->filter->name, "ffbuffersink"));
    return ctx->inputs[0]->frame_rate;
}
