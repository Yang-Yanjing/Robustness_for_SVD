 *****************************************************************************/
static int Open( vlc_object_t *p_this )
{
    demux_t     *p_demux = (demux_t*)p_this;
    demux_sys_t *p_sys;
    if( p_demux->psz_file == NULL )
        return VLC_EGENERIC;
    if( *p_demux->psz_access &&
        strcmp( p_demux->psz_access, "bd" ) &&
        strcmp( p_demux->psz_access, "file" ) )
        return VLC_EGENERIC;
    /* */
    bool b_shortname;
    char *psz_base = FindPathBase( p_demux->psz_file, &b_shortname );
    if( !psz_base )
        return VLC_EGENERIC;
    msg_Dbg( p_demux, "Using path '%s'", psz_base );
    /* Fill p_demux field */
    p_demux->p_sys = p_sys = malloc( sizeof(*p_sys) );
    if( !p_sys )
        return VLC_EGENERIC;
    p_sys->psz_base = psz_base;
    p_sys->b_shortname = b_shortname;
    TAB_INIT( p_sys->i_mpls, p_sys->pp_mpls );
    TAB_INIT( p_sys->i_clpi, p_sys->pp_clpi );
    TAB_INIT( p_sys->i_title, p_sys->pp_title );
    p_demux->info.i_title = -1;
    p_sys->p_clpi = NULL;
    p_sys->i_clpi_ep = -1;
    p_sys->p_parser = NULL;
    p_sys->p_m2ts = NULL;
    p_sys->i_play_item = -1;
    p_sys->i_packet = -1;
    p_sys->i_packet_start = -1;
    p_sys->i_packet_stop = -1;
    p_sys->i_packet_headers = -1;
    p_sys->p_out = EsOutNew( p_demux );
    if( !p_sys->p_out )
        goto error;
    p_demux->pf_control = Control;
    p_demux->pf_demux = Demux;
    /* Load all clip/playlist files */
    LoadClip( p_demux );
    LoadPlaylist( p_demux );
    /* Reorder playlist to have the most significant first
     * (as we don't have menu support, no idea how to find the main title */
    ReorderPlaylist( p_demux );
    /* Setup variables (for TS demuxer) */
    var_Create( p_demux, "ts-es-id-pid", VLC_VAR_BOOL );
    var_SetBool( p_demux, "ts-es-id-pid", true );
    /* */
    InitTitles( p_demux );
    if( SetTitle( p_demux, 0 ) )
        goto error;
    return VLC_SUCCESS;
error:
    Close( VLC_OBJECT(p_demux) );
    return VLC_EGENERIC;
}
