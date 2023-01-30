};
static inline aout_sys_t *vlc_AudioVolumeDuckNotification_sys(IAudioVolumeDuckNotification *this)
{
    return (void *)(((char *)this) - offsetof(aout_sys_t, duck));
}
