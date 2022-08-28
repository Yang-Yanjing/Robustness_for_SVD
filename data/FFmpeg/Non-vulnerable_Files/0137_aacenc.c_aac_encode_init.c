static av_cold int aac_encode_init(AVCodecContext *avctx)
{
    AACEncContext *s = avctx->priv_data;
    int i, ret = 0;
    const uint8_t *sizes[2];
    uint8_t grouping[AAC_MAX_CHANNELS];
    int lengths[2];
    avctx->frame_size = 1024;
    for (i = 0; i < 16; i++)
        if (avctx->sample_rate == avpriv_mpeg4audio_sample_rates[i])
            break;
    s->channels = avctx->channels;
    ERROR_IF(i == 16 || i >= ff_aac_swb_size_1024_len || i >= ff_aac_swb_size_128_len,
             "Unsupported sample rate %d\n", avctx->sample_rate);
    ERROR_IF(s->channels > AAC_MAX_CHANNELS,
             "Unsupported number of channels: %d\n", s->channels);
    WARN_IF(1024.0 * avctx->bit_rate / avctx->sample_rate > 6144 * s->channels,
             "Too many bits per frame requested, clamping to max\n");
    if (avctx->profile == FF_PROFILE_AAC_MAIN) {
        s->options.pred = 1;
    } else if ((avctx->profile == FF_PROFILE_AAC_LOW ||
                avctx->profile == FF_PROFILE_UNKNOWN) && s->options.pred) {
        s->profile = 0; 
        WARN_IF(1, "Prediction requested, changing profile to AAC-Main\n");
    } else if (avctx->profile == FF_PROFILE_AAC_LOW ||
               avctx->profile == FF_PROFILE_UNKNOWN) {
        s->profile = 1; 
    } else {
        ERROR_IF(1, "Unsupported profile %d\n", avctx->profile);
    }
    if (s->options.aac_coder != AAC_CODER_TWOLOOP) {
        s->options.intensity_stereo = 0;
        s->options.pns = 0;
    }
    avctx->bit_rate = (int)FFMIN(
        6144 * s->channels / 1024.0 * avctx->sample_rate,
        avctx->bit_rate);
    s->samplerate_index = i;
    s->chan_map = aac_chan_configs[s->channels-1];
    if ((ret = dsp_init(avctx, s)) < 0)
        goto fail;
    if ((ret = alloc_buffers(avctx, s)) < 0)
        goto fail;
    avctx->extradata_size = 5;
    put_audio_specific_config(avctx);
    sizes[0]   = ff_aac_swb_size_1024[i];
    sizes[1]   = ff_aac_swb_size_128[i];
    lengths[0] = ff_aac_num_swb_1024[i];
    lengths[1] = ff_aac_num_swb_128[i];
    for (i = 0; i < s->chan_map[0]; i++)
        grouping[i] = s->chan_map[i + 1] == TYPE_CPE;
    if ((ret = ff_psy_init(&s->psy, avctx, 2, sizes, lengths,
                           s->chan_map[0], grouping)) < 0)
        goto fail;
    s->psypp = ff_psy_preprocess_init(avctx);
    s->coder = &ff_aac_coders[s->options.aac_coder];
    ff_lpc_init(&s->lpc, 2*avctx->frame_size, TNS_MAX_ORDER, FF_LPC_TYPE_LEVINSON);
    if (HAVE_MIPSDSPR1)
        ff_aac_coder_init_mips(s);
    s->lambda = avctx->global_quality > 0 ? avctx->global_quality : 120;
    ff_aac_tableinit();
    avctx->initial_padding = 1024;
    ff_af_queue_init(avctx, &s->afq);
    return 0;
fail:
    aac_encode_end(avctx);
    return ret;
}
