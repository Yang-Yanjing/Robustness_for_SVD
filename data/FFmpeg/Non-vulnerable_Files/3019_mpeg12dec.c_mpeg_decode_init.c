static av_cold int mpeg_decode_init(AVCodecContext *avctx)
{
    Mpeg1Context *s    = avctx->priv_data;
    MpegEncContext *s2 = &s->mpeg_enc_ctx;
    ff_mpv_decode_defaults(s2);
    if (   avctx->codec_tag != AV_RL32("VCR2")
        && avctx->codec_tag != AV_RL32("BW10"))
        avctx->coded_width = avctx->coded_height = 0; 
    ff_mpv_decode_init(s2, avctx);
    s->mpeg_enc_ctx.avctx  = avctx;
    

    ff_mpv_idct_init(s2);
    ff_mpeg12_common_init(&s->mpeg_enc_ctx);
    ff_mpeg12_init_vlcs();
    s->mpeg_enc_ctx_allocated      = 0;
    s->mpeg_enc_ctx.picture_number = 0;
    s->repeat_field                = 0;
    s->mpeg_enc_ctx.codec_id       = avctx->codec->id;
    avctx->color_range             = AVCOL_RANGE_MPEG;
    return 0;
}
