 ****************************************************************************/
static subpicture_t *DecodeBlock( decoder_t *p_dec, block_t **pp_block )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    block_t *p_block;
    kate_packet kp;
    if( !pp_block || !*pp_block )
        return NULL;
    p_block = *pp_block;
    if( p_block->i_flags & (BLOCK_FLAG_DISCONTINUITY|BLOCK_FLAG_CORRUPTED) )
    {
#ifdef HAVE_TIGER
        if( p_block->i_flags & BLOCK_FLAG_DISCONTINUITY)
        {
            /* Hmm, should we wait before flushing the renderer ? I think not, but not certain... */
            vlc_mutex_lock( &p_sys->lock );
            tiger_renderer_seek( p_sys->p_tr, 0 );
            vlc_mutex_unlock( &p_sys->lock );
        }
#endif
        p_sys->i_max_stop = VLC_TS_INVALID;
        block_Release( p_block );
        return NULL;
    }
    /* Block to Kate packet */
    kate_packet_wrap(&kp, p_block->i_buffer, p_block->p_buffer);
    if( !p_sys->b_has_headers )
    {
        if( ProcessHeaders( p_dec ) )
        {
            block_Release( *pp_block );
            return NULL;
        }
        p_sys->b_has_headers = true;
    }
    return ProcessPacket( p_dec, &kp, pp_block );
}
