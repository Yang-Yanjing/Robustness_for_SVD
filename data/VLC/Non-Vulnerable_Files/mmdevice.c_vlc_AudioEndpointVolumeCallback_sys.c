/*** Audio endpoint volume ***/
static inline aout_sys_t *vlc_AudioEndpointVolumeCallback_sys(IAudioEndpointVolumeCallback *this)
{
    return (void *)(((char *)this) - offsetof(aout_sys_t, endpoint_callback));
}
