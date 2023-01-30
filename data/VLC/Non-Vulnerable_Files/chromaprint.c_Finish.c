}
static void Finish( sout_stream_t *p_stream )
{
    sout_stream_sys_t *p_sys = p_stream->p_sys;
    char *psz_fingerprint = NULL;
    if ( p_sys->b_finished && chromaprint_finish( p_sys->p_chromaprint_ctx ) )
    {
        chromaprint_get_fingerprint( p_sys->p_chromaprint_ctx,
                                     &psz_fingerprint );
        if ( psz_fingerprint )
        {
            p_sys->p_data->i_duration = p_sys->i_total_samples / p_sys->id->i_samplerate;
            p_sys->p_data->psz_fingerprint = strdup( psz_fingerprint );
            chromaprint_dealloc( psz_fingerprint );
            msg_Dbg( p_stream, "DURATION=%u;FINGERPRINT=%s",
                    p_sys->p_data->i_duration,
                    p_sys->p_data->psz_fingerprint );
        }
    } else {
        msg_Dbg( p_stream, "Cannot create %us fingerprint (not enough samples?)",
                 p_sys->i_duration );
    }
    p_sys->b_done = true;
    msg_Dbg( p_stream, "Fingerprinting finished" );
}
