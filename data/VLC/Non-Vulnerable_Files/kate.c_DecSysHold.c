}
static void DecSysHold( decoder_sys_t *p_sys )
{
    vlc_mutex_lock( &p_sys->lock );
    p_sys->i_refcount++;
    vlc_mutex_unlock( &p_sys->lock );
}
