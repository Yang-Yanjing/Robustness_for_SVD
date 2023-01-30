}
static int DeviceSelect (audio_output_t *aout, const char *id)
{
    aout_sys_t *sys = aout->sys;
    char *path = NULL;
    if (id != NULL)
    {
        path = strdup (id);
        if (unlikely(path == NULL))
            return -1;
    }
    free (sys->device);
    sys->device = path;
    aout_DeviceReport (aout, path);
    aout_RestartRequest (aout, AOUT_RESTART_OUTPUT);
    return 0;
}
