 *****************************************************************************/
static int Mux( sout_mux_t *p_mux )
{
    sout_mux_sys_t *p_sys = p_mux->p_sys;
    mtime_t        i_dts;
    /* End any stream that ends in that group */
    if ( p_sys->i_del_streams )
    {
        /* Remove deleted logical streams */
        for( int i = 0; i < p_sys->i_del_streams; i++ )
        {
            OggCreateStreamFooter( p_mux, p_sys->pp_del_streams[i] );
            FREENULL( p_sys->pp_del_streams[i]->p_oggds_header );
            FREENULL( p_sys->pp_del_streams[i] );
        }
        FREENULL( p_sys->pp_del_streams );
        p_sys->i_del_streams = 0;
    }
    if ( p_sys->i_streams == 0 )
    {
        /* All streams have been deleted, or none have ever been created
           From this point, we are allowed to start a new group of logical streams */
        p_sys->skeleton.b_head_done = false;
        p_sys->b_can_add_streams = true;
        p_sys->i_segment_start = p_sys->i_pos;
    }
    if ( p_sys->i_add_streams )
    {
        if ( !p_sys->b_can_add_streams )
        {
            msg_Warn( p_mux, "Can't add new stream %d/%d: Considerer increasing sout-mux-caching variable", p_sys->i_del_streams, p_mux->p_sys->i_streams);
            msg_Warn( p_mux, "Resetting and setting new identity to current streams");
            /* resetting all active streams */
            for ( int i=0; i < p_mux->p_sys->i_streams; i++ )
            {
                ogg_stream_t * p_stream = (ogg_stream_t *) p_mux->pp_inputs[i]->p_sys;
                if ( p_stream->b_finished || !p_stream->b_started ) continue;
                OggCreateStreamFooter( p_mux, p_stream );
                p_stream->i_serial_no = p_sys->i_next_serial_no++;
                p_stream->i_packet_no = 0;
                p_stream->b_finished = true;
            }
            /* rewrite fishead with final values */
            if ( p_sys->skeleton.b_head_done )
            {
                OggRewriteFisheadPage( p_mux );
            }
            p_sys->b_can_add_streams = true;
            p_sys->skeleton.b_head_done = false;
            p_sys->i_segment_start = p_sys->i_pos;
        }
        /* Open new ogg stream */
        if( sout_MuxGetStream( p_mux, 1, &i_dts) < 0 )
        {
            msg_Dbg( p_mux, "waiting for data..." );
            return VLC_SUCCESS;
        }
        msg_Dbg( p_mux, "writing streams headers" );
        p_sys->i_start_dts = i_dts;
        p_sys->i_streams = p_mux->i_nb_inputs;
        p_sys->i_del_streams = 0;
        p_sys->i_add_streams = 0;
        p_sys->skeleton.b_create = true;
        if ( ! OggCreateHeaders( p_mux ) )
            return VLC_ENOMEM;
        /* If we're switching to end of headers, then that's data start */
        if ( p_sys->b_can_add_streams )
        {
            msg_Dbg( p_mux, "data starts from %zu", p_sys->i_pos );
            p_sys->i_data_start = p_sys->i_pos;
        }
        /* Since we started sending secondaryheader or data pages,
             * we're no longer allowed to create new streams, until all streams end */
        p_sys->b_can_add_streams = false;
    }
    /* Do the regular data mux thing */
    for( ;; )
    {
        int i_stream = sout_MuxGetStream( p_mux, 1, NULL );
        if( i_stream < 0 )
            return VLC_SUCCESS;
        MuxBlock( p_mux, p_mux->pp_inputs[i_stream] );
    }
    return VLC_SUCCESS;
}
