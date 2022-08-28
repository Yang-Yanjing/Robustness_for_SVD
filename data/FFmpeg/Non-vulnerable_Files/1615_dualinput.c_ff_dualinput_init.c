int ff_dualinput_init(AVFilterContext *ctx, FFDualInputContext *s)
{
    FFFrameSyncIn *in;
    int ret;
    if ((ret = ff_framesync_init(&s->fs, ctx, 2)) < 0)
        return ret;
    in = s->fs.in;
    s->fs.opaque = s;
    s->fs.on_event = process_frame;
    in[0].time_base = ctx->inputs[0]->time_base;
    in[1].time_base = ctx->inputs[1]->time_base;
    in[0].sync   = 2;
    in[0].before = EXT_STOP;
    in[0].after  = EXT_INFINITY;
    in[1].sync   = 1;
    in[1].before = EXT_NULL;
    in[1].after  = EXT_INFINITY;
    if (s->shortest)
        in[0].after = in[1].after = EXT_STOP;
    if (!s->repeatlast) {
        in[1].after = EXT_NULL;
        in[1].sync  = 0;
    }
    return ff_framesync_configure(&s->fs);
}
