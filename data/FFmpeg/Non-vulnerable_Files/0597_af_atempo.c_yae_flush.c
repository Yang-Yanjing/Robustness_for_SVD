static int yae_flush(ATempoContext *atempo,
                     uint8_t **dst_ref,
                     uint8_t *dst_end)
{
    AudioFragment *frag = yae_curr_frag(atempo);
    int64_t overlap_end;
    int64_t start_here;
    int64_t stop_here;
    int64_t offset;
    const uint8_t *src;
    uint8_t *dst;
    int src_size;
    int dst_size;
    int nbytes;
    atempo->state = YAE_FLUSH_OUTPUT;
    if (atempo->position[0] == frag->position[0] + frag->nsamples &&
        atempo->position[1] == frag->position[1] + frag->nsamples) {
        
        return 0;
    }
    if (frag->position[0] + frag->nsamples < atempo->position[0]) {
        
        yae_load_frag(atempo, NULL, NULL);
        if (atempo->nfrag) {
            
            yae_downmix(atempo, frag);
            
            av_rdft_calc(atempo->real_to_complex, frag->xdat);
            
            if (yae_adjust_position(atempo)) {
                
                yae_load_frag(atempo, NULL, NULL);
            }
        }
    }
    
    overlap_end = frag->position[1] + FFMIN(atempo->window / 2,
                                            frag->nsamples);
    while (atempo->position[1] < overlap_end) {
        if (yae_overlap_add(atempo, dst_ref, dst_end) != 0) {
            return AVERROR(EAGAIN);
        }
    }
    
    if (frag->position[0] + frag->nsamples < atempo->position[0]) {
        yae_advance_to_next_frag(atempo);
        return AVERROR(EAGAIN);
    }
    
    start_here = FFMAX(atempo->position[1], overlap_end);
    stop_here  = frag->position[1] + frag->nsamples;
    offset     = start_here - frag->position[1];
    av_assert0(start_here <= stop_here && frag->position[1] <= start_here);
    src = frag->data + offset * atempo->stride;
    dst = (uint8_t *)*dst_ref;
    src_size = (int)(stop_here - start_here) * atempo->stride;
    dst_size = dst_end - dst;
    nbytes = FFMIN(src_size, dst_size);
    memcpy(dst, src, nbytes);
    dst += nbytes;
    atempo->position[1] += (nbytes / atempo->stride);
    
    *dst_ref = (uint8_t *)dst;
    return atempo->position[1] == stop_here ? 0 : AVERROR(EAGAIN);
}
