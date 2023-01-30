 *****************************************************************************/
static int Control( demux_t *p_demux, int i_query, va_list args )
{
    demux_sys_t *p_sys = p_demux->p_sys;
    double f, *pf;
    int64_t *pi64;
    switch( i_query )
    {
        case DEMUX_GET_POSITION:
        {
            int64_t i_start = p_sys->i_ssnd_start;
            int64_t i_end   = p_sys->i_ssnd_end > 0 ? p_sys->i_ssnd_end : stream_Size( p_demux->s );
            int64_t i_tell  = stream_Tell( p_demux->s );
            pf = (double*) va_arg( args, double* );
            if( i_start < i_end )
            {
                *pf = (double)(i_tell - i_start)/(double)(i_end - i_start);
                return VLC_SUCCESS;
            }
            return VLC_EGENERIC;
        }
        case DEMUX_SET_POSITION:
        {
            int64_t i_start = p_sys->i_ssnd_start;
            int64_t i_end  = p_sys->i_ssnd_end > 0 ? p_sys->i_ssnd_end : stream_Size( p_demux->s );
            f = (double) va_arg( args, double );
            if( i_start < i_end )
            {
                int     i_frame = (f * ( i_end - i_start )) / p_sys->i_ssnd_fsize;
                int64_t i_new   = i_start + i_frame * p_sys->i_ssnd_fsize;
                if( stream_Seek( p_demux->s, i_new ) )
                {
                    return VLC_EGENERIC;
                }
                p_sys->i_time = (int64_t)1000000 * i_frame / p_sys->fmt.audio.i_rate;
                return VLC_SUCCESS;
            }
            return VLC_EGENERIC;
        }
        case DEMUX_GET_TIME:
            pi64 = (int64_t*)va_arg( args, int64_t * );
            *pi64 = p_sys->i_time;
            return VLC_SUCCESS;
        case DEMUX_GET_LENGTH:
        {
            int64_t i_end  = p_sys->i_ssnd_end > 0 ? p_sys->i_ssnd_end : stream_Size( p_demux->s );
            pi64 = (int64_t*)va_arg( args, int64_t * );
            if( p_sys->i_ssnd_start < i_end )
            {
                *pi64 = (int64_t)1000000 * ( i_end - p_sys->i_ssnd_start ) / p_sys->i_ssnd_fsize / p_sys->fmt.audio.i_rate;
                return VLC_SUCCESS;
            }
            return VLC_EGENERIC;
        }
        case DEMUX_SET_TIME:
        case DEMUX_GET_FPS:
        default:
            return VLC_EGENERIC;
    }
}
