 *****************************************************************************/
static void Close( vlc_object_t * p_this )
{
    sout_stream_t *p_stream = (sout_stream_t*)p_this;
    free( p_stream->p_sys );
}
