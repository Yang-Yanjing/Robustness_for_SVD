}
static void Close(vlc_object_t *obj)
{
    audio_output_t *aout = (audio_output_t *)obj;
    aout_sys_t *sys = aout->sys;
    EnterCriticalSection(&sys->lock);
    sys->device = default_device; /* break out of MMSession() loop */
    sys->it = NULL; /* break out of MMThread() loop */
    WakeConditionVariable(&sys->work);
    LeaveCriticalSection(&sys->lock);
    vlc_join(sys->thread, NULL);
    DeleteCriticalSection(&sys->lock);
    free(sys);
}
