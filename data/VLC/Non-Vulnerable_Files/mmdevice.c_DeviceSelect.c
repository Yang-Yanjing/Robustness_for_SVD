}
static int DeviceSelect(audio_output_t *aout, const char *id)
{
    aout_sys_t *sys = aout->sys;
    wchar_t *device;
    if (id != NULL)
    {
        device = ToWide(id);
        if (unlikely(device == NULL))
            return -1;
    }
    else
        device = default_device;
    EnterCriticalSection(&sys->lock);
    assert(sys->device == NULL);
    sys->device = device;
    WakeConditionVariable(&sys->work);
    while (sys->device != NULL)
        SleepConditionVariableCS(&sys->ready, &sys->lock, INFINITE);
    LeaveCriticalSection(&sys->lock);
    if (sys->stream != NULL && sys->dev != NULL)
        /* Request restart of stream with the new device */
        aout_RestartRequest(aout, AOUT_RESTART_OUTPUT);
    return (sys->dev != NULL) ? 0 : -1;
}
