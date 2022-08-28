static void
yae_apply(ATempoContext *atempo,
          const uint8_t **src_ref,
          const uint8_t *src_end,
          uint8_t **dst_ref,
          uint8_t *dst_end)
{
    while (1) {
        if (atempo->state == YAE_LOAD_FRAGMENT) {
            
            if (yae_load_frag(atempo, src_ref, src_end) != 0) {
                break;
            }
            
            yae_downmix(atempo, yae_curr_frag(atempo));
            
            av_rdft_calc(atempo->real_to_complex, yae_curr_frag(atempo)->xdat);
            
            if (!atempo->nfrag) {
                yae_advance_to_next_frag(atempo);
                continue;
            }
            atempo->state = YAE_ADJUST_POSITION;
        }
        if (atempo->state == YAE_ADJUST_POSITION) {
            
            if (yae_adjust_position(atempo)) {
                
                
                atempo->state = YAE_RELOAD_FRAGMENT;
            } else {
                atempo->state = YAE_OUTPUT_OVERLAP_ADD;
            }
        }
        if (atempo->state == YAE_RELOAD_FRAGMENT) {
            
            if (yae_load_frag(atempo, src_ref, src_end) != 0) {
                break;
            }
            
            yae_downmix(atempo, yae_curr_frag(atempo));
            
            av_rdft_calc(atempo->real_to_complex, yae_curr_frag(atempo)->xdat);
            atempo->state = YAE_OUTPUT_OVERLAP_ADD;
        }
        if (atempo->state == YAE_OUTPUT_OVERLAP_ADD) {
            
            if (yae_overlap_add(atempo, dst_ref, dst_end) != 0) {
                break;
            }
            
            yae_advance_to_next_frag(atempo);
            atempo->state = YAE_LOAD_FRAGMENT;
        }
    }
}
static void
yae_apply(ATempoContext *atempo,
          const uint8_t **src_ref,
          const uint8_t *src_end,
          uint8_t **dst_ref,
          uint8_t *dst_end)
{
    while (1) {
        if (atempo->state == YAE_LOAD_FRAGMENT) {
            
            if (yae_load_frag(atempo, src_ref, src_end) != 0) {
                break;
            }
            
            yae_downmix(atempo, yae_curr_frag(atempo));
            
            av_rdft_calc(atempo->real_to_complex, yae_curr_frag(atempo)->xdat);
            
            if (!atempo->nfrag) {
                yae_advance_to_next_frag(atempo);
                continue;
            }
            atempo->state = YAE_ADJUST_POSITION;
        }
