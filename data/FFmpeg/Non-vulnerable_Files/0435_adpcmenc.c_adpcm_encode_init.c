static av_cold int adpcm_encode_init(AVCodecContext *avctx)
{
    ADPCMEncodeContext *s = avctx->priv_data;
    uint8_t *extradata;
    int i;
    int ret = AVERROR(ENOMEM);
    if (avctx->channels > 2) {
        av_log(avctx, AV_LOG_ERROR, "only stereo or mono is supported\n");
        return AVERROR(EINVAL);
    }
    if (avctx->trellis && (unsigned)avctx->trellis > 16U) {
        av_log(avctx, AV_LOG_ERROR, "invalid trellis size\n");
        return AVERROR(EINVAL);
    }
    if (avctx->trellis) {
        int frontier  = 1 << avctx->trellis;
        int max_paths =  frontier * FREEZE_INTERVAL;
        FF_ALLOC_OR_GOTO(avctx, s->paths,
                         max_paths * sizeof(*s->paths), error);
        FF_ALLOC_OR_GOTO(avctx, s->node_buf,
                         2 * frontier * sizeof(*s->node_buf),  error);
        FF_ALLOC_OR_GOTO(avctx, s->nodep_buf,
                         2 * frontier * sizeof(*s->nodep_buf), error);
        FF_ALLOC_OR_GOTO(avctx, s->trellis_hash,
                         65536 * sizeof(*s->trellis_hash), error);
    }
    avctx->bits_per_coded_sample = av_get_bits_per_sample(avctx->codec->id);
    switch (avctx->codec->id) {
    case AV_CODEC_ID_ADPCM_IMA_WAV:
        

        avctx->frame_size = (BLKSIZE - 4 * avctx->channels) * 8 /
                            (4 * avctx->channels) + 1;
        

        avctx->block_align = BLKSIZE;
        avctx->bits_per_coded_sample = 4;
        break;
    case AV_CODEC_ID_ADPCM_IMA_QT:
        avctx->frame_size  = 64;
        avctx->block_align = 34 * avctx->channels;
        break;
    case AV_CODEC_ID_ADPCM_MS:
        

        avctx->frame_size = (BLKSIZE - 7 * avctx->channels) * 2 / avctx->channels + 2;
        avctx->bits_per_coded_sample = 4;
        avctx->block_align    = BLKSIZE;
        if (!(avctx->extradata = av_malloc(32 + AV_INPUT_BUFFER_PADDING_SIZE)))
            goto error;
        avctx->extradata_size = 32;
        extradata = avctx->extradata;
        bytestream_put_le16(&extradata, avctx->frame_size);
        bytestream_put_le16(&extradata, 7); 
        for (i = 0; i < 7; i++) {
            bytestream_put_le16(&extradata, ff_adpcm_AdaptCoeff1[i] * 4);
            bytestream_put_le16(&extradata, ff_adpcm_AdaptCoeff2[i] * 4);
        }
        break;
    case AV_CODEC_ID_ADPCM_YAMAHA:
        avctx->frame_size  = BLKSIZE * 2 / avctx->channels;
        avctx->block_align = BLKSIZE;
        break;
    case AV_CODEC_ID_ADPCM_SWF:
        if (avctx->sample_rate != 11025 &&
            avctx->sample_rate != 22050 &&
            avctx->sample_rate != 44100) {
            av_log(avctx, AV_LOG_ERROR, "Sample rate must be 11025, "
                   "22050 or 44100\n");
            ret = AVERROR(EINVAL);
            goto error;
        }
        avctx->frame_size = 512 * (avctx->sample_rate / 11025);
        break;
    default:
        ret = AVERROR(EINVAL);
        goto error;
    }
    return 0;
error:
    adpcm_encode_close(avctx);
    return ret;
}
