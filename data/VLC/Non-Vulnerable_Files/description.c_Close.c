 *****************************************************************************/
static void Close( vlc_object_t *p_this )
{
    sout_stream_t *p_stream = (sout_stream_t *)p_this;
    sout_stream_sys_t *p_sys = p_stream->p_sys;
    msg_Dbg( p_this, "Closing" );
    free( p_sys );
}
