 *****************************************************************************/
int Import_ASX( vlc_object_t *p_this )
{
    demux_t *p_demux = (demux_t *)p_this;
    const uint8_t *p_peek;
    CHECK_PEEK( p_peek, 10 );
    // skip over possible leading empty lines and empty spaces
    p_peek = (uint8_t *)SkipBlanks((char *)p_peek, 6);
    if( POKE( p_peek, "<asx", 4 ) || demux_IsPathExtension( p_demux, ".asx" ) ||
        demux_IsPathExtension( p_demux, ".wax" ) || demux_IsPathExtension( p_demux, ".wvx" ) ||
        demux_IsForced( p_demux, "asx-open" ) )
    {
        ;
    }
    else
        return VLC_EGENERIC;
    STANDARD_DEMUX_INIT_MSG( "found valid ASX playlist" );
    p_demux->p_sys->psz_prefix = FindPrefix( p_demux );
    p_demux->p_sys->psz_data = NULL;
    p_demux->p_sys->i_data_len = -1;
    p_demux->p_sys->b_utf8 = false;
    p_demux->p_sys->b_skip_ads =
        var_InheritBool( p_demux, "playlist-skip-ads" );
    return VLC_SUCCESS;
}
