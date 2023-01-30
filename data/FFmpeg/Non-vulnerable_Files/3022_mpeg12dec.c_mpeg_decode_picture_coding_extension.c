static void mpeg_decode_picture_coding_extension(Mpeg1Context *s1)
{
    MpegEncContext *s = &s1->mpeg_enc_ctx;
    s->full_pel[0]       = s->full_pel[1] = 0;
    s->mpeg_f_code[0][0] = get_bits(&s->gb, 4);
    s->mpeg_f_code[0][1] = get_bits(&s->gb, 4);
    s->mpeg_f_code[1][0] = get_bits(&s->gb, 4);
    s->mpeg_f_code[1][1] = get_bits(&s->gb, 4);
    if (!s->pict_type && s1->mpeg_enc_ctx_allocated) {
        av_log(s->avctx, AV_LOG_ERROR,
               "Missing picture start code, guessing missing values\n");
        if (s->mpeg_f_code[1][0] == 15 && s->mpeg_f_code[1][1] == 15) {
            if (s->mpeg_f_code[0][0] == 15 && s->mpeg_f_code[0][1] == 15)
                s->pict_type = AV_PICTURE_TYPE_I;
            else
                s->pict_type = AV_PICTURE_TYPE_P;
        } else
            s->pict_type = AV_PICTURE_TYPE_B;
        s->current_picture.f->pict_type = s->pict_type;
        s->current_picture.f->key_frame = s->pict_type == AV_PICTURE_TYPE_I;
    }
    s->mpeg_f_code[0][0] += !s->mpeg_f_code[0][0];
    s->mpeg_f_code[0][1] += !s->mpeg_f_code[0][1];
    s->mpeg_f_code[1][0] += !s->mpeg_f_code[1][0];
    s->mpeg_f_code[1][1] += !s->mpeg_f_code[1][1];
    s->intra_dc_precision         = get_bits(&s->gb, 2);
    s->picture_structure          = get_bits(&s->gb, 2);
    s->top_field_first            = get_bits1(&s->gb);
    s->frame_pred_frame_dct       = get_bits1(&s->gb);
    s->concealment_motion_vectors = get_bits1(&s->gb);
    s->q_scale_type               = get_bits1(&s->gb);
    s->intra_vlc_format           = get_bits1(&s->gb);
    s->alternate_scan             = get_bits1(&s->gb);
    s->repeat_first_field         = get_bits1(&s->gb);
    s->chroma_420_type            = get_bits1(&s->gb);
    s->progressive_frame          = get_bits1(&s->gb);
    if (s->alternate_scan) {
        ff_init_scantable(s->idsp.idct_permutation, &s->inter_scantable, ff_alternate_vertical_scan);
        ff_init_scantable(s->idsp.idct_permutation, &s->intra_scantable, ff_alternate_vertical_scan);
    } else {
        ff_init_scantable(s->idsp.idct_permutation, &s->inter_scantable, ff_zigzag_direct);
        ff_init_scantable(s->idsp.idct_permutation, &s->intra_scantable, ff_zigzag_direct);
    }
    
    ff_dlog(s->avctx, "intra_dc_precision=%d\n", s->intra_dc_precision);
    ff_dlog(s->avctx, "picture_structure=%d\n", s->picture_structure);
    ff_dlog(s->avctx, "top field first=%d\n", s->top_field_first);
    ff_dlog(s->avctx, "repeat first field=%d\n", s->repeat_first_field);
    ff_dlog(s->avctx, "conceal=%d\n", s->concealment_motion_vectors);
    ff_dlog(s->avctx, "intra_vlc_format=%d\n", s->intra_vlc_format);
    ff_dlog(s->avctx, "alternate_scan=%d\n", s->alternate_scan);
    ff_dlog(s->avctx, "frame_pred_frame_dct=%d\n", s->frame_pred_frame_dct);
    ff_dlog(s->avctx, "progressive_frame=%d\n", s->progressive_frame);
}
