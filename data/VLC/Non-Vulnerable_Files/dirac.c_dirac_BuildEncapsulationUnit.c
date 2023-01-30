 */
static block_t *dirac_BuildEncapsulationUnit( decoder_t *p_dec, block_t *p_block )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    assert(p_block->i_buffer >= 13 && 0x42424344 == GetDWBE( p_block->p_buffer ));
    if( p_sys->i_eu_pts <= VLC_TS_INVALID && p_sys->i_eu_dts <= VLC_TS_INVALID )
    {
        /* earliest block with pts/dts gets to set the pts/dts for the dated
         * encapsulation unit as a whole */
        /* NB, the 'earliest block' criteria is aribtary */
        if( p_block->i_pts > VLC_TS_INVALID || p_block->i_dts > VLC_TS_INVALID )
        {
            p_sys->i_eu_pts = p_block->i_pts;
            p_sys->i_eu_dts = p_block->i_dts;
        }
    }
    /* inpectdataunit also updates flags for the EU.
     *  - if this is the first block in the EU, then it hasn't been added
     *    to the chain yet (so, p_block will become the front of the chain
     *  - otherwise, use the flags of the chain (first block) */
    block_t *p_eu = p_sys->p_eu ? p_sys->p_eu : p_block;
    int i_block = dirac_InspectDataUnit( p_dec, &p_block, p_eu);
    if( !p_block )
    {
        /* block has been discarded during inspection */
        /* becareful, don't discard anything that is dated,
         * as this needs to go into the timegen loop.  set
         * the DIRAC_DISCARD block flag, and it'll be dropped
         * at output time */
        return NULL;
    }
    block_ChainLastAppend( &p_sys->pp_eu_last, p_block );
    dirac_block_encap_t *p_dbe = dirac_GetBlockEncap( p_block );
#ifdef SANITIZE_PREV_PARSE_OFFSET
    /* fixup prev_parse_offset to point to the last data unit
     * to arrive */
    if( p_dbe )
    {
        SetDWBE( p_block->p_buffer + 9, p_sys->u_eu_last_npo );
        p_sys->u_eu_last_npo = p_dbe->u_last_next_offset;
    }
#endif
    if( i_block != DIRAC_DU_ENDS_EU )
    {
        /* encapsulation unit not ended */
        return NULL;
    }
    /* gather up encapsulation unit, reassociating the final
     * private state with the gathered block */
    block_t *p_eu_last = (block_t*) p_sys->pp_eu_last - offsetof( block_t, p_next );
    p_dbe = dirac_RemoveBlockEncap( p_eu_last );
    uint8_t u_parse_code = p_block->p_buffer[4];
    /* gather up the encapsulation unit */
    p_block = block_ChainGather( p_sys->p_eu );
    assert( p_block ); /* block_ChainGather doesn't define when it frees chain */
    p_block->i_flags |= DIRAC_NON_DATED;
    if( p_dbe )
    {
        dirac_AddBlockEncap( &p_block, p_dbe );
        if( dirac_isPicture( u_parse_code ) ) p_block->i_flags &= ~DIRAC_NON_DATED;
    }
    p_sys->p_eu = NULL;
    p_sys->pp_eu_last = &p_sys->p_eu;
    return p_block;
}
