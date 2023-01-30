}
static int DeviceSelect (audio_output_t *aout, const char *id)
{
    aout_sys_t *sys = aout->sys;
    char *device = strdup (id ? id : "default");
    if (unlikely(device == NULL))
        return -1;
    free (sys->device);
    sys->device = device;
    aout_DeviceReport (aout, device);
    aout_RestartRequest (aout, AOUT_RESTART_OUTPUT);
    return 0;
}
