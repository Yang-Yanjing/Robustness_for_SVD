 *****************************************************************************/
static int OpenCommon( vlc_object_t *p_this )
{
    sout_stream_t     *p_stream = (sout_stream_t*)p_this;
    sout_stream_sys_t *p_sys;
    if( !p_stream->p_next )
    {
        msg_Err( p_stream, "cannot create chain" );
        return VLC_EGENERIC;
    }
    p_sys = malloc( sizeof( sout_stream_sys_t ) );
    if( unlikely( !p_sys ) )
        return VLC_ENOMEM;
    p_stream->pf_del    = Del;
    p_stream->pf_send   = Send;
    p_stream->p_sys     = p_sys;
    return VLC_SUCCESS;
}
