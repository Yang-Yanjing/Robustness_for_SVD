}
static void *MMThread(void *data)
{
    audio_output_t *aout = data;
    aout_sys_t *sys = aout->sys;
    IMMDeviceEnumerator *it = sys->it;
    EnterMTA();
    IMMDeviceEnumerator_RegisterEndpointNotificationCallback(it,
                                                          &sys->device_events);
    DevicesEnum(aout, it);
    EnterCriticalSection(&sys->lock);
    do
        if (FAILED(MMSession(aout, it)))
            SleepConditionVariableCS(&sys->work, &sys->lock, INFINITE);
    while (sys->it != NULL);
    LeaveCriticalSection(&sys->lock);
    IMMDeviceEnumerator_UnregisterEndpointNotificationCallback(it,
                                                          &sys->device_events);
    IMMDeviceEnumerator_Release(it);
    LeaveMTA();
    return NULL;
}
