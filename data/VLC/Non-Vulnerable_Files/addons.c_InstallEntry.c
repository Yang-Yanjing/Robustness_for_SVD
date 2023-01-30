}
static int InstallEntry( addons_manager_t *p_manager, addon_entry_t *p_entry )
{
    if ( p_entry->e_type == ADDON_UNKNOWN ||
         p_entry->e_type == ADDON_PLUGIN ||
         p_entry->e_type == ADDON_OTHER )
        return VLC_EBADVAR;
    vlc_mutex_lock( &p_manager->p_priv->installer.lock );
    ARRAY_APPEND( p_manager->p_priv->installer.entries, p_entry );
    if( !p_manager->p_priv->installer.b_live )
    {
        if( vlc_clone( &p_manager->p_priv->installer.thread, InstallerThread, p_manager,
                       VLC_THREAD_PRIORITY_LOW ) )
        {
            msg_Err( p_manager->p_priv->p_parent,
                     "cannot spawn addons installer thread" );
            vlc_mutex_unlock( &p_manager->p_priv->installer.lock );
            return VLC_EGENERIC;
        }
        else
            p_manager->p_priv->installer.b_live = true;
    }
    vlc_mutex_unlock( &p_manager->p_priv->installer.lock );
    vlc_cond_signal( &p_manager->p_priv->installer.waitcond );
    return VLC_SUCCESS;
}
