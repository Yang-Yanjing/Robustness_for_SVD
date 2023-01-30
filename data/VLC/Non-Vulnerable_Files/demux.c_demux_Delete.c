 *****************************************************************************/
void demux_Delete( demux_t *p_demux )
{
    stream_t *s;
    module_unneed( p_demux, p_demux->p_module );
    free( p_demux->psz_file );
    free( p_demux->psz_location );
    free( p_demux->psz_demux );
    free( p_demux->psz_access );
    s = p_demux->s;
    vlc_object_release( p_demux );
    if( s != NULL )
        stream_Delete( s );
}
