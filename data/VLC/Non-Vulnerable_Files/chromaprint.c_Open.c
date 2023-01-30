 *****************************************************************************/
static int Open( vlc_object_t *p_this )
{
    sout_stream_t *p_stream = (sout_stream_t*)p_this;
    sout_stream_sys_t *p_sys;
    p_stream->p_sys = p_sys = malloc(sizeof(sout_stream_sys_t));
    if ( unlikely( ! p_sys ) ) return VLC_ENOMEM;
    p_sys->id = NULL;
    p_sys->b_finished = false;
    p_sys->b_done = false;
    p_sys->i_total_samples = 0;
    p_sys->i_duration = var_InheritInteger( p_stream, "duration" );
    p_sys->p_data = var_InheritAddress( p_stream, "fingerprint-data" );
    if ( !p_sys->p_data )
    {
        msg_Err( p_stream, "Fingerprint data holder not set" );
        free( p_sys );
        return VLC_ENOVAR;
    }
    msg_Dbg( p_stream, "chromaprint version %s", chromaprint_get_version() );
    p_sys->p_chromaprint_ctx = chromaprint_new( CHROMAPRINT_ALGORITHM_DEFAULT );
    if ( ! p_sys->p_chromaprint_ctx )
    {
        msg_Err( p_stream, "Can't create chromaprint context" );
        free( p_sys );
        return VLC_EGENERIC;
    }
    p_stream->pf_add  = Add;
    p_stream->pf_del  = Del;
    p_stream->pf_send = Send;
    return VLC_SUCCESS;
}
