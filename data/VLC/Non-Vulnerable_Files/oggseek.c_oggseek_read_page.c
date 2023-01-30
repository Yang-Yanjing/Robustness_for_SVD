 ****************************************************************************/
int64_t oggseek_read_page( demux_t *p_demux )
{
    demux_sys_t *p_ogg = p_demux->p_sys  ;
    uint8_t header[PAGE_HEADER_BYTES+255];
    int i_nsegs;
    int i;
    int64_t i_in_pos;
    int64_t i_result;
    int i_page_size;
    char *buf;
    demux_sys_t *p_sys  = p_demux->p_sys;
    /* store position of this page */
    i_in_pos = p_ogg->i_input_position = stream_Tell( p_demux->s );
    if ( p_sys->b_page_waiting) {
        msg_Warn( p_demux, "Ogg page already loaded" );
        return 0;
    }
    if ( stream_Read ( p_demux->s, header, PAGE_HEADER_BYTES ) < PAGE_HEADER_BYTES )
    {
        stream_Seek( p_demux->s, i_in_pos );
        msg_Dbg ( p_demux, "Reached clean EOF in ogg file" );
        return 0;
    }
    i_nsegs = header[ PAGE_HEADER_BYTES - 1 ];
    if ( stream_Read ( p_demux->s, header+PAGE_HEADER_BYTES, i_nsegs ) < i_nsegs )
    {
        stream_Seek( p_demux->s, i_in_pos );
        msg_Warn ( p_demux, "Reached broken EOF in ogg file" );
        return 0;
    }
    i_page_size = PAGE_HEADER_BYTES + i_nsegs;
    for ( i = 0; i < i_nsegs; i++ )
    {
        i_page_size += header[ PAGE_HEADER_BYTES + i ];
    }
    ogg_sync_reset( &p_ogg->oy );
    buf = ogg_sync_buffer( &p_ogg->oy, i_page_size );
    memcpy( buf, header, PAGE_HEADER_BYTES + i_nsegs );
    i_result = stream_Read ( p_demux->s, (uint8_t*)buf + PAGE_HEADER_BYTES + i_nsegs,
                             i_page_size - PAGE_HEADER_BYTES - i_nsegs );
    ogg_sync_wrote( &p_ogg->oy, i_result + PAGE_HEADER_BYTES + i_nsegs );
    if ( ogg_sync_pageout( &p_ogg->oy, &p_ogg->current_page ) != 1 )
    {
        msg_Err( p_demux , "Got invalid packet, read %"PRId64" of %i: %s %"PRId64,
                 i_result, i_page_size, buf, i_in_pos );
        return 0;
    }
    return i_result + PAGE_HEADER_BYTES + i_nsegs;
}
