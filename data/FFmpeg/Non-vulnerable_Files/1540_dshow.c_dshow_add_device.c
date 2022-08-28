static int
dshow_add_device(AVFormatContext *avctx,
                 enum dshowDeviceType devtype)
{
    struct dshow_ctx *ctx = avctx->priv_data;
    AM_MEDIA_TYPE type;
    AVCodecContext *codec;
    AVStream *st;
    int ret = AVERROR(EIO);
    st = avformat_new_stream(avctx, NULL);
    if (!st) {
        ret = AVERROR(ENOMEM);
        goto error;
    }
    st->id = devtype;
    ctx->capture_filter[devtype]->stream_index = st->index;
    libAVPin_ConnectionMediaType(ctx->capture_pin[devtype], &type);
    codec = st->codec;
    if (devtype == VideoDevice) {
        BITMAPINFOHEADER *bih = NULL;
        AVRational time_base;
        if (IsEqualGUID(&type.formattype, &FORMAT_VideoInfo)) {
            VIDEOINFOHEADER *v = (void *) type.pbFormat;
            time_base = (AVRational) { v->AvgTimePerFrame, 10000000 };
            bih = &v->bmiHeader;
        } else if (IsEqualGUID(&type.formattype, &FORMAT_VideoInfo2)) {
            VIDEOINFOHEADER2 *v = (void *) type.pbFormat;
            time_base = (AVRational) { v->AvgTimePerFrame, 10000000 };
            bih = &v->bmiHeader;
        }
        if (!bih) {
            av_log(avctx, AV_LOG_ERROR, "Could not get media type.\n");
            goto error;
        }
        codec->time_base  = time_base;
        codec->codec_type = AVMEDIA_TYPE_VIDEO;
        codec->width      = bih->biWidth;
        codec->height     = bih->biHeight;
        codec->codec_tag  = bih->biCompression;
        codec->pix_fmt    = dshow_pixfmt(bih->biCompression, bih->biBitCount);
        if (bih->biCompression == MKTAG('H', 'D', 'Y', 'C')) {
            av_log(avctx, AV_LOG_DEBUG, "attempt to use full range for HDYC...\n");
            codec->color_range = AVCOL_RANGE_MPEG; 
        }
        if (codec->pix_fmt == AV_PIX_FMT_NONE) {
            const AVCodecTag *const tags[] = { avformat_get_riff_video_tags(), NULL };
            codec->codec_id = av_codec_get_id(tags, bih->biCompression);
            if (codec->codec_id == AV_CODEC_ID_NONE) {
                av_log(avctx, AV_LOG_ERROR, "Unknown compression type. "
                                 "Please report type 0x%X.\n", (int) bih->biCompression);
                return AVERROR_PATCHWELCOME;
            }
            codec->bits_per_coded_sample = bih->biBitCount;
        } else {
            codec->codec_id = AV_CODEC_ID_RAWVIDEO;
            if (bih->biCompression == BI_RGB || bih->biCompression == BI_BITFIELDS) {
                codec->bits_per_coded_sample = bih->biBitCount;
                codec->extradata = av_malloc(9 + AV_INPUT_BUFFER_PADDING_SIZE);
                if (codec->extradata) {
                    codec->extradata_size = 9;
                    memcpy(codec->extradata, "BottomUp", 9);
                }
            }
        }
    } else {
        WAVEFORMATEX *fx = NULL;
        if (IsEqualGUID(&type.formattype, &FORMAT_WaveFormatEx)) {
            fx = (void *) type.pbFormat;
        }
        if (!fx) {
            av_log(avctx, AV_LOG_ERROR, "Could not get media type.\n");
            goto error;
        }
        codec->codec_type  = AVMEDIA_TYPE_AUDIO;
        codec->sample_fmt  = sample_fmt_bits_per_sample(fx->wBitsPerSample);
        codec->codec_id    = waveform_codec_id(codec->sample_fmt);
        codec->sample_rate = fx->nSamplesPerSec;
        codec->channels    = fx->nChannels;
    }
    avpriv_set_pts_info(st, 64, 1, 10000000);
    ret = 0;
error:
    return ret;
}
