static av_cold int init(AVFilterContext *ctx)
{
    FrameRateContext *s = ctx->priv;
    s->dest_frame_num = 0;
    s->crnt = (N_SRCE)>>1;
    s->last = N_SRCE - 1;
    s->next = s->crnt - 1;
    s->prev = s->crnt + 1;
    return 0;
}
