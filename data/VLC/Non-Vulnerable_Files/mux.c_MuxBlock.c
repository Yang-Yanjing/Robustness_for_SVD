}
static int MuxBlock( sout_mux_t *p_mux, sout_input_t *p_input )
{
    sout_mux_sys_t *p_sys = p_mux->p_sys;
    block_t *p_data = block_FifoGet( p_input->p_fifo );
    int i_stream = *((int *)p_input->p_sys);
    AVStream *p_stream = p_sys->oc->streams[i_stream];
    AVPacket pkt;
    memset( &pkt, 0, sizeof(AVPacket) );
    av_init_packet(&pkt);
    pkt.data = p_data->p_buffer;
    pkt.size = p_data->i_buffer;
    pkt.stream_index = i_stream;
    if( p_data->i_flags & BLOCK_FLAG_TYPE_I )
    {
#ifdef AVFMT_ALLOW_FLUSH
        /* Make sure we don't inadvertedly mark buffered data as keyframes. */
        if( p_sys->oc->oformat->flags & AVFMT_ALLOW_FLUSH )
            av_write_frame( p_sys->oc, NULL );
#endif
        p_sys->b_write_keyframe = true;
        pkt.flags |= AV_PKT_FLAG_KEY;
    }
    if( p_data->i_pts > 0 )
        pkt.pts = p_data->i_pts * p_stream->time_base.den /
            CLOCK_FREQ / p_stream->time_base.num;
    if( p_data->i_dts > 0 )
        pkt.dts = p_data->i_dts * p_stream->time_base.den /
            CLOCK_FREQ / p_stream->time_base.num;
    /* this is another hack to prevent libavformat from triggering the "non monotone timestamps" check in avformat/utils.c */
    p_stream->cur_dts = ( p_data->i_dts * p_stream->time_base.den /
            CLOCK_FREQ / p_stream->time_base.num ) - 1;
    if( av_write_frame( p_sys->oc, &pkt ) < 0 )
    {
        msg_Err( p_mux, "could not write frame (pts: %"PRId64", dts: %"PRId64") "
                 "(pkt pts: %"PRId64", dts: %"PRId64")",
                 p_data->i_pts, p_data->i_dts, pkt.pts, pkt.dts );
        block_Release( p_data );
        return VLC_EGENERIC;
    }
    block_Release( p_data );
    return VLC_SUCCESS;
}
