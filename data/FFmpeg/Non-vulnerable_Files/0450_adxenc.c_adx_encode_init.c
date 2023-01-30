static av_cold int adx_encode_init(AVCodecContext *avctx)
{
    ADXContext *c = avctx->priv_data;
    if (avctx->channels > 2) {
        av_log(avctx, AV_LOG_ERROR, "Invalid number of channels\n");
        return AVERROR(EINVAL);
    }
    avctx->frame_size = BLOCK_SAMPLES;
    
    c->cutoff = 500;
    ff_adx_calculate_coeffs(c->cutoff, avctx->sample_rate, COEFF_BITS, c->coeff);
    return 0;
}
