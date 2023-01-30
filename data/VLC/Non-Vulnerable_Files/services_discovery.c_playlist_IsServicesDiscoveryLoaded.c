}
bool playlist_IsServicesDiscoveryLoaded( playlist_t * p_playlist,
                                         const char *psz_name )
{
    playlist_private_t *priv = pl_priv( p_playlist );
    bool found = false;
    PL_LOCK;
    for( int i = 0; i < priv->i_sds; i++ )
    {
        vlc_sd_internal_t *sd = priv->pp_sds[i];
        if( sd->psz_name && !strcmp( psz_name, sd->psz_name ) )
        {
            found = true;
            break;
        }
    }
    PL_UNLOCK;
    return found;
}
