/*** Audio session events ***/
static inline aout_sys_t *vlc_AudioSessionEvents_sys(IAudioSessionEvents *this)
{
    return (void *)(((char *)this) - offsetof(aout_sys_t, session_events));
}
