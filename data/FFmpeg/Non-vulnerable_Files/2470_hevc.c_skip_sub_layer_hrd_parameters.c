static void skip_sub_layer_hrd_parameters(GetBitContext *gb,
                                          unsigned int cpb_cnt_minus1,
                                          uint8_t sub_pic_hrd_params_present_flag)
{
    unsigned int i;
    for (i = 0; i <= cpb_cnt_minus1; i++) {
        get_ue_golomb_long(gb); 
        get_ue_golomb_long(gb); 
        if (sub_pic_hrd_params_present_flag) {
            get_ue_golomb_long(gb); 
            get_ue_golomb_long(gb); 
        }
        skip_bits1(gb); 
    }
}
