}
int playlist_ServicesDiscoveryAdd( playlist_t *p_playlist,
                                   const char *psz_name )
{
    /* Perform the addition */
    services_discovery_t *p_sd;
    msg_Dbg( p_playlist, "adding services_discovery %s...", psz_name );
    p_sd = vlc_sd_Create( VLC_OBJECT(p_playlist), psz_name );
    if( !p_sd )
        return VLC_ENOMEM;
    /* Free in playlist_ServicesDiscoveryRemove */
    vlc_sd_internal_t * p_sds = malloc( sizeof(*p_sds) );
    if( !p_sds )
    {
        vlc_sd_Destroy( p_sd );
        return VLC_ENOMEM;
    }
    playlist_item_t *p_node;
    /* Look for configuration chain "longname" */
    const char *psz_longname = "?";
    if( p_sd->p_cfg )
    {
        config_chain_t *cfg = p_sd->p_cfg;
        while( cfg )
        {
            if( cfg->psz_name && !strcmp( cfg->psz_name, "longname" ) )
            {
                psz_longname = cfg->psz_value;
                break;
            }
            cfg = cfg->p_next;
        }
    }
    PL_LOCK;
    p_node = playlist_NodeCreate( p_playlist, psz_longname,
                                  p_playlist->p_root, PLAYLIST_END, 0, NULL );
    PL_UNLOCK;
    vlc_event_manager_t *em = services_discovery_EventManager( p_sd );
    vlc_event_attach( em, vlc_ServicesDiscoveryItemAdded,
                      playlist_sd_item_added, p_node );
    vlc_event_attach( em, vlc_ServicesDiscoveryItemRemoved,
                      playlist_sd_item_removed, p_node );
    vlc_event_attach( em, vlc_ServicesDiscoveryItemRemoveAll,
                      playlist_sd_item_removeall, p_node );
    if( !vlc_sd_Start( p_sd ) )
    {
        vlc_sd_Destroy( p_sd );
        free( p_sds );
        return VLC_EGENERIC;
    }
    p_sds->p_sd = p_sd;
    p_sds->p_node = p_node;
    p_sds->psz_name = strdup( psz_name );
    PL_LOCK;
    TAB_APPEND( pl_priv(p_playlist)->i_sds, pl_priv(p_playlist)->pp_sds, p_sds );
    PL_UNLOCK;
    return VLC_SUCCESS;
}
