 *****************************************************************************/
int demux_vaControlHelper( stream_t *s,
                            int64_t i_start, int64_t i_end,
                            int64_t i_bitrate, int i_align,
                            int i_query, va_list args )
{
    int64_t i_tell;
    double  f, *pf;
    int64_t i64, *pi64;
    if( i_end < 0 )    i_end   = stream_Size( s );
    if( i_start < 0 )  i_start = 0;
    if( i_align <= 0 ) i_align = 1;
    i_tell = stream_Tell( s );
    switch( i_query )
    {
        case DEMUX_GET_LENGTH:
            pi64 = (int64_t*)va_arg( args, int64_t * );
            if( i_bitrate > 0 && i_end > i_start )
            {
                *pi64 = INT64_C(8000000) * (i_end - i_start) / i_bitrate;
                return VLC_SUCCESS;
            }
            return VLC_EGENERIC;
        case DEMUX_GET_TIME:
            pi64 = (int64_t*)va_arg( args, int64_t * );
            if( i_bitrate > 0 && i_tell >= i_start )
            {
                *pi64 = INT64_C(8000000) * (i_tell - i_start) / i_bitrate;
                return VLC_SUCCESS;
            }
            return VLC_EGENERIC;
        case DEMUX_GET_POSITION:
            pf = (double*)va_arg( args, double * );
            if( i_start < i_end )
            {
                *pf = (double)( i_tell - i_start ) /
                      (double)( i_end  - i_start );
                return VLC_SUCCESS;
            }
            return VLC_EGENERIC;
        case DEMUX_SET_POSITION:
            f = (double)va_arg( args, double );
            if( i_start < i_end && f >= 0.0 && f <= 1.0 )
            {
                int64_t i_block = (f * ( i_end - i_start )) / i_align;
                if( stream_Seek( s, i_start + i_block * i_align ) )
                {
                    return VLC_EGENERIC;
                }
                return VLC_SUCCESS;
            }
            return VLC_EGENERIC;
        case DEMUX_SET_TIME:
            i64 = (int64_t)va_arg( args, int64_t );
            if( i_bitrate > 0 && i64 >= 0 )
            {
                int64_t i_block = i64 * i_bitrate / INT64_C(8000000) / i_align;
                if( stream_Seek( s, i_start + i_block * i_align ) )
                {
                    return VLC_EGENERIC;
                }
                return VLC_SUCCESS;
            }
            return VLC_EGENERIC;
        case DEMUX_GET_META:
            return stream_vaControl( s, STREAM_GET_META, args );
        case DEMUX_GET_PTS_DELAY:
        case DEMUX_GET_FPS:
        case DEMUX_HAS_UNSUPPORTED_META:
        case DEMUX_SET_NEXT_DEMUX_TIME:
        case DEMUX_GET_TITLE_INFO:
        case DEMUX_SET_GROUP:
        case DEMUX_SET_ES:
        case DEMUX_GET_ATTACHMENTS:
        case DEMUX_CAN_RECORD:
        case DEMUX_SET_RECORD_STATE:
        case DEMUX_GET_SIGNAL:
            return VLC_EGENERIC;
        default:
            msg_Err( s, "unknown query in demux_vaControlDefault" );
            return VLC_EGENERIC;
    }
}
