 ****************************************************************************/
static void PacketizeReset( void *p_private, bool b_broken )
{
    decoder_t *p_dec = p_private;
    decoder_sys_t *p_sys = p_dec->p_sys;
    if( b_broken )
    {
        if( p_sys->p_frame )
            block_ChainRelease( p_sys->p_frame );
        p_sys->p_frame = NULL;
        p_sys->b_frame_sps = false;
        p_sys->b_frame_pps = false;
        p_sys->slice.i_frame_type = 0;
        p_sys->b_slice = false;
    }
    p_sys->i_frame_pts = VLC_TS_INVALID;
    p_sys->i_frame_dts = VLC_TS_INVALID;
}
