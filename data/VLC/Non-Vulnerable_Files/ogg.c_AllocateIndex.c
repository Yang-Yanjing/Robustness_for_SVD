}
static bool AllocateIndex( sout_mux_t *p_mux, sout_input_t *p_input )
{
    ogg_stream_t *p_stream = (ogg_stream_t *) p_input->p_sys;
    size_t i_size;
    if ( p_stream->i_length )
    {
        uint64_t i_interval = (uint64_t)p_mux->p_sys->skeleton.i_index_intvl * 1000;
        uint64_t i;
        if( p_input->p_fmt->i_cat == VIDEO_ES &&
                p_input->p_fmt->video.i_frame_rate )
        {
            /* optimize for fps < 1 */
            i_interval= __MAX( p_mux->p_sys->skeleton.i_index_intvl * 1000,
                       INT64_C(10000000) *
                       p_input->p_fmt->video.i_frame_rate_base /
                       p_input->p_fmt->video.i_frame_rate );
        }
        size_t i_tuple_size = 0;
        /* estimate length of pos value */
        if ( p_input->p_fmt->i_bitrate )
        {
            i = i_interval * p_input->p_fmt->i_bitrate / 1000000;
            while ( i <<= 1 ) i_tuple_size++;
        }
        else
        {
            /* Likely 64KB<<keyframe interval<<16MB */
            /* We can't really guess due to muxing */
            i_tuple_size = 24 / 8;
        }
        /* add length of interval value */
        i = i_interval;
        while ( i <<= 1 ) i_tuple_size++;
        i_size = i_tuple_size * ( p_stream->i_length / i_interval + 2 );
    }
    else
    {
        i_size = ( INT64_C(3600) * 11.2 * 1000 / p_mux->p_sys->skeleton.i_index_intvl )
                * p_mux->p_sys->skeleton.i_index_ratio;
        msg_Dbg( p_mux, "No stream length, using default allocation for index" );
    }
    i_size *= ( 8.0 / 7 ); /* 7bits encoding overhead */
    msg_Dbg( p_mux, "allocating %zu bytes for index", i_size );
    p_stream->skeleton.p_index = calloc( i_size, sizeof(uint8_t) );
    if ( !p_stream->skeleton.p_index ) return false;
    p_stream->skeleton.i_index_size = i_size;
    p_stream->skeleton.i_index_payload = 0;
    return true;
}
