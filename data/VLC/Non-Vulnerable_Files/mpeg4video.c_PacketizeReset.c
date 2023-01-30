 *****************************************************************************/
static void PacketizeReset( void *p_private, bool b_broken )
{
    decoder_t *p_dec = p_private;
    decoder_sys_t *p_sys = p_dec->p_sys;
    if( b_broken )
    {
        if( p_sys->p_frame )
            block_ChainRelease( p_sys->p_frame );
        p_sys->p_frame = NULL;
        p_sys->pp_last = &p_sys->p_frame;
    }
    p_sys->i_interpolated_pts =
    p_sys->i_interpolated_dts =
    p_sys->i_last_ref_pts = VLC_TS_INVALID;
    p_sys->i_last_time_ref =
    p_sys->i_time_ref =
    p_sys->i_last_time =
    p_sys->i_last_timeincr = 0;
}
