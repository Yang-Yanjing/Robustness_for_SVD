static av_cold int allocate_buffers(AC3EncodeContext *s)
{
    AVCodecContext *avctx = s->avctx;
    int blk, ch;
    int channels = s->channels + 1; 
    int channel_blocks = channels * s->num_blocks;
    int total_coefs    = AC3_MAX_COEFS * channel_blocks;
    if (s->allocate_sample_buffers(s))
        goto alloc_fail;
    FF_ALLOC_ARRAY_OR_GOTO(avctx, s->bap_buffer, total_coefs,
                     sizeof(*s->bap_buffer), alloc_fail);
    FF_ALLOC_ARRAY_OR_GOTO(avctx, s->bap1_buffer, total_coefs,
                     sizeof(*s->bap1_buffer), alloc_fail);
    FF_ALLOCZ_ARRAY_OR_GOTO(avctx, s->mdct_coef_buffer, total_coefs,
                      sizeof(*s->mdct_coef_buffer), alloc_fail);
    FF_ALLOC_ARRAY_OR_GOTO(avctx, s->exp_buffer, total_coefs,
                     sizeof(*s->exp_buffer), alloc_fail);
    FF_ALLOC_ARRAY_OR_GOTO(avctx, s->grouped_exp_buffer, channel_blocks, 128 *
                     sizeof(*s->grouped_exp_buffer), alloc_fail);
    FF_ALLOC_ARRAY_OR_GOTO(avctx, s->psd_buffer, total_coefs,
                     sizeof(*s->psd_buffer), alloc_fail);
    FF_ALLOC_ARRAY_OR_GOTO(avctx, s->band_psd_buffer, channel_blocks, 64 *
                     sizeof(*s->band_psd_buffer), alloc_fail);
    FF_ALLOC_ARRAY_OR_GOTO(avctx, s->mask_buffer, channel_blocks, 64 *
                     sizeof(*s->mask_buffer), alloc_fail);
    FF_ALLOC_ARRAY_OR_GOTO(avctx, s->qmant_buffer, total_coefs,
                     sizeof(*s->qmant_buffer), alloc_fail);
    if (s->cpl_enabled) {
        FF_ALLOC_ARRAY_OR_GOTO(avctx, s->cpl_coord_exp_buffer, channel_blocks, 16 *
                         sizeof(*s->cpl_coord_exp_buffer), alloc_fail);
        FF_ALLOC_ARRAY_OR_GOTO(avctx, s->cpl_coord_mant_buffer, channel_blocks, 16 *
                         sizeof(*s->cpl_coord_mant_buffer), alloc_fail);
    }
    for (blk = 0; blk < s->num_blocks; blk++) {
        AC3Block *block = &s->blocks[blk];
        FF_ALLOCZ_ARRAY_OR_GOTO(avctx, block->mdct_coef, channels, sizeof(*block->mdct_coef),
                          alloc_fail);
        FF_ALLOCZ_ARRAY_OR_GOTO(avctx, block->exp, channels, sizeof(*block->exp),
                          alloc_fail);
        FF_ALLOCZ_ARRAY_OR_GOTO(avctx, block->grouped_exp, channels, sizeof(*block->grouped_exp),
                          alloc_fail);
        FF_ALLOCZ_ARRAY_OR_GOTO(avctx, block->psd, channels, sizeof(*block->psd),
                          alloc_fail);
        FF_ALLOCZ_ARRAY_OR_GOTO(avctx, block->band_psd, channels, sizeof(*block->band_psd),
                          alloc_fail);
        FF_ALLOCZ_ARRAY_OR_GOTO(avctx, block->mask, channels, sizeof(*block->mask),
                          alloc_fail);
        FF_ALLOCZ_ARRAY_OR_GOTO(avctx, block->qmant, channels, sizeof(*block->qmant),
                          alloc_fail);
        if (s->cpl_enabled) {
            FF_ALLOCZ_ARRAY_OR_GOTO(avctx, block->cpl_coord_exp, channels, sizeof(*block->cpl_coord_exp),
                              alloc_fail);
            FF_ALLOCZ_ARRAY_OR_GOTO(avctx, block->cpl_coord_mant, channels, sizeof(*block->cpl_coord_mant),
                              alloc_fail);
        }
        for (ch = 0; ch < channels; ch++) {
            
            block->grouped_exp[ch] = &s->grouped_exp_buffer[128           * (blk * channels + ch)];
            block->psd[ch]         = &s->psd_buffer        [AC3_MAX_COEFS * (blk * channels + ch)];
            block->band_psd[ch]    = &s->band_psd_buffer   [64            * (blk * channels + ch)];
            block->mask[ch]        = &s->mask_buffer       [64            * (blk * channels + ch)];
            block->qmant[ch]       = &s->qmant_buffer      [AC3_MAX_COEFS * (blk * channels + ch)];
            if (s->cpl_enabled) {
                block->cpl_coord_exp[ch]  = &s->cpl_coord_exp_buffer [16  * (blk * channels + ch)];
                block->cpl_coord_mant[ch] = &s->cpl_coord_mant_buffer[16  * (blk * channels + ch)];
            }
            
            block->exp[ch]         = &s->exp_buffer        [AC3_MAX_COEFS * (s->num_blocks * ch + blk)];
            block->mdct_coef[ch]   = &s->mdct_coef_buffer  [AC3_MAX_COEFS * (s->num_blocks * ch + blk)];
        }
    }
    if (!s->fixed_point) {
        FF_ALLOCZ_ARRAY_OR_GOTO(avctx, s->fixed_coef_buffer, total_coefs,
                          sizeof(*s->fixed_coef_buffer), alloc_fail);
        for (blk = 0; blk < s->num_blocks; blk++) {
            AC3Block *block = &s->blocks[blk];
            FF_ALLOCZ_ARRAY_OR_GOTO(avctx, block->fixed_coef, channels,
                              sizeof(*block->fixed_coef), alloc_fail);
            for (ch = 0; ch < channels; ch++)
                block->fixed_coef[ch] = &s->fixed_coef_buffer[AC3_MAX_COEFS * (s->num_blocks * ch + blk)];
        }
    } else {
        for (blk = 0; blk < s->num_blocks; blk++) {
            AC3Block *block = &s->blocks[blk];
            FF_ALLOCZ_ARRAY_OR_GOTO(avctx, block->fixed_coef, channels,
                              sizeof(*block->fixed_coef), alloc_fail);
            for (ch = 0; ch < channels; ch++)
                block->fixed_coef[ch] = (int32_t *)block->mdct_coef[ch];
        }
    }
    return 0;
alloc_fail:
    return AVERROR(ENOMEM);
}
