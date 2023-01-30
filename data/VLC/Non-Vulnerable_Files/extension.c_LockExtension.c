/** Lock this extension. Can fail. */
bool LockExtension( extension_t *p_ext )
{
    vlc_mutex_lock( &p_ext->p_sys->command_lock );
    if( p_ext->p_sys->b_exiting )
    {
        vlc_mutex_unlock( &p_ext->p_sys->command_lock );
        return false;
    }
    vlc_mutex_lock( &p_ext->p_sys->running_lock );
    if( p_ext->p_sys->b_exiting )
    {
        vlc_mutex_unlock( &p_ext->p_sys->running_lock );
        vlc_mutex_unlock( &p_ext->p_sys->command_lock );
        return false;
    }
    vlc_mutex_unlock( &p_ext->p_sys->command_lock );
    return true;
}
