}
static int Open(vlc_va_t *va, AVCodecContext *ctx, const es_format_t *fmt)
{
    union
    {
        char str[4];
        vlc_fourcc_t fourcc;
    } u = { .fourcc = fmt->i_codec };
    AVVDPAUContext *hwctx = av_vdpau_alloc_context();
    if (unlikely(hwctx == NULL))
       return VLC_ENOMEM;
    msg_Dbg(va, "codec %d (%4.4s) profile %d level %d", ctx->codec_id, u.str,
            fmt->i_profile, fmt->i_level);
    hwctx->decoder = DECODER_MAGIC;
    hwctx->render = Render;
    va->sys = (vlc_va_sys_t *)hwctx;
    va->description = "Dummy video decoding accelerator";
    va->pix_fmt = AV_PIX_FMT_VDPAU;
    va->setup = Setup;
    va->get = Lock;
    va->release = Unlock;
    va->extract = Copy;
    return VLC_SUCCESS;
}
static void Close(vlc_va_t *va)
{
    av_free(va->sys);
}
