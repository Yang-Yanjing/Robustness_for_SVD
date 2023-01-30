 *****************************************************************************/
static int DelStream( sout_mux_t *p_mux, sout_input_t *p_input )
{
    sout_mux_sys_t *p_sys  = p_mux->p_sys;
    ogg_stream_t   *p_stream = (ogg_stream_t*)p_input->p_sys;
    block_t *p_og;
    msg_Dbg( p_mux, "removing input" );
    /* flush all remaining data */
    if( p_input->p_sys )
    {
        if( !p_stream->b_new )
        {
            while( block_FifoCount( p_input->p_fifo ) )
                MuxBlock( p_mux, p_input );
        }
        if( !p_stream->b_new &&
            ( p_og = OggStreamFlush( p_mux, &p_stream->os, 0 ) ) )
        {
            OggSetDate( p_og, p_stream->i_dts, p_stream->i_length );
            p_mux->p_sys->i_pos += sout_AccessOutWrite( p_mux->p_access, p_og );
        }
        /* move input in delete queue */
        if( !p_stream->b_new )
        {
            p_sys->pp_del_streams = xrealloc( p_sys->pp_del_streams,
                        (p_sys->i_del_streams + 1) * sizeof(ogg_stream_t *) );
            p_sys->pp_del_streams[p_sys->i_del_streams++] = p_stream;
        }
        else
        {
            /* wasn't already added so get rid of it */
            FREENULL( p_stream->p_oggds_header );
            FREENULL( p_stream );
            p_sys->i_add_streams--;
        }
    }
    p_input->p_sys = NULL;
    return 0;
}
