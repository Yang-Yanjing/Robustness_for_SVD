static av_cold int init(AVFilterContext *ctx)
{
    InterleaveContext *s = ctx->priv;
    const AVFilterPad *outpad = &ctx->filter->outputs[0];
    int i;
    s->queues = av_calloc(s->nb_inputs, sizeof(s->queues[0]));
    if (!s->queues)
        return AVERROR(ENOMEM);
    for (i = 0; i < s->nb_inputs; i++) {
        AVFilterPad inpad = { 0 };
        inpad.name = av_asprintf("input%d", i);
        if (!inpad.name)
            return AVERROR(ENOMEM);
        inpad.type         = outpad->type;
        inpad.filter_frame = filter_frame;
        switch (outpad->type) {
        case AVMEDIA_TYPE_VIDEO:
            inpad.get_video_buffer = ff_null_get_video_buffer; break;
        case AVMEDIA_TYPE_AUDIO:
            inpad.get_audio_buffer = ff_null_get_audio_buffer; break;
        default:
            av_assert0(0);
        }
        ff_insert_inpad(ctx, i, &inpad);
    }
    return 0;
}
