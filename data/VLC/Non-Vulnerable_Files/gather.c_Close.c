 *****************************************************************************/
static void Close( vlc_object_t * p_this )
{
    sout_stream_t     *p_stream = (sout_stream_t*)p_this;
    sout_stream_sys_t *p_sys = p_stream->p_sys;
    int i;
    for( i = 0; i < p_sys->i_id; i++ )
    {
        sout_stream_id_sys_t *id = p_sys->id[i];
        sout_StreamIdDel( p_stream->p_next, id->id );
        es_format_Clean( &id->fmt );
        free( id );
    }
    TAB_CLEAN( p_sys->i_id, p_sys->id );
    free( p_sys );
}
