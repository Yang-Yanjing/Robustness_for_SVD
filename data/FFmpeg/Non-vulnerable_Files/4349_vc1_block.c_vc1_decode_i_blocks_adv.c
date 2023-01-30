static void vc1_decode_i_blocks_adv(VC1Context *v)
{
    int k;
    MpegEncContext *s = &v->s;
    int cbp, val;
    uint8_t *coded_val;
    int mb_pos;
    int mquant = v->pq;
    int mqdiff;
    GetBitContext *gb = &s->gb;
    
    switch (v->y_ac_table_index) {
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
    
    s->mb_x             = s->mb_y = 0;
    s->mb_intra         = 1;
    s->first_slice_line = 1;
    s->mb_y             = s->start_mb_y;
    if (s->start_mb_y) {
        s->mb_x = 0;
        init_block_index(v);
        memset(&s->coded_block[s->block_index[0] - s->b8_stride], 0,
               (1 + s->b8_stride) * sizeof(*s->coded_block));
    }
    for (; s->mb_y < s->end_mb_y; s->mb_y++) {
        s->mb_x = 0;
        init_block_index(v);
        for (;s->mb_x < s->mb_width; s->mb_x++) {
            int16_t (*block)[64] = v->block[v->cur_blk_idx];
            ff_update_block_index(s);
            s->bdsp.clear_blocks(block[0]);
            mb_pos = s->mb_x + s->mb_y * s->mb_stride;
            s->current_picture.mb_type[mb_pos + v->mb_off]                         = MB_TYPE_INTRA;
            s->current_picture.motion_val[1][s->block_index[0] + v->blocks_off][0] = 0;
            s->current_picture.motion_val[1][s->block_index[0] + v->blocks_off][1] = 0;
            
            if (v->fieldtx_is_raw)
                v->fieldtx_plane[mb_pos] = get_bits1(&v->s.gb);
            cbp = get_vlc2(&v->s.gb, ff_msmp4_mb_i_vlc.table, MB_INTRA_VLC_BITS, 2);
            if (v->acpred_is_raw)
                v->s.ac_pred = get_bits1(&v->s.gb);
            else
                v->s.ac_pred = v->acpred_plane[mb_pos];
            if (v->condover == CONDOVER_SELECT && v->overflg_is_raw)
                v->over_flags_plane[mb_pos] = get_bits1(&v->s.gb);
            GET_MQUANT();
            s->current_picture.qscale_table[mb_pos] = mquant;
            
            s->y_dc_scale = s->y_dc_scale_table[mquant];
            s->c_dc_scale = s->c_dc_scale_table[mquant];
            for (k = 0; k < 6; k++) {
                val = ((cbp >> (5 - k)) & 1);
                if (k < 4) {
                    int pred   = vc1_coded_block_pred(&v->s, k, &coded_val);
                    val        = val ^ pred;
                    *coded_val = val;
                }
                cbp |= val << (5 - k);
                v->a_avail = !s->first_slice_line || (k == 2 || k == 3);
                v->c_avail = !!s->mb_x || (k == 1 || k == 3);
                vc1_decode_i_block_adv(v, block[k], k, val,
                                       (k < 4) ? v->codingset : v->codingset2, mquant);
                if (CONFIG_GRAY && k > 3 && (s->avctx->flags & AV_CODEC_FLAG_GRAY))
                    continue;
                v->vc1dsp.vc1_inv_trans_8x8(block[k]);
            }
            ff_vc1_smooth_overlap_filter_iblk(v);
            vc1_put_signed_blocks_clamped(v);
            if (v->s.loop_filter)
                ff_vc1_loop_filter_iblk_delayed(v, v->pq);
            if (get_bits_count(&s->gb) > v->bits) {
                
                ff_er_add_slice(&s->er, 0, s->start_mb_y, s->mb_x, s->mb_y, ER_MB_ERROR);
                av_log(s->avctx, AV_LOG_ERROR, "Bits overconsumption: %i > %i\n",
                       get_bits_count(&s->gb), v->bits);
                return;
            }
        }
        if (!v->s.loop_filter)
            ff_mpeg_draw_horiz_band(s, s->mb_y * 16, 16);
        else if (s->mb_y)
            ff_mpeg_draw_horiz_band(s, (s->mb_y-1) * 16, 16);
        s->first_slice_line = 0;
    }
    
    s->mb_x = 0;
    init_block_index(v);
    for (; s->mb_x < s->mb_width; s->mb_x++) {
        ff_update_block_index(s);
        vc1_put_signed_blocks_clamped(v);
        if (v->s.loop_filter)
            ff_vc1_loop_filter_iblk_delayed(v, v->pq);
    }
    if (v->s.loop_filter)
        ff_mpeg_draw_horiz_band(s, (s->end_mb_y - 1) * 16, 16);
    ff_er_add_slice(&s->er, 0, s->start_mb_y << v->field_mode, s->mb_width - 1,
                    (s->end_mb_y << v->field_mode) - 1, ER_MB_END);
}
