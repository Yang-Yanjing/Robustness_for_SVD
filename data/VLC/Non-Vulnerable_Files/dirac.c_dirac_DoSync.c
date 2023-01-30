 */
static block_t *dirac_DoSync( decoder_t *p_dec )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    parse_info_t pu;
    static const uint8_t p_parsecode[4] = {'B','B','C','D'};
    do {
        switch( p_sys->i_state )
        {
        case NOT_SYNCED:
        {
            if( VLC_SUCCESS !=
                block_FindStartcodeFromOffset( &p_sys->bytestream, &p_sys->i_offset, p_parsecode, 4 ) )
            {
                /* p_sys->i_offset will have been set to:
                 *   end of bytestream - amount of prefix found
                 * can resume search from this point when more data arrives */
                return NULL;
            }
            /* candidate parse_code_prefix has been found at p_sys->i_offset */
            if( VLC_SUCCESS != block_PeekOffsetBytes( &p_sys->bytestream, p_sys->i_offset + 12, NULL, 0 ) )
            {
                /* insufficient data has been accumulated to fully extract
                 * a parse_info header. exit for now in the hope of more
                 * data later to retry at exactly the same point */
                return NULL;
            }
            p_sys->i_state = TRY_SYNC;
            break; /* candidate found, try to sync */
        }
        case SYNCED: /* -> TRY_SYNC | NOT_SYNCED */
            /* sanity: can only reach this after having extracted a DU,
             * which causes data to be consumed and local i_offset to be reset */
            assert( p_sys->i_offset == 0 );
            if( VLC_SUCCESS != block_PeekOffsetBytes( &p_sys->bytestream, 12, NULL, 0 ) )
            {
                /* insufficient data has been accumulated to fully extract
                 * a parse_info header, retry later */
                return NULL;
            }
            if( !dirac_UnpackParseInfo( &pu, &p_sys->bytestream, 0 )
             || !pu.u_next_offset || (p_sys->u_last_npo != pu.u_prev_offset) )
            {
                /* !a: not a valid parse info.
                 * !pu.u_next_offset: don't know the length of the data unit
                 *                    search for the next one that points back
                 *                    to this one to determine length.
                 * (p_sys->u_last_npo != pu.u_prev_offset): some desync
                 */
                p_sys->i_state = NOT_SYNCED;
                break;
            }
            if( pu.u_next_offset > 1024*1024 )
            {
                /* sanity check for erronious hugs next_parse_offsets
                 * (eg, 2^32-1) that would cause a very long wait
                 * and large space consumption: fall back to try sync */
                p_sys->i_state = TRY_SYNC;
                break;
            }
            /* check that the start of the next data unit is avaliable */
            if( VLC_SUCCESS !=
                block_PeekOffsetBytes( &p_sys->bytestream, pu.u_next_offset + 12, NULL, 0 ) )
            {
                return NULL; /* retry later */
            }
            /* attempt to synchronise backwards from pu.u_next_offset */
            p_sys->i_offset = pu.u_next_offset;
            /* fall through */
        case TRY_SYNC: /* -> SYNCED | NOT_SYNCED */
        {
            if( !p_sys->i_offset )
                goto sync_fail; /* if a is at start of bytestream, b can't be in buffer */
            parse_info_t pu_a;
            bool a = dirac_UnpackParseInfo( &pu_a, &p_sys->bytestream, p_sys->i_offset );
            if( !a || (pu_a.u_prev_offset > p_sys->i_offset) )
                goto sync_fail; /* b lies beyond start of bytestream: can't sync */
            if( !pu_a.u_prev_offset )
            {
                if( p_sys->i_state == TRY_SYNC )
                {
                    goto sync_fail; /* can't find different pu_b from pu_a */
                }
                /* state == SYNCED: already know where pu_b is.
                 * pu_a has probably been inserted by something that doesn't
                 * know what the last next_parse_offset was */
                pu_a.u_prev_offset = pu.u_next_offset;
            }
            parse_info_t *pu_b = &pu;
            bool b = dirac_UnpackParseInfo( pu_b, &p_sys->bytestream, p_sys->i_offset - pu_a.u_prev_offset );
            if( !b || (pu_b->u_next_offset && pu_a.u_prev_offset != pu_b->u_next_offset) )
            {
                /* if pu_b->u_next_offset = 0, have to assume we've synced, ie,
                 * just rely on finding a valid pu_b from pu_a. */
                goto sync_fail;
            }
            p_sys->u_last_npo = pu_b->u_next_offset;
            if( !pu_b->u_next_offset ) pu_b->u_next_offset = pu_a.u_prev_offset;
            /* offset was pointing at pu_a, rewind to point at pu_b */
            p_sys->i_offset -= pu_a.u_prev_offset;
            p_sys->i_state = SYNCED;
            break;
        }
sync_fail:
            if( p_sys->i_state == SYNCED ) p_sys->i_offset = 0;
            p_sys->i_offset++;
            p_sys->i_state = NOT_SYNCED;
            break; /* find somewhere else to try again */
        default:;
        }
    } while( SYNCED != p_sys->i_state );
    /*
     * synced, attempt to extract a data unit
     */
    /* recover any timestamps from the data that is about to be flushed */
    dirac_RecoverTimestamps( p_dec, p_sys->i_offset );
    /* flush everything up to the start of the DU */
    block_SkipBytes( &p_sys->bytestream, p_sys->i_offset );
    block_BytestreamFlush( &p_sys->bytestream );
    p_sys->i_offset = 0;
    /* setup the data unit buffer */
    block_t *p_block = block_Alloc( pu.u_next_offset );
    if( !p_block )
        return NULL;
    p_block->i_pts = p_sys->i_sync_pts;
    p_block->i_dts = p_sys->i_sync_dts;
    p_sys->i_sync_pts = p_sys->i_sync_dts = VLC_TS_INVALID;
    /* recover any new timestamps from the data that is about to be consumed */
    dirac_RecoverTimestamps( p_dec, p_sys->i_offset );
    block_GetBytes( &p_sys->bytestream, p_block->p_buffer, p_block->i_buffer );
    /* save parse offset in private area for later use */
    dirac_block_encap_t *p_dbe = dirac_NewBlockEncap( &p_block );
    if( p_dbe ) p_dbe->u_last_next_offset = pu.u_next_offset;
    return p_block;
}
