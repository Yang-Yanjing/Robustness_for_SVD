}
static void ParseSlice( decoder_t *p_dec, bool *pb_new_picture, slice_t *p_slice,
                        int i_nal_ref_idc, int i_nal_type, const block_t *p_frag )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    uint8_t *pb_dec;
    int i_dec;
    int i_slice_type;
    slice_t slice;
    bs_t s;
    /* do not convert the whole frame */
    CreateDecodedNAL( &pb_dec, &i_dec, &p_frag->p_buffer[5],
                     __MIN( p_frag->i_buffer - 5, 60 ) );
    bs_init( &s, pb_dec, i_dec );
    /* first_mb_in_slice */
    /* int i_first_mb = */ bs_read_ue( &s );
    /* slice_type */
    switch( (i_slice_type = bs_read_ue( &s )) )
    {
    case 0: case 5:
        slice.i_frame_type = BLOCK_FLAG_TYPE_P;
        break;
    case 1: case 6:
        slice.i_frame_type = BLOCK_FLAG_TYPE_B;
        break;
    case 2: case 7:
        slice.i_frame_type = BLOCK_FLAG_TYPE_I;
        break;
    case 3: case 8: /* SP */
        slice.i_frame_type = BLOCK_FLAG_TYPE_P;
        break;
    case 4: case 9:
        slice.i_frame_type = BLOCK_FLAG_TYPE_I;
        break;
    default:
        slice.i_frame_type = 0;
        break;
    }
    /* */
    slice.i_nal_type = i_nal_type;
    slice.i_nal_ref_idc = i_nal_ref_idc;
    slice.i_pic_parameter_set_id = bs_read_ue( &s );
    slice.i_frame_num = bs_read( &s, p_sys->i_log2_max_frame_num + 4 );
    slice.i_field_pic_flag = 0;
    slice.i_bottom_field_flag = -1;
    if( !p_sys->b_frame_mbs_only )
    {
        /* field_pic_flag */
        slice.i_field_pic_flag = bs_read( &s, 1 );
        if( slice.i_field_pic_flag )
            slice.i_bottom_field_flag = bs_read( &s, 1 );
    }
    slice.i_idr_pic_id = p_sys->slice.i_idr_pic_id;
    if( slice.i_nal_type == NAL_SLICE_IDR )
        slice.i_idr_pic_id = bs_read_ue( &s );
    slice.i_pic_order_cnt_lsb = -1;
    slice.i_delta_pic_order_cnt_bottom = -1;
    slice.i_delta_pic_order_cnt0 = 0;
    slice.i_delta_pic_order_cnt1 = 0;
    if( p_sys->i_pic_order_cnt_type == 0 )
    {
        slice.i_pic_order_cnt_lsb = bs_read( &s, p_sys->i_log2_max_pic_order_cnt_lsb + 4 );
        if( p_sys->i_pic_order_present_flag && !slice.i_field_pic_flag )
            slice.i_delta_pic_order_cnt_bottom = bs_read_se( &s );
    }
    else if( (p_sys->i_pic_order_cnt_type == 1) &&
             (!p_sys->i_delta_pic_order_always_zero_flag) )
    {
        slice.i_delta_pic_order_cnt0 = bs_read_se( &s );
        if( p_sys->i_pic_order_present_flag && !slice.i_field_pic_flag )
            slice.i_delta_pic_order_cnt1 = bs_read_se( &s );
    }
    free( pb_dec );
    /* Detection of the first VCL NAL unit of a primary coded picture
     * (cf. 7.4.1.2.4) */
    bool b_pic = false;
    if( slice.i_frame_num != p_sys->slice.i_frame_num ||
        slice.i_pic_parameter_set_id != p_sys->slice.i_pic_parameter_set_id ||
        slice.i_field_pic_flag != p_sys->slice.i_field_pic_flag ||
        slice.i_nal_ref_idc != p_sys->slice.i_nal_ref_idc )
        b_pic = true;
    if( (slice.i_bottom_field_flag != -1) &&
        (p_sys->slice.i_bottom_field_flag != -1) &&
        (slice.i_bottom_field_flag != p_sys->slice.i_bottom_field_flag) )
        b_pic = true;
    if( p_sys->i_pic_order_cnt_type == 0 &&
        ( slice.i_pic_order_cnt_lsb != p_sys->slice.i_pic_order_cnt_lsb ||
          slice.i_delta_pic_order_cnt_bottom != p_sys->slice.i_delta_pic_order_cnt_bottom ) )
        b_pic = true;
    else if( p_sys->i_pic_order_cnt_type == 1 &&
             ( slice.i_delta_pic_order_cnt0 != p_sys->slice.i_delta_pic_order_cnt0 ||
               slice.i_delta_pic_order_cnt1 != p_sys->slice.i_delta_pic_order_cnt1 ) )
        b_pic = true;
    if( ( slice.i_nal_type == NAL_SLICE_IDR || p_sys->slice.i_nal_type == NAL_SLICE_IDR ) &&
        ( slice.i_nal_type != p_sys->slice.i_nal_type || slice.i_idr_pic_id != p_sys->slice.i_idr_pic_id ) )
            b_pic = true;
    /* */
    *pb_new_picture = b_pic;
    *p_slice = slice;
}
