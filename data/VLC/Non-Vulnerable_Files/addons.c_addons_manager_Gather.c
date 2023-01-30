}
void addons_manager_Gather( addons_manager_t *p_manager, const char *psz_uri )
{
    if ( !psz_uri )
        return;
    vlc_mutex_lock( &p_manager->p_priv->finder.lock );
    ARRAY_APPEND( p_manager->p_priv->finder.uris, strdup( psz_uri ) );
    if( !p_manager->p_priv->finder.b_live )
    {
        if( vlc_clone( &p_manager->p_priv->finder.thread, FinderThread, p_manager,
                       VLC_THREAD_PRIORITY_LOW ) )
        {
            msg_Err( p_manager->p_priv->p_parent,
                     "cannot spawn entries provider thread" );
            vlc_mutex_unlock( &p_manager->p_priv->finder.lock );
            return;
        }
        p_manager->p_priv->finder.b_live = true;
    }
    vlc_mutex_unlock( &p_manager->p_priv->finder.lock );
    vlc_cond_signal( &p_manager->p_priv->finder.waitcond );
}
