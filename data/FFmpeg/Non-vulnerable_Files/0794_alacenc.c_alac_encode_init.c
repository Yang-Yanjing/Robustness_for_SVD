static av_cold int alac_encode_init(AVCodecContext *avctx)
{
    AlacEncodeContext *s = avctx->priv_data;
    int ret;
    uint8_t *alac_extradata;
    avctx->frame_size = s->frame_size = DEFAULT_FRAME_SIZE;
    if (avctx->sample_fmt == AV_SAMPLE_FMT_S32P) {
        if (avctx->bits_per_raw_sample != 24)
            av_log(avctx, AV_LOG_WARNING, "encoding as 24 bits-per-sample\n");
        avctx->bits_per_raw_sample = 24;
    } else {
        avctx->bits_per_raw_sample = 16;
        s->extra_bits              = 0;
    }
    
    if (avctx->compression_level == FF_COMPRESSION_DEFAULT)
        s->compression_level = 2;
    else
        s->compression_level = av_clip(avctx->compression_level, 0, 2);
    
    s->rc.history_mult    = 40;
    s->rc.initial_history = 10;
    s->rc.k_modifier      = 14;
    s->rc.rice_modifier   = 4;
    s->max_coded_frame_size = get_max_frame_size(avctx->frame_size,
                                                 avctx->channels,
                                                 avctx->bits_per_raw_sample);
    avctx->extradata = av_mallocz(ALAC_EXTRADATA_SIZE + AV_INPUT_BUFFER_PADDING_SIZE);
    if (!avctx->extradata) {
        ret = AVERROR(ENOMEM);
        goto error;
    }
    avctx->extradata_size = ALAC_EXTRADATA_SIZE;
    alac_extradata = avctx->extradata;
    AV_WB32(alac_extradata,    ALAC_EXTRADATA_SIZE);
    AV_WB32(alac_extradata+4,  MKBETAG('a','l','a','c'));
    AV_WB32(alac_extradata+12, avctx->frame_size);
    AV_WB8 (alac_extradata+17, avctx->bits_per_raw_sample);
    AV_WB8 (alac_extradata+21, avctx->channels);
    AV_WB32(alac_extradata+24, s->max_coded_frame_size);
    AV_WB32(alac_extradata+28,
            avctx->sample_rate * avctx->channels * avctx->bits_per_raw_sample); 
    AV_WB32(alac_extradata+32, avctx->sample_rate);
    
    if (s->compression_level > 0) {
        AV_WB8(alac_extradata+18, s->rc.history_mult);
        AV_WB8(alac_extradata+19, s->rc.initial_history);
        AV_WB8(alac_extradata+20, s->rc.k_modifier);
    }
    s->min_prediction_order = DEFAULT_MIN_PRED_ORDER;
    if (avctx->min_prediction_order >= 0) {
        if (avctx->min_prediction_order < MIN_LPC_ORDER ||
           avctx->min_prediction_order > ALAC_MAX_LPC_ORDER) {
            av_log(avctx, AV_LOG_ERROR, "invalid min prediction order: %d\n",
                   avctx->min_prediction_order);
            ret = AVERROR(EINVAL);
            goto error;
        }
        s->min_prediction_order = avctx->min_prediction_order;
    }
    s->max_prediction_order = DEFAULT_MAX_PRED_ORDER;
    if (avctx->max_prediction_order >= 0) {
        if (avctx->max_prediction_order < MIN_LPC_ORDER ||
            avctx->max_prediction_order > ALAC_MAX_LPC_ORDER) {
            av_log(avctx, AV_LOG_ERROR, "invalid max prediction order: %d\n",
                   avctx->max_prediction_order);
            ret = AVERROR(EINVAL);
            goto error;
        }
        s->max_prediction_order = avctx->max_prediction_order;
    }
    if (s->max_prediction_order < s->min_prediction_order) {
        av_log(avctx, AV_LOG_ERROR,
               "invalid prediction orders: min=%d max=%d\n",
               s->min_prediction_order, s->max_prediction_order);
        ret = AVERROR(EINVAL);
        goto error;
    }
    s->avctx = avctx;
    if ((ret = ff_lpc_init(&s->lpc_ctx, avctx->frame_size,
                           s->max_prediction_order,
                           FF_LPC_TYPE_LEVINSON)) < 0) {
        goto error;
    }
    return 0;
error:
    alac_encode_close(avctx);
    return ret;
}
