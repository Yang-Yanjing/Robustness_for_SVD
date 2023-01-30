 *****************************************************************************/
static int AccessDemuxOpen ( vlc_object_t *p_this )
{
    demux_t *p_demux = (demux_t*)p_this;
    dvdnav_t *p_dvdnav = NULL;
    char *psz_file = NULL;
    const char *psz_path = NULL;
    int i_ret = VLC_EGENERIC;
    bool forced = false;
    if( p_demux->psz_access != NULL
     && !strncmp(p_demux->psz_access, "dvd", 3) )
        forced = true;
    if( !p_demux->psz_file || !*p_demux->psz_file )
    {
        /* Only when selected */
        if( !forced )
            return VLC_EGENERIC;
        psz_file = var_InheritString( p_this, "dvd" );
    }
    else
        psz_file = strdup( p_demux->psz_file );
#if defined( _WIN32 ) || defined( __OS2__ )
    if( psz_file != NULL )
    {
        /* Remove trailing backslash, otherwise dvdnav_open will fail */
        size_t flen = strlen( psz_file );
        if( flen > 0 && psz_file[flen - 1] == '\\' )
            psz_file[flen - 1] = '\0';
    }
    else
        psz_file = strdup("");
#endif
    if( unlikely(psz_file == NULL) )
        return VLC_EGENERIC;
    /* Try some simple probing to avoid going through dvdnav_open too often */
    if( !forced && ProbeDVD( psz_file ) != VLC_SUCCESS )
        goto bailout;
    /* Open dvdnav */
    psz_path = ToLocale( psz_file );
    if( dvdnav_open( &p_dvdnav, psz_path ) != DVDNAV_STATUS_OK )
    {
        msg_Warn( p_demux, "cannot open DVD (%s)", psz_file);
        goto bailout;
    }
    i_ret = CommonOpen( p_this, p_dvdnav, !!DVD_READ_CACHE );
    if( i_ret != VLC_SUCCESS )
        dvdnav_close( p_dvdnav );
bailout:
    free( psz_file );
    if( psz_path )
        LocaleFree( psz_path );
    return i_ret;
}
