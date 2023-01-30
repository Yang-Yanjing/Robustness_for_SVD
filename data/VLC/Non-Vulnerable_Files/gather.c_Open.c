 *****************************************************************************/
static int Open( vlc_object_t *p_this )
{
    sout_stream_t     *p_stream = (sout_stream_t*)p_this;
    sout_stream_sys_t *p_sys;
    p_stream->p_sys = p_sys = malloc( sizeof( sout_stream_sys_t ) );
    if( p_sys == NULL )
        return VLC_EGENERIC;
    if( !p_stream->p_next )
    {
        free( p_sys );
        return VLC_EGENERIC;
    }
    p_stream->pf_add    = Add;
    p_stream->pf_del    = Del;
    p_stream->pf_send   = Send;
    TAB_INIT( p_sys->i_id, p_sys->id );
    return VLC_SUCCESS;
}
