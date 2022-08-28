static av_cold int alloc_buffers(AVCodecContext *avctx, AACEncContext *s)
{
    int ch;
    FF_ALLOCZ_ARRAY_OR_GOTO(avctx, s->buffer.samples, s->channels, 3 * 1024 * sizeof(s->buffer.samples[0]), alloc_fail);
    FF_ALLOCZ_ARRAY_OR_GOTO(avctx, s->cpe, s->chan_map[0], sizeof(ChannelElement), alloc_fail);
    FF_ALLOCZ_OR_GOTO(avctx, avctx->extradata, 5 + AV_INPUT_BUFFER_PADDING_SIZE, alloc_fail);
    for(ch = 0; ch < s->channels; ch++)
        s->planar_samples[ch] = s->buffer.samples + 3 * 1024 * ch;
    return 0;
alloc_fail:
    return AVERROR(ENOMEM);
}
