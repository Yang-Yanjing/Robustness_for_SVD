static inline int uses_vdpau(AVCodecContext *avctx) {
    return avctx->pix_fmt == AV_PIX_FMT_VDPAU_MPEG1 || avctx->pix_fmt == AV_PIX_FMT_VDPAU_MPEG2;
}
