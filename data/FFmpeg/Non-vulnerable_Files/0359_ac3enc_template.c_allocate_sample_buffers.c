                                 int len);
int AC3_NAME(allocate_sample_buffers)(AC3EncodeContext *s)
{
    int ch;
    FF_ALLOC_OR_GOTO(s->avctx, s->windowed_samples, AC3_WINDOW_SIZE *
                     sizeof(*s->windowed_samples), alloc_fail);
    FF_ALLOC_ARRAY_OR_GOTO(s->avctx, s->planar_samples, s->channels, sizeof(*s->planar_samples),
                     alloc_fail);
    for (ch = 0; ch < s->channels; ch++) {
        FF_ALLOCZ_OR_GOTO(s->avctx, s->planar_samples[ch],
                          (AC3_FRAME_SIZE+AC3_BLOCK_SIZE) * sizeof(**s->planar_samples),
                          alloc_fail);
    }
    return 0;
alloc_fail:
    return AVERROR(ENOMEM);
}
