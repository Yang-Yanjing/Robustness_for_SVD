}
int playlist_ServicesDiscoveryControl( playlist_t *p_playlist, const char *psz_name, int i_control, ... )
{
    playlist_private_t *priv = pl_priv( p_playlist );
    int i_ret = VLC_EGENERIC;
    int i;
    PL_LOCK;
    for( i = 0; i < priv->i_sds; i++ )
    {
        vlc_sd_internal_t *sd = priv->pp_sds[i];
        if( sd->psz_name && !strcmp( psz_name, sd->psz_name ) )
        {
            va_list args;
            va_start( args, i_control );
            i_ret = vlc_sd_control( sd->p_sd, i_control, args );
            va_end( args );
            break;
        }
    }
    assert( i != priv->i_sds );
    PL_UNLOCK;
    return i_ret;
}
