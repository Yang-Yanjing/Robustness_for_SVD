 */
static int Open( vlc_object_t * p_this )
{
    demux_t *p_demux = (demux_t*)p_this;
    /* Accept only if forced */
    if( !p_demux->b_force )
        return VLC_EGENERIC;
    char *access = var_InheritString( p_demux, "demuxdump-access" );
    if( access == NULL )
        return VLC_EGENERIC;
    /* --sout-file-append (defaults to false) */
    var_Create( p_demux, "sout-file-append", VLC_VAR_BOOL );
    if( var_InheritBool( p_demux, "demuxdump-append" ) )
        var_SetBool( p_demux, "sout-file-append", true );
    /* --sout-file-format (always false) */
    var_Create( p_demux, "sout-file-format", VLC_VAR_BOOL );
    char *path = var_InheritString( p_demux, "demuxdump-file" );
    if( path == NULL )
    {
        free( access );
        msg_Err( p_demux, "no dump file name given" );
        return VLC_EGENERIC;
    }
    sout_access_out_t *out = sout_AccessOutNew( p_demux, access, path );
    free( path );
    free( access );
    if( out == NULL )
    {
        msg_Err( p_demux, "cannot create output" );
        return VLC_EGENERIC;
    }
    p_demux->p_sys = (void *)out;
    p_demux->pf_demux = Demux;
    p_demux->pf_control = Control;
    return VLC_SUCCESS;
}
