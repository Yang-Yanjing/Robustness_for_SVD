}
int playlist_ServicesDiscoveryRemove( playlist_t * p_playlist,
                                      const char *psz_name )
{
    playlist_private_t *priv = pl_priv( p_playlist );
    vlc_sd_internal_t * p_sds = NULL;
    PL_LOCK;
    for( int i = 0; i < priv->i_sds; i++ )
    {
        if( !strcmp( psz_name, priv->pp_sds[i]->psz_name ) )
        {
            p_sds = priv->pp_sds[i];
            REMOVE_ELEM( priv->pp_sds, priv->i_sds, i );
            break;
        }
    }
    PL_UNLOCK;
    if( !p_sds )
    {
        msg_Warn( p_playlist, "discovery %s is not loaded", psz_name );
        return VLC_EGENERIC;
    }
    services_discovery_t *p_sd = p_sds->p_sd;
    assert( p_sd );
    vlc_sd_Stop( p_sd );
    vlc_event_detach( services_discovery_EventManager( p_sd ),
                        vlc_ServicesDiscoveryItemAdded,
                        playlist_sd_item_added,
                        p_sds->p_node );
    vlc_event_detach( services_discovery_EventManager( p_sd ),
                        vlc_ServicesDiscoveryItemRemoved,
                        playlist_sd_item_removed,
                        p_sds->p_node );
    /* Remove the sd playlist node if it exists */
    PL_LOCK;
    playlist_NodeDelete( p_playlist, p_sds->p_node, true, false );
    PL_UNLOCK;
    vlc_sd_Destroy( p_sd );
    free( p_sds->psz_name );
    free( p_sds );
    return VLC_SUCCESS;
}
