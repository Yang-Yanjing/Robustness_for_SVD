 *****************************************************************************/
static void Close( vlc_object_t *p_this )
{
    sout_stream_t *p_stream = (sout_stream_t*)p_this;
    sout_stream_sys_t *p_sys = p_stream->p_sys;
    SendFlush( p_this );
    SendTeardown( p_this );
    FreeSys( p_this, p_sys );
}
