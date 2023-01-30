}
int config_AutoSaveConfigFile( vlc_object_t *p_this )
{
    int ret = 0;
    assert( p_this );
    vlc_rwlock_rdlock (&config_lock);
    if (config_dirty)
    {
        /* Note: this will get the read lock recursively. Ok. */
        ret = config_SaveConfigFile (p_this);
        config_dirty = (ret != 0);
    }
    vlc_rwlock_unlock (&config_lock);
    return ret;
}
