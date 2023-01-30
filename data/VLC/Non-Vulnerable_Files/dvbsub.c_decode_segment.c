}
static void decode_segment( decoder_t *p_dec, bs_t *s )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    int i_type;
    int i_page_id;
    int i_size;
    /* sync_byte (already checked) */
    bs_skip( s, 8 );
    /* segment type */
    i_type = bs_read( s, 8 );
    /* page id */
    i_page_id = bs_read( s, 16 );
    /* segment size */
    i_size = bs_show( s, 16 );
    if( ( i_page_id != p_sys->i_id ) &&
        ( i_page_id != p_sys->i_ancillary_id ) )
    {
#ifdef DEBUG_DVBSUB
        msg_Dbg( p_dec, "subtitle skipped (page id: %i, %i)",
                 i_page_id, p_sys->i_id );
#endif
        bs_skip( s,  8 * ( 2 + i_size ) );
        return;
    }
    if( ( p_sys->i_ancillary_id != p_sys->i_id ) &&
        ( i_type == DVBSUB_ST_PAGE_COMPOSITION ) &&
        ( i_page_id == p_sys->i_ancillary_id ) )
    {
#ifdef DEBUG_DVBSUB
        msg_Dbg( p_dec, "skipped invalid ancillary subtitle packet" );
#endif
        bs_skip( s,  8 * ( 2 + i_size ) );
        return;
    }
#ifdef DEBUG_DVBSUB
    if( i_page_id == p_sys->i_id )
        msg_Dbg( p_dec, "segment (id: %i)", i_page_id );
    else
        msg_Dbg( p_dec, "ancillary segment (id: %i)", i_page_id );
#endif
    switch( i_type )
    {
    case DVBSUB_ST_PAGE_COMPOSITION:
#ifdef DEBUG_DVBSUB
        msg_Dbg( p_dec, "decode_page_composition" );
#endif
        decode_page_composition( p_dec, s );
        break;
    case DVBSUB_ST_REGION_COMPOSITION:
#ifdef DEBUG_DVBSUB
        msg_Dbg( p_dec, "decode_region_composition" );
#endif
        decode_region_composition( p_dec, s );
        break;
    case DVBSUB_ST_CLUT_DEFINITION:
#ifdef DEBUG_DVBSUB
        msg_Dbg( p_dec, "decode_clut" );
#endif
        decode_clut( p_dec, s );
        break;
    case DVBSUB_ST_OBJECT_DATA:
#ifdef DEBUG_DVBSUB
        msg_Dbg( p_dec, "decode_object" );
#endif
        decode_object( p_dec, s );
        break;
    case DVBSUB_ST_DISPLAY_DEFINITION:
#ifdef DEBUG_DVBSUB
        msg_Dbg( p_dec, "decode_display_definition" );
#endif
        decode_display_definition( p_dec, s );
        break;
    case DVBSUB_ST_ENDOFDISPLAY:
#ifdef DEBUG_DVBSUB
        msg_Dbg( p_dec, "end of display" );
#endif
        bs_skip( s,  8 * ( 2 + i_size ) );
        break;
    case DVBSUB_ST_STUFFING:
#ifdef DEBUG_DVBSUB
        msg_Dbg( p_dec, "skip stuffing" );
#endif
        bs_skip( s,  8 * ( 2 + i_size ) );
        break;
    default:
        msg_Warn( p_dec, "unsupported segment type: (%04x)", i_type );
        bs_skip( s,  8 * ( 2 + i_size ) );
        break;
    }
}
