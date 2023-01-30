/** Unlock this extension. */
void UnlockExtension( extension_t *p_ext )
{
    vlc_mutex_unlock( &p_ext->p_sys->running_lock );
}
