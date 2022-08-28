static av_always_inline void get_mvdata_interlaced(VC1Context *v, int *dmv_x,
                                                   int *dmv_y, int *pred_flag)
{
    int index, index1;
    int extend_x, extend_y;
    GetBitContext *gb = &v->s.gb;
    int bits, esc;
    int val, sign;
    if (v->numref) {
        bits = VC1_2REF_MVDATA_VLC_BITS;
        esc  = 125;
    } else {
        bits = VC1_1REF_MVDATA_VLC_BITS;
        esc  = 71;
    }
    extend_x = v->dmvrange & 1;
    extend_y = (v->dmvrange >> 1) & 1;
    index = get_vlc2(gb, v->imv_vlc->table, bits, 3);
    if (index == esc) {
        *dmv_x = get_bits(gb, v->k_x);
        *dmv_y = get_bits(gb, v->k_y);
        if (v->numref) {
            if (pred_flag)
                *pred_flag = *dmv_y & 1;
            *dmv_y = (*dmv_y + (*dmv_y & 1)) >> 1;
        }
    }
    else {
        av_assert0(index < esc);
        index1 = (index + 1) % 9;
        if (index1 != 0) {
            val    = get_bits(gb, index1 + extend_x);
            sign   = 0 - (val & 1);
            *dmv_x = (sign ^ ((val >> 1) + offset_table[extend_x][index1])) - sign;
        } else
            *dmv_x = 0;
        index1 = (index + 1) / 9;
        if (index1 > v->numref) {
            val    = get_bits(gb, (index1 >> v->numref) + extend_y);
            sign   = 0 - (val & 1);
            *dmv_y = (sign ^ ((val >> 1) + offset_table[extend_y][index1 >> v->numref])) - sign;
        } else
            *dmv_y = 0;
        if (v->numref && pred_flag)
            *pred_flag = index1 & 1;
    }
}
