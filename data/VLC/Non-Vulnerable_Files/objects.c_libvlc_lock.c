 *****************************************************************************/
static void libvlc_lock (libvlc_int_t *p_libvlc)
{
    vlc_mutex_lock (&(libvlc_priv (p_libvlc)->structure_lock));
}
