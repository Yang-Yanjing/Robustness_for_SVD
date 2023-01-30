 *****************************************************************************/
static int Open( vlc_object_t *p_this )
{
    demux_t      *p_demux = (demux_t*)p_this;
    demux_sys_t  *p_sys;
    char         *psz_file;
    ifo_handle_t *p_vmg_file;
    if( !p_demux->psz_file || !*p_demux->psz_file )
    {
        /* Only when selected */
        if( !p_demux->psz_access || !*p_demux->psz_access )
            return VLC_EGENERIC;
        psz_file = var_InheritString( p_this, "dvd" );
    }
    else
        psz_file = strdup( p_demux->psz_file );
#if defined( _WIN32 ) || defined( __OS2__ )
    if( psz_file != NULL )
    {
        size_t flen = strlen( psz_file );
        if( flen > 0 && psz_file[flen - 1] == '\\' )
            psz_file[flen - 1] = '\0';
    }
    else
        psz_file = strdup("");
#endif
    if( unlikely(psz_file == NULL) )
        return VLC_EGENERIC;
    /* Open dvdread */
    const char *psz_path = ToLocale( psz_file );
    dvd_reader_t *p_dvdread = DVDOpen( psz_path );
    LocaleFree( psz_path );
    if( p_dvdread == NULL )
    {
        msg_Err( p_demux, "DVDRead cannot open source: %s", psz_file );
        dialog_Fatal( p_demux, _("Playback failure"),
                      _("DVDRead could not open the disc \"%s\"."), psz_file );
        free( psz_file );
        return VLC_EGENERIC;
    }
    free( psz_file );
    /* Ifo allocation & initialisation */
    if( !( p_vmg_file = ifoOpen( p_dvdread, 0 ) ) )
    {
        msg_Warn( p_demux, "cannot open VMG info" );
        return VLC_EGENERIC;
    }
    msg_Dbg( p_demux, "VMG opened" );
    /* Fill p_demux field */
    DEMUX_INIT_COMMON(); p_sys = p_demux->p_sys;
    ps_track_init( p_sys->tk );
    p_sys->i_sar_num = 0;
    p_sys->i_sar_den = 0;
    p_sys->i_title_cur_time = (mtime_t) 0;
    p_sys->i_cell_cur_time = (mtime_t) 0;
    p_sys->i_cell_duration = (mtime_t) 0;
    p_sys->p_dvdread = p_dvdread;
    p_sys->p_vmg_file = p_vmg_file;
    p_sys->p_title = NULL;
    p_sys->p_vts_file = NULL;
    p_sys->i_title = p_sys->i_chapter = -1;
    p_sys->i_mux_rate = 0;
    p_sys->i_angle = var_CreateGetInteger( p_demux, "dvdread-angle" );
    if( p_sys->i_angle <= 0 ) p_sys->i_angle = 1;
    DemuxTitles( p_demux, &p_sys->i_angle );
    if( DvdReadSetArea( p_demux, 0, 0, p_sys->i_angle ) != VLC_SUCCESS )
    {
        msg_Err( p_demux, "DvdReadSetArea(0,0,%i) failed (can't decrypt DVD?)",
                 p_sys->i_angle );
        Close( p_this );
        return VLC_EGENERIC;
    }
    return VLC_SUCCESS;
}
