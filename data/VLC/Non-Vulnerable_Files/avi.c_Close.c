 *****************************************************************************/
static void Close( vlc_object_t * p_this )
{
    sout_mux_t      *p_mux = (sout_mux_t*)p_this;
    sout_mux_sys_t  *p_sys = p_mux->p_sys;
    block_t       *p_hdr, *p_idx1;
    int                 i_stream;
    msg_Dbg( p_mux, "AVI muxer closed" );
    /* first create idx1 chunk (write at the end of the stream */
    p_idx1 = avi_HeaderCreateidx1( p_mux );
    p_sys->i_idx1_size = p_idx1->i_buffer;
    sout_AccessOutWrite( p_mux->p_access, p_idx1 );
    /* calculate some value for headers creations */
    for( i_stream = 0; i_stream < p_sys->i_streams; i_stream++ )
    {
        avi_stream_t *p_stream;
        p_stream = &p_sys->stream[i_stream];
        p_stream->f_fps = 25;
        if( p_stream->i_duration > 0 )
        {
            p_stream->f_fps = (float)p_stream->i_frames /
                              ( (float)p_stream->i_duration /
                                (float)1000000 );
        }
        p_stream->i_bitrate = 128 * 1024;
        if( p_stream->i_duration > 0 )
        {
            p_stream->i_bitrate =
                8 * (uint64_t)1000000 *
                    (uint64_t)p_stream->i_totalsize /
                    (uint64_t)p_stream->i_duration;
        }
        msg_Info( p_mux, "stream[%d] duration:%"PRId64" totalsize:%"PRId64
                  " frames:%d fps:%f KiB/s:%d",
                  i_stream,
                  (int64_t)p_stream->i_duration / (int64_t)1000000,
                  p_stream->i_totalsize,
                  p_stream->i_frames,
                  p_stream->f_fps, p_stream->i_bitrate/1024 );
    }
    p_hdr = avi_HeaderCreateRIFF( p_mux );
    sout_AccessOutSeek( p_mux->p_access, 0 );
    sout_AccessOutWrite( p_mux->p_access, p_hdr );
    for( i_stream = 0; i_stream < p_sys->i_streams; i_stream++ )
    {
        avi_stream_t *p_stream;
        p_stream = &p_sys->stream[i_stream];
        free( p_stream->p_bih );
        free( p_stream->p_wf );
    }
    free( p_sys->idx1.entry );
    free( p_sys );
}
