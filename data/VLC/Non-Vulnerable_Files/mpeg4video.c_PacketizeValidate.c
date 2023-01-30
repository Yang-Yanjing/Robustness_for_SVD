}
static int PacketizeValidate( void *p_private, block_t *p_au )
{
    decoder_t *p_dec = p_private;
    decoder_sys_t *p_sys = p_dec->p_sys;
    /* We've just started the stream, wait for the first PTS.
     * We discard here so we can still get the sequence header. */
    if( p_sys->i_interpolated_pts <= VLC_TS_INVALID &&
        p_sys->i_interpolated_dts <= VLC_TS_INVALID )
    {
        msg_Dbg( p_dec, "need a starting pts/dts" );
        return VLC_EGENERIC;
    }
    /* When starting the stream we can have the first frame with
     * a null DTS (i_interpolated_pts is initialized to 0) */
    if( !p_au->i_dts )
        p_au->i_dts = p_au->i_pts;
    return VLC_SUCCESS;
}
