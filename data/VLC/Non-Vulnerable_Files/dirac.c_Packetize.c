 *****************************************************************************/
static block_t *Packetize( decoder_t *p_dec, block_t **pp_block )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    block_t *p_block = NULL;
    int i_flushing = 0;
    if( pp_block && *pp_block )
    {
        p_block = *pp_block;
        *pp_block = NULL;
        if( p_block->i_flags & BLOCK_FLAG_DISCONTINUITY )
        {
            /* pre-emptively insert an EOS at a discontinuity, protects
             * any decoders from any sudden changes */
            block_Release( p_block );
            p_block = dirac_EmitEOS( p_dec, 0 );
            if( p_block )
            {
                p_block->p_next = dirac_EmitEOS( p_dec, 13 );
                /* need two EOS to ensure it gets detected by synchro
                 * duplicates get discarded in forming encapsulation unit */
            }
        }
        else if( p_block->i_flags & BLOCK_FLAG_CORRUPTED )
        {
            /* silently discard corruption sentinels,
             * synchronizer will then discard affected data units.
             * do not produce an EOS data unit as this is very
             * disruptive to the stream (and may make a larger error). */
            block_Release( p_block );
            p_block = NULL;
        }
        if( p_block )
            block_BytestreamPush( &p_sys->bytestream, p_block );
    }
    /* form as many encapsulation units as possible, give up
     * when the synchronizer runs out of input data */
    while( ( p_block = dirac_DoSync( p_dec ) ) )
    {
        p_block = dirac_BuildEncapsulationUnit( p_dec, p_block );
        if( !p_block )
            continue;
        /* add to tail of output queue (ie, not reordered) */
        block_ChainLastAppend( &p_sys->pp_outqueue_last, p_block );
        /* insert encapsulation unit into timestamp generator
         * which then calculates some timestamps if required */
        i_flushing = dirac_TimeGenPush( p_dec, p_block );
        if( i_flushing )
            break;
    }
    block_t *p_output = NULL;
    block_t **pp_output = &p_output;
    /* extract all the dated packets from the head of the output queue */
    /* explicitly nondated packets repeat the previous timestamps to
     * stop vlc discarding them */
    while( (p_block = p_sys->p_outqueue) )
    {
        if( p_block->i_flags & DIRAC_DISCARD )
        {
            p_sys->p_outqueue = p_block->p_next;
            p_block->p_next = NULL;
            block_Release( p_block );
            continue;
        }
        if( i_flushing || p_block->i_flags & DIRAC_NON_DATED )
        {
            p_block->i_dts = p_sys->i_dts_last_out;
            p_block->i_pts = p_sys->i_pts_last_out;
        }
        else if( p_block->i_pts <= VLC_TS_INVALID ) break;
        else if( p_block->i_dts <= VLC_TS_INVALID ) break;
        p_sys->i_dts_last_out = p_block->i_dts;
        p_sys->i_pts_last_out = p_block->i_pts;
        p_sys->p_outqueue = p_block->p_next;
        p_block->p_next = NULL;
        /* clear any flags we set */
        p_block->i_flags &= ~BLOCK_FLAG_PRIVATE_MASK;
        block_ChainLastAppend( &pp_output, p_block );
        mtime_t i_delay = p_block->i_pts - p_block->i_dts;
        if( i_delay < 0 )
            msg_Err( p_dec, "pts - dts is negative(%"PRId64"): incorrect RoB size", i_delay );
    }
    if( i_flushing )
    {
        p_sys->i_eu_dts = p_sys->i_eu_pts = VLC_TS_INVALID;
        /* reset timegen state (except synchronizer) */
        p_sys->b_seen_seq_hdr = false;
        if( i_flushing < 2 )
        {
            /* this state isn't safe to loose if there was
             * an unsignalled discontinuity */
            p_sys->b_pts = p_sys->b_dts = false;
        }
        p_sys->b_tg_last_picnum = false;
        dirac_ReorderInit( &p_sys->reorder_buf );
        assert( p_sys->p_outqueue == NULL );
    }
    /* perform sanity check:
     *  if there were a block at the front of outqueue that never
     *  satisfied the extraction criteria, but all blocks after did,
     *  the output queue would grow bounded by the stream length.
     * If there are 10 data units in the output queue, assume this
     * has happened and purge all blocks that fail extraction criteria */
    int i_count;
    block_ChainProperties( p_sys->p_outqueue, &i_count, NULL, NULL );
    if( i_count > 9 )
    {
        p_block = p_sys->p_outqueue;
        while( p_block )
        {
            block_t *p_block_next = p_block->p_next;
            if( p_block->i_pts > VLC_TS_INVALID && p_block->i_dts > VLC_TS_INVALID )
                break;
            dirac_ReorderDequeueAndReleaseBlock( p_dec, p_block );
            p_sys->p_outqueue = p_block = p_block_next;
        }
    }
    if( !p_sys->p_outqueue )
    {
        p_sys->pp_outqueue_last = &p_sys->p_outqueue;
    }
    return p_output;
}
