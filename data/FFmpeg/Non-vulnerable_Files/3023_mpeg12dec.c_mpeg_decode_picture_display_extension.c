static void mpeg_decode_picture_display_extension(Mpeg1Context *s1)
{
    MpegEncContext *s = &s1->mpeg_enc_ctx;
    int i, nofco;
    nofco = 1;
    if (s->progressive_sequence) {
        if (s->repeat_first_field) {
            nofco++;
            if (s->top_field_first)
                nofco++;
        }
    } else {
        if (s->picture_structure == PICT_FRAME) {
            nofco++;
            if (s->repeat_first_field)
                nofco++;
        }
    }
    for (i = 0; i < nofco; i++) {
        s1->pan_scan.position[i][0] = get_sbits(&s->gb, 16);
        skip_bits(&s->gb, 1); 
        s1->pan_scan.position[i][1] = get_sbits(&s->gb, 16);
        skip_bits(&s->gb, 1); 
    }
    if (s->avctx->debug & FF_DEBUG_PICT_INFO)
        av_log(s->avctx, AV_LOG_DEBUG,
               "pde (%"PRId16",%"PRId16") (%"PRId16",%"PRId16") (%"PRId16",%"PRId16")\n",
               s1->pan_scan.position[0][0], s1->pan_scan.position[0][1],
               s1->pan_scan.position[1][0], s1->pan_scan.position[1][1],
               s1->pan_scan.position[2][0], s1->pan_scan.position[2][1]);
}
