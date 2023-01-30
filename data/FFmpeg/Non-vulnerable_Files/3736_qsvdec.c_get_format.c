static int get_format(AVCodecContext *avctx, const enum AVPixelFormat *pix_fmts)
{
    while (*pix_fmts != AV_PIX_FMT_NONE) {
        if (*pix_fmts == AV_PIX_FMT_QSV) {
            if (!avctx->hwaccel_context) {
                DecodeContext *decode = avctx->opaque;
                AVQSVContext *qsv = av_qsv_alloc_context();
                if (!qsv)
                    return AV_PIX_FMT_NONE;
                qsv->session   = decode->mfx_session;
                qsv->iopattern = MFX_IOPATTERN_OUT_VIDEO_MEMORY;
                avctx->hwaccel_context = qsv;
            }
            return AV_PIX_FMT_QSV;
        }
        pix_fmts++;
    }
    fprintf(stderr, "The QSV pixel format not offered in get_format()\n");
    return AV_PIX_FMT_NONE;
}
