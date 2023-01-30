 *****************************************************************************/
static int Control( demux_t *p_demux, int i_query, va_list args )
{
    demux_sys_t *p_sys  = p_demux->p_sys;
    double   f, *pf;
    int64_t i64, *pi64;
    switch( i_query )
    {
        case DEMUX_GET_POSITION:
            pf = (double*)va_arg( args, double * );
            if( p_sys->i_total_length > 0 && p_sys->i_pcr >= 0 )
            {
                *pf = (double)p_sys->i_pcr / (double)p_sys->i_total_length;
            }
            else
            {
                i64 = stream_Size( p_demux->s );
                if( i64 > 0 )
                {
                    const double f_current = stream_Tell( p_demux->s );
                    *pf = f_current / (double)i64;
                }
                else
                {
                    *pf = 0.0;
                }
            }
            return VLC_SUCCESS;
        case DEMUX_SET_POSITION:
        {
            int64_t i_pos;
            f = (double)va_arg( args, double );
            p_sys->i_pcr = -1;
            /* first try to see if we can seek based on time (== GET_LENGTH works) */
            if( p_sys->i_total_length > 0 && ( i_pos = demux_IndexConvertTime( &p_sys->idx, p_sys->i_total_length * f ) ) > 0 )
                return ControlSetPosition( p_demux, i_pos, false );
            /* if not search based on total stream size */
            else if( ( i_pos = demux_IndexFindOffset( &p_sys->idx, stream_Size( p_demux->s ) * f ) ) >= 0 )
                return ControlSetPosition( p_demux, i_pos, false );
            else if( ( i_pos =  p_sys->i_first_frame_offset + ( stream_Size( p_demux->s ) - p_sys->i_first_frame_offset ) * f ) >= 0 )
                return ControlSetPosition( p_demux, i_pos, true );
            else
                return VLC_EGENERIC;
        }
        case DEMUX_GET_TIME:
            pi64 = (int64_t*)va_arg( args, int64_t * );
            *pi64 = p_sys->i_pcr >= 0 ? p_sys->i_pcr : 0;
            return VLC_SUCCESS;
        case DEMUX_SET_TIME:
        {
            int64_t i_pos;
            i64 = (int64_t)va_arg( args, int64_t );
            p_sys->i_pcr = -1;
            i_pos = demux_IndexConvertTime( &p_sys->idx, i64 );
            if( i_pos < 0 )
                return VLC_EGENERIC;
            else
                return ControlSetPosition( p_demux, i_pos, false );
        }
        case DEMUX_GET_LENGTH:
            pi64 = (int64_t*)va_arg( args, int64_t * );
            if( p_sys->i_total_length >= 0 )
            {
                *pi64 = p_sys->i_total_length;
                return VLC_SUCCESS;
            }
            else if( stream_Tell( p_demux->s ) > p_sys->i_first_frame_offset )
            {
                /* This should give an approximation of the total duration */
                *pi64 = (double)( stream_Size( p_demux->s ) - p_sys->i_first_frame_offset ) /
                        (double)( stream_Tell( p_demux->s ) - p_sys->i_first_frame_offset )
                        * (double)( p_sys->i_pcr >= 0 ? p_sys->i_pcr : 0 );
                return VLC_SUCCESS;
            }
            else
                return VLC_EGENERIC;
        case DEMUX_GET_FPS:
            pf = (double*)va_arg( args, double * );
            *pf = p_sys->hdr.d_fps;
            return VLC_SUCCESS;
        case DEMUX_GET_META:
        default:
            return VLC_EGENERIC;
    }
}
