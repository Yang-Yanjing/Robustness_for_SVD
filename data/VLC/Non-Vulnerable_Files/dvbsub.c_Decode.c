 *****************************************************************************/
static subpicture_t *Decode( decoder_t *p_dec, block_t **pp_block )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    block_t       *p_block;
    subpicture_t  *p_spu = NULL;
    if( ( pp_block == NULL ) || ( *pp_block == NULL ) ) return NULL;
    p_block = *pp_block;
    *pp_block = NULL;
    /* configure for SD res in case DDS is not present */
    /* a change of PTS is a good indication we must get a new DDS */
    if (p_sys->i_pts != p_block->i_pts)
        default_dds_init( p_dec );
    p_sys->i_pts = p_block->i_pts;
    if( p_sys->i_pts <= VLC_TS_INVALID )
    {
#ifdef DEBUG_DVBSUB
        /* Some DVB channels send stuffing segments in non-dated packets so
         * don't complain too loudly. */
        msg_Warn( p_dec, "non dated subtitle" );
#endif
        block_Release( p_block );
        return NULL;
    }
    bs_init( &p_sys->bs, p_block->p_buffer, p_block->i_buffer );
    if( bs_read( &p_sys->bs, 8 ) != 0x20 ) /* Data identifier */
    {
        msg_Dbg( p_dec, "invalid data identifier" );
        block_Release( p_block );
        return NULL;
    }
    if( bs_read( &p_sys->bs, 8 ) ) /* Subtitle stream id */
    {
        msg_Dbg( p_dec, "invalid subtitle stream id" );
        block_Release( p_block );
        return NULL;
    }
#ifdef DEBUG_DVBSUB
    msg_Dbg( p_dec, "subtitle packet received: %"PRId64, p_sys->i_pts );
#endif
    p_sys->b_page = false;
    while( bs_show( &p_sys->bs, 8 ) == 0x0f ) /* Sync byte */
    {
        decode_segment( p_dec, &p_sys->bs );
    }
    if( ( bs_read( &p_sys->bs, 8 ) & 0x3f ) != 0x3f ) /* End marker */
    {
        msg_Warn( p_dec, "end marker not found (corrupted subtitle ?)" );
        block_Release( p_block );
        return NULL;
    }
    /* Check if the page is to be displayed */
    if( p_sys->p_page && p_sys->b_page )
        p_spu = render( p_dec );
    block_Release( p_block );
    return p_spu;
}
