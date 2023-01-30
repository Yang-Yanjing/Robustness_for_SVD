 *****************************************************************************/
static block_t *OggStreamGetPage( sout_mux_t *p_mux,
                                  ogg_stream_state *p_os, mtime_t i_pts,
                                  bool flush )
{
    (void)p_mux;
    block_t *p_og, *p_og_first = NULL;
    ogg_page og;
    int (*pager)( ogg_stream_state*, ogg_page* ) = flush ? ogg_stream_flush : ogg_stream_pageout;
    while( pager( p_os, &og ) )
    {
        /* Flush all data */
        p_og = block_Alloc( og.header_len + og.body_len );
        memcpy( p_og->p_buffer, og.header, og.header_len );
        memcpy( p_og->p_buffer + og.header_len, og.body, og.body_len );
        p_og->i_dts     = 0;
        p_og->i_pts     = i_pts;
        p_og->i_length  = 0;
        i_pts = 0; // write it only once
        block_ChainAppend( &p_og_first, p_og );
    }
    return p_og_first;
}
