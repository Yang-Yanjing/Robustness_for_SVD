}
void playlist_ServicesDiscoveryKillAll( playlist_t *p_playlist )
{
    playlist_private_t *priv = pl_priv( p_playlist );
    while( priv->i_sds > 0 )
        playlist_ServicesDiscoveryRemove( p_playlist,
                                          priv->pp_sds[0]->psz_name );
}
