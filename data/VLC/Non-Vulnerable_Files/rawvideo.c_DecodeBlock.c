 ****************************************************************************/
static void *DecodeBlock( decoder_t *p_dec, block_t **pp_block )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    if( pp_block == NULL || *pp_block == NULL )
        return NULL;
    block_t *p_block = *pp_block;
    if( p_block->i_pts <= VLC_TS_INVALID && p_block->i_dts <= VLC_TS_INVALID &&
        !date_Get( &p_sys->pts ) )
    {
        /* We've just started the stream, wait for the first PTS. */
        block_Release( p_block );
        return NULL;
    }
    /* Date management: If there is a pts avaliable, use that. */
    if( p_block->i_pts > VLC_TS_INVALID )
    {
        date_Set( &p_sys->pts, p_block->i_pts );
    }
    else if( p_block->i_dts > VLC_TS_INVALID )
    {
        /* NB, davidf doesn't quite agree with this in general, it is ok
         * for rawvideo since it is in order (ie pts=dts), however, it
         * may not be ok for an out-of-order codec, so don't copy this
         * without thinking */
        date_Set( &p_sys->pts, p_block->i_dts );
    }
    if( p_block->i_buffer < p_sys->size )
    {
        msg_Warn( p_dec, "invalid frame size (%zu < %zu)",
                  p_block->i_buffer, p_sys->size );
        block_Release( p_block );
        return NULL;
    }
    *pp_block = NULL;
    return p_block;
}
