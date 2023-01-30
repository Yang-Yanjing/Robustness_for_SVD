}
static void libvlc_unlock (libvlc_int_t *p_libvlc)
{
    vlc_mutex_unlock (&(libvlc_priv (p_libvlc)->structure_lock));
}
