 *****************************************************************************/
static int Control( demux_t *p_demux, int i_query, va_list args )
{
    demux_sys_t *p_sys = p_demux->p_sys;
    double   f, *pf;
    int64_t i64, *pi64;
    bool *pb_bool;
    switch( i_query )
    {
        case DEMUX_HAS_UNSUPPORTED_META:
            pb_bool = (bool*)va_arg( args, bool* );
            *pb_bool = true;
            return VLC_SUCCESS;
        case DEMUX_GET_LENGTH:
            pi64 = (int64_t*)va_arg( args, int64_t * );
#ifndef HAVE_MPC_MPCDEC_H
            *pi64 = INT64_C(1000000) * p_sys->info.pcm_samples /
                        p_sys->info.sample_freq;
#else
            *pi64 = INT64_C(1000000) * (p_sys->info.samples -
                                        p_sys->info.beg_silence) /
                p_sys->info.sample_freq;
#endif
            return VLC_SUCCESS;
        case DEMUX_GET_POSITION:
            pf = (double*)va_arg( args, double * );
#ifndef HAVE_MPC_MPCDEC_H
            if( p_sys->info.pcm_samples > 0 )
                *pf = (double) p_sys->i_position /
                      (double)p_sys->info.pcm_samples;
#else
            if( p_sys->info.samples - p_sys->info.beg_silence > 0)
                *pf = (double) p_sys->i_position /
                      (double)(p_sys->info.samples - p_sys->info.beg_silence);
#endif
            else
                *pf = 0.0;
            return VLC_SUCCESS;
        case DEMUX_GET_TIME:
            pi64 = (int64_t*)va_arg( args, int64_t * );
            *pi64 = INT64_C(1000000) * p_sys->i_position /
                        p_sys->info.sample_freq;
            return VLC_SUCCESS;
        case DEMUX_SET_POSITION:
            f = (double)va_arg( args, double );
#ifndef HAVE_MPC_MPCDEC_H
            i64 = (int64_t)(f * p_sys->info.pcm_samples);
            if( mpc_decoder_seek_sample( &p_sys->decoder, i64 ) )
#else
            i64 = (int64_t)(f * (p_sys->info.samples -
                                 p_sys->info.beg_silence));
            if( mpc_demux_seek_sample( p_sys->decoder, i64 ) == MPC_STATUS_OK )
#endif
            {
                p_sys->i_position = i64;
                return VLC_SUCCESS;
            }
            return VLC_EGENERIC;
        case DEMUX_SET_TIME:
            i64 = (int64_t)va_arg( args, int64_t );
#ifndef HAVE_MPC_MPCDEC_H
            if( mpc_decoder_seek_sample( &p_sys->decoder, i64 ) )
#else
             if( mpc_demux_seek_sample( p_sys->decoder, i64 ) == MPC_STATUS_OK )
#endif
            {
                p_sys->i_position = i64;
                return VLC_SUCCESS;
            }
            return VLC_EGENERIC;
        default:
            return VLC_EGENERIC;
    }
}
