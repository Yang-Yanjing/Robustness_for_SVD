 *************************************************************************/
int Oggseek_BlindSeektoAbsoluteTime( demux_t *p_demux, logical_stream_t *p_stream,
                                     int64_t i_time, bool b_fastseek )
{
    demux_sys_t *p_sys  = p_demux->p_sys;
    int64_t i_lowerpos = -1;
    int64_t i_upperpos = -1;
    bool b_found = false;
    /* Search in skeleton */
    Ogg_GetBoundsUsingSkeletonIndex( p_stream, i_time, &i_lowerpos, &i_upperpos );
    if ( i_lowerpos != -1 ) b_found = true;
    /* And also search in our own index */
    if ( !b_found && OggSeekIndexFind( p_stream, i_time, &i_lowerpos, &i_upperpos ) )
    {
        b_found = true;
    }
    /* Or try to be smart with audio fixed bitrate streams */
    if ( !b_found && p_stream->fmt.i_cat == AUDIO_ES && p_sys->i_streams == 1
         && p_sys->i_bitrate && Ogg_GetKeyframeGranule( p_stream, 0xFF00FF00 ) == 0xFF00FF00 )
    {
        /* But only if there's no keyframe/preload requirements */
        /* FIXME: add function to get preload time by codec, ex: opus */
        i_lowerpos = i_time * p_sys->i_bitrate / INT64_C(8000000);
        b_found = true;
    }
    /* or search */
    if ( !b_found && b_fastseek )
    {
        i_lowerpos = OggBisectSearchByTime( p_demux, p_stream, i_time,
                                            p_stream->i_data_start, p_sys->i_total_length );
        b_found = ( i_lowerpos != -1 );
    }
    if ( !b_found ) return -1;
    if ( i_lowerpos < p_stream->i_data_start || i_upperpos > p_sys->i_total_length )
        return -1;
    /* And really do seek */
    p_sys->i_input_position = i_lowerpos;
    seek_byte( p_demux, p_sys->i_input_position );
    ogg_stream_reset( &p_stream->os );
    return i_lowerpos;
}
