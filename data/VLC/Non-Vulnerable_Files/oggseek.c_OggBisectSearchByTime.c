/* returns pos */
static int64_t OggBisectSearchByTime( demux_t *p_demux, logical_stream_t *p_stream,
            int64_t i_targettime, int64_t i_pos_lower, int64_t i_pos_upper)
{
    int64_t i_start_pos;
    int64_t i_end_pos;
    int64_t i_segsize;
    struct
    {
        int64_t i_pos;
        int64_t i_timestamp;
        int64_t i_granule;
    } bestlower = { p_stream->i_data_start, -1, -1 },
      current = { -1, -1, -1 },
      lowestupper = { -1, -1, -1 };
    demux_sys_t *p_sys  = p_demux->p_sys;
    i_pos_lower = __MAX( i_pos_lower, p_stream->i_data_start );
    i_pos_upper = __MIN( i_pos_upper, p_sys->i_total_length );
    if ( i_pos_upper < 0 ) i_pos_upper = p_sys->i_total_length;
    i_start_pos = i_pos_lower;
    i_end_pos = i_pos_upper;
    i_segsize = ( i_end_pos - i_start_pos + 1 ) >> 1;
    i_start_pos += i_segsize;
    OggDebug( msg_Dbg(p_demux, "Bisecting for time=%"PRId64" between %"PRId64" and %"PRId64,
            i_targettime, i_pos_lower, i_pos_upper ) );
    do
    {
        /* see if the frame lies in current segment */
        i_start_pos = __MAX( i_start_pos, i_pos_lower );
        i_end_pos = __MIN( i_end_pos, i_pos_upper );
        if ( i_start_pos >= i_end_pos )
        {
            if ( i_start_pos == i_pos_lower)
            {
                return i_start_pos;
            }
            return -1;
        }
        current.i_pos = find_first_page_granule( p_demux,
                                                 i_start_pos, i_end_pos,
                                                 p_stream,
                                                 &current.i_granule );
        current.i_timestamp = Oggseek_GranuleToAbsTimestamp( p_stream,
                                                             current.i_granule, false );
        if ( current.i_timestamp == -1 && current.i_granule > 0 )
        {
            msg_Err( p_demux, "Unmatched granule. New codec ?" );
            return -1;
        }
        else if ( current.i_timestamp < -1 )  /* due to preskip with some codecs */
        {
            current.i_timestamp = 0;
        }
        if ( current.i_pos != -1 && current.i_granule != -1 )
        {
            /* found a page */
            if ( current.i_timestamp <= i_targettime )
            {
                /* set our lower bound */
                if ( current.i_timestamp > bestlower.i_timestamp )
                    bestlower = current;
                i_start_pos = current.i_pos;
            }
            else if ( current.i_timestamp > i_targettime )
            {
                if ( lowestupper.i_timestamp == -1 || current.i_timestamp < lowestupper.i_timestamp )
                    lowestupper = current;
                /* check lower half of segment */
                i_start_pos -= i_segsize;
                i_end_pos -= i_segsize;
            }
        }
        else
        {
            /* no keyframe found, check lower segment */
            i_end_pos -= i_segsize;
            i_start_pos -= i_segsize;
        }
        OggDebug( msg_Dbg(p_demux, "Bisect restart i_segsize=%"PRId64" between %"PRId64
                                   " and %"PRId64 " bl %"PRId64" lu %"PRId64,
                i_segsize, i_start_pos, i_end_pos, bestlower.i_granule, lowestupper.i_granule  ) );
        i_segsize = ( i_end_pos - i_start_pos + 1 ) >> 1;
        i_start_pos += i_segsize;
    } while ( i_segsize > 64 );
    if ( bestlower.i_granule == -1 )
    {
        if ( lowestupper.i_granule == -1 )
            return -1;
        else
            bestlower = lowestupper;
    }
    if ( p_stream->b_oggds )
    {
        int64_t a = OggBackwardSeekToFrame( p_demux,
                __MAX ( bestlower.i_pos - OGGSEEK_BYTES_TO_READ, p_stream->i_data_start ),
                bestlower.i_pos,
                p_stream, bestlower.i_granule /* unused */ );
        return a;
    }
    /* If not each packet is usable as keyframe, query the codec for keyframe */
    else if ( Ogg_GetKeyframeGranule( p_stream, bestlower.i_granule ) != bestlower.i_granule )
    {
        int64_t i_keyframegranule = Ogg_GetKeyframeGranule( p_stream, bestlower.i_granule );
        OggDebug( msg_Dbg( p_demux, "Need to reseek to keyframe (%"PRId64") granule (%"PRId64"!=%"PRId64") to t=%"PRId64,
                           i_keyframegranule >> p_stream->i_granule_shift,
                           bestlower.i_granule,
                           i_pos_upper,
                           Oggseek_GranuleToAbsTimestamp( p_stream, i_keyframegranule, false ) ) );
        OggDebug( msg_Dbg( p_demux, "Seeking back to %"PRId64, __MAX ( bestlower.i_pos - OGGSEEK_BYTES_TO_READ, p_stream->i_data_start ) ) );
        int64_t a = OggBackwardSeekToFrame( p_demux,
            __MAX ( bestlower.i_pos - OGGSEEK_BYTES_TO_READ, p_stream->i_data_start ),
            stream_Size( p_demux->s ), p_stream, i_keyframegranule );
        return a;
    }
    return bestlower.i_pos;
}
/************************************************************************
 * public functions
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
int Oggseek_SeektoAbsolutetime( demux_t *p_demux, logical_stream_t *p_stream,
                                int64_t i_time )
{
    demux_sys_t *p_sys  = p_demux->p_sys;
    OggDebug( msg_Dbg( p_demux, "=================== Seeking To Absolute Time %"PRId64, i_time ) );
    int64_t i_offset_lower = -1;
    int64_t i_offset_upper = -1;
    if ( Ogg_GetBoundsUsingSkeletonIndex( p_stream, i_time, &i_offset_lower, &i_offset_upper ) )
    {
        /* Exact match */
        OggDebug( msg_Dbg( p_demux, "Found keyframe at %"PRId64" using skeleton index", i_offset_lower ) );
        if ( i_offset_lower == -1 ) i_offset_lower = p_stream->i_data_start;
        p_sys->i_input_position = i_offset_lower;
        seek_byte( p_demux, p_sys->i_input_position );
        ogg_stream_reset( &p_stream->os );
        return i_offset_lower;
    }
    OggDebug( msg_Dbg( p_demux, "Search bounds set to %"PRId64" %"PRId64" using skeleton index", i_offset_lower, i_offset_upper ) );
    OggNoDebug(
        OggSeekIndexFind( p_stream, i_time, &i_offset_lower, &i_offset_upper )
    );
    i_offset_lower = __MAX( i_offset_lower, p_stream->i_data_start );
    i_offset_upper = __MIN( i_offset_upper, p_sys->i_total_length );
    int64_t i_pagepos = OggBisectSearchByTime( p_demux, p_stream, i_time,
                                       i_offset_lower, i_offset_upper);
    if ( i_pagepos >= 0 )
    {
        /* be sure to clear any state or read+pagein() will fail on same # */
        ogg_stream_reset( &p_stream->os );
        seek_byte( p_demux, p_sys->i_input_position );
    }
    /* Insert keyframe position into index */
    OggNoDebug(
    if ( i_pagepos >= p_stream->i_data_start )
        OggSeek_IndexAdd( p_stream, i_time, i_pagepos )
    );
    OggDebug( msg_Dbg( p_demux, "=================== Seeked To %"PRId64" time %"PRId64, i_pagepos, i_time ) );
    return i_pagepos;
}
/****************************************************************************
 * oggseek_read_page: Read a full Ogg page from the physical bitstream.
 ****************************************************************************
 * Returns number of bytes read. This should always be > 0
 * unless we are at the end of stream.
 *
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
