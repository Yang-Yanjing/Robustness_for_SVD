static void mpeg_decode_sequence_display_extension(Mpeg1Context *s1)
{
    MpegEncContext *s = &s1->mpeg_enc_ctx;
    int color_description, w, h;
    skip_bits(&s->gb, 3); 
    color_description = get_bits1(&s->gb);
    if (color_description) {
        s->avctx->color_primaries = get_bits(&s->gb, 8);
        s->avctx->color_trc       = get_bits(&s->gb, 8);
        s->avctx->colorspace      = get_bits(&s->gb, 8);
    }
    w = get_bits(&s->gb, 14);
    skip_bits(&s->gb, 1); 
    h = get_bits(&s->gb, 14);
    
    s1->pan_scan.width  = 16 * w;
    s1->pan_scan.height = 16 * h;
    if (s->avctx->debug & FF_DEBUG_PICT_INFO)
        av_log(s->avctx, AV_LOG_DEBUG, "sde w:%d, h:%d\n", w, h);
}
