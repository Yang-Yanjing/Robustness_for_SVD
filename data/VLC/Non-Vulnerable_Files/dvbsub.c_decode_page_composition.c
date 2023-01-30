}
static void decode_page_composition( decoder_t *p_dec, bs_t *s )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    int i_version, i_state, i_segment_length, i_timeout, i;
    /* A page is composed by 0 or more region */
    i_segment_length = bs_read( s, 16 );
    i_timeout = bs_read( s, 8 );
    i_version = bs_read( s, 4 );
    i_state = bs_read( s, 2 );
    bs_skip( s, 2 ); /* Reserved */
    if( i_state == DVBSUB_PCS_STATE_CHANGE )
    {
        /* End of an epoch, reset decoder buffer */
#ifdef DEBUG_DVBSUB
        msg_Dbg( p_dec, "page composition mode change" );
#endif
        free_all( p_dec );
    }
    else if( !p_sys->p_page && ( i_state != DVBSUB_PCS_STATE_ACQUISITION ) &&
             ( i_state != DVBSUB_PCS_STATE_CHANGE ) )
    {
        /* Not a full PCS, we need to wait for one */
        msg_Dbg( p_dec, "didn't receive an acquisition page yet" );
#if 0
        /* Try to start decoding even without an acquisition page */
        bs_skip( s,  8 * (i_segment_length - 2) );
        return;
#endif
    }
#ifdef DEBUG_DVBSUB
    if( i_state == DVBSUB_PCS_STATE_ACQUISITION )
        msg_Dbg( p_dec, "acquisition page composition" );
#endif
    /* Check version number */
    if( p_sys->p_page && ( p_sys->p_page->i_version == i_version ) )
    {
        bs_skip( s,  8 * (i_segment_length - 2) );
        return;
    }
    else if( p_sys->p_page )
    {
        if( p_sys->p_page->i_region_defs )
            free( p_sys->p_page->p_region_defs );
        p_sys->p_page->p_region_defs = NULL;
        p_sys->p_page->i_region_defs = 0;
    }
    if( !p_sys->p_page )
    {
#ifdef DEBUG_DVBSUB
        msg_Dbg( p_dec, "new page" );
#endif
        /* Allocate a new page */
        p_sys->p_page = malloc( sizeof(dvbsub_page_t) );
        if( !p_sys->p_page )
            return;
    }
    p_sys->p_page->i_version = i_version;
    p_sys->p_page->i_timeout = i_timeout;
    p_sys->b_page = true;
    /* Number of regions */
    p_sys->p_page->i_region_defs = (i_segment_length - 2) / 6;
    if( p_sys->p_page->i_region_defs == 0 ) return;
    p_sys->p_page->p_region_defs =
        malloc( p_sys->p_page->i_region_defs * sizeof(dvbsub_regiondef_t) );
    if( p_sys->p_page->p_region_defs )
    {
        for( i = 0; i < p_sys->p_page->i_region_defs; i++ )
        {
            p_sys->p_page->p_region_defs[i].i_id = bs_read( s, 8 );
            bs_skip( s, 8 ); /* Reserved */
            p_sys->p_page->p_region_defs[i].i_x = bs_read( s, 16 );
            p_sys->p_page->p_region_defs[i].i_y = bs_read( s, 16 );
#ifdef DEBUG_DVBSUB
            msg_Dbg( p_dec, "page_composition, region %i (%i,%i)",
                    i, p_sys->p_page->p_region_defs[i].i_x,
                    p_sys->p_page->p_region_defs[i].i_y );
#endif
        }
    }
}
