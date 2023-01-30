}
static int PacketizeValidate( void *p_private, block_t *p_au )
{
    decoder_t *p_dec = p_private;
    decoder_sys_t *p_sys = p_dec->p_sys;
    /* If a discontinuity has been encountered, then wait till
     * the next Intra frame before continuing with packetizing */
    if( p_sys->b_discontinuity &&
        p_sys->b_sync_on_intra_frame )
    {
        if( (p_au->i_flags & BLOCK_FLAG_TYPE_I) == 0 )
        {
            msg_Dbg( p_dec, "waiting on intra frame" );
            return VLC_EGENERIC;
        }
        msg_Dbg( p_dec, "synced on intra frame" );
        p_sys->b_discontinuity = false;
        p_au->i_flags |= BLOCK_FLAG_DISCONTINUITY;
    }
    /* We've just started the stream, wait for the first PTS.
     * We discard here so we can still get the sequence header. */
    if( p_sys->i_dts <= VLC_TS_INVALID && p_sys->i_pts <= VLC_TS_INVALID &&
        p_sys->i_interpolated_dts <= VLC_TS_INVALID )
    {
        msg_Dbg( p_dec, "need a starting pts/dts" );
        return VLC_EGENERIC;
    }
    /* When starting the stream we can have the first frame with
     * an invalid DTS (i_interpolated_pts is initialized to VLC_TS_INVALID) */
    if( p_au->i_dts <= VLC_TS_INVALID )
        p_au->i_dts = p_au->i_pts;
    return VLC_SUCCESS;
}
