 *****************************************************************************/
static int Control( demux_t *p_demux, int i_query, va_list args )
{
    /* demux_sys_t *p_sys = p_demux->p_sys; */
    double f, *pf;
    int64_t i64;
    switch( i_query )
    {
        case DEMUX_GET_POSITION:
            if( ( i64 = stream_Size( p_demux->s ) ) > 0 )
            {
                pf = (double*) va_arg( args, double* );
                double current = stream_Tell( p_demux->s );
                *pf = current / (double)i64;
                return VLC_SUCCESS;
            }
            return VLC_EGENERIC;
        case DEMUX_SET_POSITION:
            f = (double) va_arg( args, double );
            i64 = stream_Size( p_demux->s );
            if( stream_Seek( p_demux->s, (int64_t)(i64 * f) ) || ReSynch( p_demux ) )
            {
                return VLC_EGENERIC;
            }
            return VLC_SUCCESS;
#if 0
        case DEMUX_GET_TIME:
            pi64 = (int64_t*)va_arg( args, int64_t * );
            if( p_sys->i_time < 0 )
            {
                *pi64 = 0;
                return VLC_EGENERIC;
            }
            *pi64 = p_sys->i_time;
            return VLC_SUCCESS;
#if 0
        case DEMUX_GET_LENGTH:
            pi64 = (int64_t*)va_arg( args, int64_t * );
            if( p_sys->i_mux_rate > 0 )
            {
                *pi64 = (int64_t)1000000 * ( stream_Size( p_demux->s ) / 50 ) / p_sys->i_mux_rate;
                return VLC_SUCCESS;
            }
            *pi64 = 0;
            return VLC_EGENERIC;
#endif
        case DEMUX_GET_FPS:
            pf = (double*)va_arg( args, double * );
            *pf = (double)1000000.0 / (double)p_sys->i_pcr_inc;
            return VLC_SUCCESS;
#endif
        case DEMUX_SET_TIME:
        default:
            return VLC_EGENERIC;
    }
}
