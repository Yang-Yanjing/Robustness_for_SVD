 *****************************************************************************/
static int Control( demux_t *p_demux, int i_query, va_list args )
{
    int64_t i64, *pi64, i_sample;
    double f, *pf;
    frame_span_t position;
    demux_sys_t *p_sys  = p_demux->p_sys;
    uint64_t i_num_samples = TotalNumSamples( p_demux );
    switch( i_query )
    {
        case DEMUX_GET_LENGTH:
            pi64 = ( int64_t* )va_arg( args, int64_t * );
            *pi64 = CLOCK_FREQ * ( i_num_samples / p_sys->fmt.audio.i_rate );
            return VLC_SUCCESS;
        case DEMUX_GET_TIME:
            pi64 = ( int64_t* )va_arg( args, int64_t * );
            *pi64 = CLOCK_FREQ * ( p_sys->position.i_samples / p_sys->fmt.audio.i_rate );
            return VLC_SUCCESS;
        case DEMUX_GET_POSITION:
            pf = (double*)va_arg( args, double * );
            *pf = i_num_samples ? (double)p_sys->position.i_samples / (double)i_num_samples : 0.0;
            return VLC_SUCCESS;
        case DEMUX_SET_POSITION:
            f = (double)va_arg( args, double );
            i_sample = f * i_num_samples;
            if( SetSpanWithSample( p_demux, &position, i_sample ))
                return VLC_EGENERIC;
            p_sys->position = position;
            return VLC_SUCCESS;
        case DEMUX_SET_TIME:
            i64 = (int64_t)va_arg( args, int64_t );
            i_sample = i64 * p_sys->fmt.audio.i_rate / INT64_C( 1000000 );
            if( SetSpanWithSample( p_demux, &position, i_sample ))
                return VLC_EGENERIC;
            p_sys->position = position;
            return VLC_SUCCESS;
        case DEMUX_GET_META:
            return stream_Control( p_demux->s, STREAM_GET_META, args );
        default:
            return VLC_EGENERIC;
    }
    return VLC_EGENERIC;
}
