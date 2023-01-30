}
static int ParseVOP( decoder_t *p_dec, block_t *p_vop )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    int64_t i_time_increment, i_time_ref;
    int i_modulo_time_base = 0, i_time_increment_bits;
    bs_t s;
    bs_init( &s, &p_vop->p_buffer[4], p_vop->i_buffer - 4 );
    switch( bs_read( &s, 2 ) )
    {
    case 0:
        p_sys->i_flags = BLOCK_FLAG_TYPE_I;
        break;
    case 1:
        p_sys->i_flags = BLOCK_FLAG_TYPE_P;
        break;
    case 2:
        p_sys->i_flags = BLOCK_FLAG_TYPE_B;
        p_sys->b_frame = true;
        break;
    case 3: /* gni ? */
        p_sys->i_flags = BLOCK_FLAG_TYPE_PB;
        break;
    }
    while( bs_read( &s, 1 ) ) i_modulo_time_base++;
    if( !bs_read1( &s ) ) return VLC_EGENERIC; /* Marker */
    /* VOP time increment */
    i_time_increment_bits = vlc_log2(p_dec->p_sys->i_fps_num - 1) + 1;
    if( i_time_increment_bits < 1 ) i_time_increment_bits = 1;
    i_time_increment = bs_read( &s, i_time_increment_bits );
    /* Interpolate PTS/DTS */
    if( !(p_sys->i_flags & BLOCK_FLAG_TYPE_B) )
    {
        p_sys->i_last_time_ref = p_sys->i_time_ref;
        p_sys->i_time_ref +=
            (i_modulo_time_base * p_dec->p_sys->i_fps_num);
        i_time_ref = p_sys->i_time_ref;
    }
    else
    {
        i_time_ref = p_sys->i_last_time_ref +
            (i_modulo_time_base * p_dec->p_sys->i_fps_num);
    }
#if 0
    msg_Err( p_dec, "interp pts/dts (%lli,%lli), pts/dts (%lli,%lli)",
             p_sys->i_interpolated_pts, p_sys->i_interpolated_dts,
             p_vop->i_pts, p_vop->i_dts );
#endif
    if( p_dec->p_sys->i_fps_num < 5 && /* Work-around buggy streams */
        p_dec->fmt_in.video.i_frame_rate > 0 &&
        p_dec->fmt_in.video.i_frame_rate_base > 0 )
    {
        p_sys->i_interpolated_pts += INT64_C(1000000) *
        p_dec->fmt_in.video.i_frame_rate_base /
        p_dec->fmt_in.video.i_frame_rate;
    }
    else if( p_dec->p_sys->i_fps_num )
        p_sys->i_interpolated_pts +=
            ( INT64_C(1000000) * (i_time_ref + i_time_increment -
              p_sys->i_last_time - p_sys->i_last_timeincr) /
              p_dec->p_sys->i_fps_num );
    p_sys->i_last_time = i_time_ref;
    p_sys->i_last_timeincr = i_time_increment;
    /* Correct interpolated dts when we receive a new pts/dts */
    if( p_vop->i_pts > VLC_TS_INVALID )
        p_sys->i_interpolated_pts = p_vop->i_pts;
    if( p_vop->i_dts > VLC_TS_INVALID )
        p_sys->i_interpolated_dts = p_vop->i_dts;
    if( (p_sys->i_flags & BLOCK_FLAG_TYPE_B) || !p_sys->b_frame )
    {
        /* Trivial case (DTS == PTS) */
        p_sys->i_interpolated_dts = p_sys->i_interpolated_pts;
        if( p_vop->i_pts > VLC_TS_INVALID )
            p_sys->i_interpolated_dts = p_vop->i_pts;
        if( p_vop->i_dts > VLC_TS_INVALID )
            p_sys->i_interpolated_dts = p_vop->i_dts;
        p_sys->i_interpolated_pts = p_sys->i_interpolated_dts;
    }
    else
    {
        if( p_sys->i_last_ref_pts > VLC_TS_INVALID )
            p_sys->i_interpolated_dts = p_sys->i_last_ref_pts;
        p_sys->i_last_ref_pts = p_sys->i_interpolated_pts;
    }
    return VLC_SUCCESS;
}
