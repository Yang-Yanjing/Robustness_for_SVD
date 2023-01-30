}
int Oggseek_BlindSeektoPosition( demux_t *p_demux, logical_stream_t *p_stream,
                                 double f, bool b_canfastseek )
{
    OggDebug( msg_Dbg( p_demux, "=================== Seeking To Blind Pos" ) );
    int64_t i_size = stream_Size( p_demux->s );
    int64_t i_granule;
    int64_t i_pagepos;
    i_size = find_first_page_granule( p_demux,
                                             i_size * f, i_size,
                                             p_stream,
                                             &i_granule );
    OggDebug( msg_Dbg( p_demux, "Seek start pos is %"PRId64" granule %"PRId64, i_size, i_granule ) );
    i_granule = Ogg_GetKeyframeGranule( p_stream, i_granule );
    if ( b_canfastseek )
    {
        /* Peek back until we meet a keyframe to start our decoding up to our
         * final seek time */
        i_pagepos = OggBackwardSeekToFrame( p_demux,
                __MAX ( i_size - MAX_PAGE_SIZE, p_stream->i_data_start ),
                __MIN ( i_size + MAX_PAGE_SIZE, p_demux->p_sys->i_total_length ),
                p_stream, i_granule );
    }
    else
    {
        /* Otherwise, we just sync to the next keyframe we meet */
        i_pagepos = OggForwardSeekToFrame( p_demux,
                __MAX ( i_size - MAX_PAGE_SIZE, p_stream->i_data_start ),
                stream_Size( p_demux->s ),
                p_stream, i_granule, false );
    }
    OggDebug( msg_Dbg( p_demux, "=================== Seeked To %"PRId64" granule %"PRId64, i_pagepos, i_granule ) );
    return i_pagepos;
}
