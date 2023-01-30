static int yae_overlap_add(ATempoContext *atempo,
                           uint8_t **dst_ref,
                           uint8_t *dst_end)
{
    
    const AudioFragment *prev = yae_prev_frag(atempo);
    const AudioFragment *frag = yae_curr_frag(atempo);
    const int64_t start_here = FFMAX(atempo->position[1],
                                     frag->position[1]);
    const int64_t stop_here = FFMIN(prev->position[1] + prev->nsamples,
                                    frag->position[1] + frag->nsamples);
    const int64_t overlap = stop_here - start_here;
    const int64_t ia = start_here - prev->position[1];
    const int64_t ib = start_here - frag->position[1];
    const float *wa = atempo->hann + ia;
    const float *wb = atempo->hann + ib;
    const uint8_t *a = prev->data + ia * atempo->stride;
    const uint8_t *b = frag->data + ib * atempo->stride;
    uint8_t *dst = *dst_ref;
    av_assert0(start_here <= stop_here &&
               frag->position[1] <= start_here &&
               overlap <= frag->nsamples);
    if (atempo->format == AV_SAMPLE_FMT_U8) {
        yae_blend(uint8_t);
    } else if (atempo->format == AV_SAMPLE_FMT_S16) {
        yae_blend(int16_t);
    } else if (atempo->format == AV_SAMPLE_FMT_S32) {
        yae_blend(int);
    } else if (atempo->format == AV_SAMPLE_FMT_FLT) {
        yae_blend(float);
    } else if (atempo->format == AV_SAMPLE_FMT_DBL) {
        yae_blend(double);
    }
    
    *dst_ref = dst;
    return atempo->position[1] == stop_here ? 0 : AVERROR(EAGAIN);
}
