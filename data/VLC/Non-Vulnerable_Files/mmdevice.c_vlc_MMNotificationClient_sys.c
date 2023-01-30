}
static inline aout_sys_t *vlc_MMNotificationClient_sys(IMMNotificationClient *this)
{
    return (void *)(((char *)this) - offsetof(aout_sys_t, device_events));
}
