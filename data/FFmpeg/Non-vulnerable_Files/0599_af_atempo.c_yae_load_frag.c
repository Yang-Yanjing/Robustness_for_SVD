static int yae_load_frag(ATempoContext *atempo,
                         const uint8_t **src_ref,
                         const uint8_t *src_end)
{
    
    AudioFragment *frag = yae_curr_frag(atempo);
    uint8_t *dst;
    int64_t missing, start, zeros;
    uint32_t nsamples;
    const uint8_t *a, *b;
    int i0, i1, n0, n1, na, nb;
    int64_t stop_here = frag->position[0] + atempo->window;
    if (src_ref && yae_load_data(atempo, src_ref, src_end, stop_here) != 0) {
        return AVERROR(EAGAIN);
    }
    
    missing =
        stop_here > atempo->position[0] ?
        stop_here - atempo->position[0] : 0;
    nsamples =
        missing < (int64_t)atempo->window ?
        (uint32_t)(atempo->window - missing) : 0;
    
    frag->nsamples = nsamples;
    dst = frag->data;
    start = atempo->position[0] - atempo->size;
    zeros = 0;
    if (frag->position[0] < start) {
        
        zeros = FFMIN(start - frag->position[0], (int64_t)nsamples);
        av_assert0(zeros != nsamples);
        memset(dst, 0, zeros * atempo->stride);
        dst += zeros * atempo->stride;
    }
    if (zeros == nsamples) {
        return 0;
    }
    
    na = (atempo->head < atempo->tail ?
          atempo->tail - atempo->head :
          atempo->ring - atempo->head);
    nb = atempo->head < atempo->tail ? 0 : atempo->tail;
    
    av_assert0(nsamples <= zeros + na + nb);
    a = atempo->buffer + atempo->head * atempo->stride;
    b = atempo->buffer;
    i0 = frag->position[0] + zeros - start;
    i1 = i0 < na ? 0 : i0 - na;
    n0 = i0 < na ? FFMIN(na - i0, (int)(nsamples - zeros)) : 0;
    n1 = nsamples - zeros - n0;
    if (n0) {
        memcpy(dst, a + i0 * atempo->stride, n0 * atempo->stride);
        dst += n0 * atempo->stride;
    }
    if (n1) {
        memcpy(dst, b + i1 * atempo->stride, n1 * atempo->stride);
    }
    return 0;
}
