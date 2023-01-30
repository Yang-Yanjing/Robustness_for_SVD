static av_cold int adx_decode_init(AVCodecContext *avctx)
{
    ADXContext *c = avctx->priv_data;
    int ret, header_size;
    if (avctx->extradata_size >= 24) {
        if ((ret = ff_adx_decode_header(avctx, avctx->extradata,
                                        avctx->extradata_size, &header_size,
                                        c->coeff)) < 0) {
            av_log(avctx, AV_LOG_ERROR, "error parsing ADX header\n");
            return AVERROR_INVALIDDATA;
        }
        c->channels      = avctx->channels;
        c->header_parsed = 1;
    }
    avctx->sample_fmt = AV_SAMPLE_FMT_S16P;
    return 0;
}
