enum AVChromaLocation ff_choose_chroma_location(AVFormatContext *s, AVStream *st)
{
    AVCodecContext *avctx = st->codec;
    const AVPixFmtDescriptor *pix_desc = av_pix_fmt_desc_get(avctx->pix_fmt);
    if (avctx->chroma_sample_location != AVCHROMA_LOC_UNSPECIFIED)
        return avctx->chroma_sample_location;
    if (pix_desc) {
        if (pix_desc->log2_chroma_h == 0) {
            return AVCHROMA_LOC_TOPLEFT;
        } else if (pix_desc->log2_chroma_w == 1 && pix_desc->log2_chroma_h == 1) {
            if (avctx->field_order == AV_FIELD_UNKNOWN || avctx->field_order == AV_FIELD_PROGRESSIVE) {
                switch (avctx->codec_id) {
                case AV_CODEC_ID_MJPEG:
                case AV_CODEC_ID_MPEG1VIDEO: return AVCHROMA_LOC_CENTER;
                }
            }
            if (avctx->field_order == AV_FIELD_UNKNOWN || avctx->field_order != AV_FIELD_PROGRESSIVE) {
                switch (avctx->codec_id) {
                case AV_CODEC_ID_MPEG2VIDEO: return AVCHROMA_LOC_LEFT;
                }
            }
        }
    }
    return AVCHROMA_LOC_UNSPECIFIED;
}
