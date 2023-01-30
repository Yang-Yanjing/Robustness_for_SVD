}
static int SetupAvioCb(vlc_object_t *access)
{
    static vlc_mutex_t avio_lock = VLC_STATIC_MUTEX;
    vlc_mutex_lock(&avio_lock);
    assert(!access != !current_access);
    if (access && current_access) {
        vlc_mutex_unlock(&avio_lock);
        return VLC_EGENERIC;
    }
    url_set_interrupt_cb(access ? UrlInterruptCallbackSingle : NULL);
    current_access = access;
    vlc_mutex_unlock(&avio_lock);
    return VLC_SUCCESS;
}
