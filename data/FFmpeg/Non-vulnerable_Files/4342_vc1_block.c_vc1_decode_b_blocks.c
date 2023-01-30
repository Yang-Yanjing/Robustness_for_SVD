static void vc1_decode_b_blocks(VC1Context *v)
{
    MpegEncContext *s = &v->s;
    
    switch (v->c_ac_table_index) {
    case 0:
        v->codingset = (v->pqindex <= 8) ? CS_HIGH_RATE_INTRA : CS_LOW_MOT_INTRA;
        break;
    case 1:
        v->codingset = CS_HIGH_MOT_INTRA;
        break;
    case 2:
        v->codingset = CS_MID_RATE_INTRA;
        break;
    }
    switch (v->c_ac_table_index) {
    case 0:
        v->codingset2 = (v->pqindex <= 8) ? CS_HIGH_RATE_INTER : CS_LOW_MOT_INTER;
        break;
    case 1:
        v->codingset2 = CS_HIGH_MOT_INTER;
        break;
    case 2:
        v->codingset2 = CS_MID_RATE_INTER;
        break;
    }
    s->first_slice_line = 1;
    for (s->mb_y = s->start_mb_y; s->mb_y < s->end_mb_y; s->mb_y++) {
        s->mb_x = 0;
        init_block_index(v);
        for (; s->mb_x < s->mb_width; s->mb_x++) {
            ff_update_block_index(s);
            if (v->fcm == ILACE_FIELD)
                vc1_decode_b_mb_intfi(v);
            else if (v->fcm == ILACE_FRAME)
                vc1_decode_b_mb_intfr(v);
            else
                vc1_decode_b_mb(v);
            if (get_bits_count(&s->gb) > v->bits || get_bits_count(&s->gb) < 0) {
                
                ff_er_add_slice(&s->er, 0, s->start_mb_y, s->mb_x, s->mb_y, ER_MB_ERROR);
                av_log(s->avctx, AV_LOG_ERROR, "Bits overconsumption: %i > %i at %ix%i\n",
                       get_bits_count(&s->gb), v->bits, s->mb_x, s->mb_y);
                return;
            }
            if (v->s.loop_filter)
                ff_vc1_loop_filter_iblk(v, v->pq);
        }
        if (!v->s.loop_filter)
            ff_mpeg_draw_horiz_band(s, s->mb_y * 16, 16);
        else if (s->mb_y)
            ff_mpeg_draw_horiz_band(s, (s->mb_y - 1) * 16, 16);
        s->first_slice_line = 0;
    }
    if (v->s.loop_filter)
        ff_mpeg_draw_horiz_band(s, (s->end_mb_y - 1) * 16, 16);
    ff_er_add_slice(&s->er, 0, s->start_mb_y << v->field_mode, s->mb_width - 1,
                    (s->end_mb_y << v->field_mode) - 1, ER_MB_END);
}
